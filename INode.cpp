#include "Utility.h"
#include "INode.h"
#include "FileSystem.h"
#include "User.h"

extern BufferManager g_BufferManager;
extern FileSystem g_FileSystem;
extern User g_User;

DiskINode::DiskINode() {
    this->d_mode = 0;
    this->d_nlink = 0;
    this->d_uid = -1;
    this->d_gid = -1;
    this->d_size = 0;
	Utility::memset(d_addr, 0, sizeof(d_addr));
    this->d_atime = 0;
    this->d_mtime = 0;
}

DiskINode::~DiskINode() {
}

INode::INode() {
	this->i_flag = 0;
	this->i_mode = 0;
	this->i_count = 0;
	this->i_nlink = 0;
	this->i_number = -1;
	this->i_uid = -1;
	this->i_gid = -1;
	this->i_size = 0;
	this->i_lastr = -1;
	Utility::memset(i_addr, 0, sizeof(i_addr));
}

INode::~INode() {
}

/* 根据Inode对象中的物理磁盘块索引表，读取相应的文件数据 */
void INode::ReadI() {
	User& u = g_User;
	BufferManager& bufferManager = g_BufferManager;
	int lbn, bn; //lbn: 逻辑块号  bn:物理块号
	int offset, nbytes;
	Buffer* pBuffer;

	/* 需要读字节数为零，则返回 */
	if (0 == u.IOParam.count) {
		return;
	}
	this->i_flag |= INode::IACC;  //标记该INode被访问过，需要修改最近一次访问时间

	while (User::U_NOERROR == u.u_error && u.IOParam.count) {
		lbn = bn = u.IOParam.offset / INode::BLOCK_SIZE;
		offset = u.IOParam.offset % INode::BLOCK_SIZE;
		
		/* 传送到用户区的字节数量，取读请求的剩余字节数与当前字符块内有效字节数较小值 */
		nbytes = Utility::min(INode::BLOCK_SIZE - offset /* 块内有效字节数 */, u.IOParam.count);
		int remain = this->i_size - u.IOParam.offset;//文件的size减去已读取的offset, 等于文件中还没有读的字节数
		if (remain <= 0) {
			return;
		}
		
		/* 传送的字节数量还取决于剩余文件的长度 */
		nbytes = Utility::min(nbytes, remain);//min后, nbytes表示当前块内最多允许读的字节数
		if ((bn = this->Bmap(lbn)) == 0) {//使用Bmap函数找到逻辑块对应的物理块
			return;
		}
		
		pBuffer = bufferManager.Bread(bn); //申请一个缓存块
		this->i_lastr = lbn;

		/* 缓存中数据起始读位置 */
		unsigned char* start = pBuffer->addr + offset;  //接着上一次的位置继续读
		Utility::memcpy(u.IOParam.base, start, nbytes);  //从start位置读nbytes到base指向的区域
		u.IOParam.base += nbytes;
		u.IOParam.offset += nbytes;
		u.IOParam.count -= nbytes;

		bufferManager.Brelse(pBuffer);//释放读入文件所占用的缓存块
	}
}

/* 根据Inode对象中的物理磁盘块索引表，将数据写入文件 */
void INode::WriteI() {
	int lbn, bn;  //lbn:逻辑块号  bn:物理块号
	int offset, nbytes;
	Buffer* pBuffer;
	User& u = g_User;
	BufferManager& bufferManager = g_BufferManager;

	this->i_flag |= (INode::IACC | INode::IUPD);  //标记i结点被访问过、被修改过

	/* 需要写字节数为零，则返回 */
	if (0 == u.IOParam.count) {
		return;
	}

	while (User::U_NOERROR == u.u_error && u.IOParam.count) {
		lbn = u.IOParam.offset / INode::BLOCK_SIZE;
		offset = u.IOParam.offset % INode::BLOCK_SIZE;
		nbytes = Utility::min(INode::BLOCK_SIZE - offset, u.IOParam.count);
		if ((bn = this->Bmap(lbn)) == 0) {
			return;
		}

		if (INode::BLOCK_SIZE == nbytes) {
			/* 如果写入数据正好满一个字符块，则为其分配缓存 */
			pBuffer = bufferManager.GetBlk(bn);
		}
		else {
			/* 写入数据不满一个字符块，先读后写（读出该字符块以保护不需要重写的数据） */
			pBuffer = bufferManager.Bread(bn);
		}

		/* 缓存中数据的起始写位置 写操作: 从用户目标区拷贝数据到缓冲区 */
		unsigned char* start = pBuffer->addr + offset;
		Utility::memcpy(start, u.IOParam.base, nbytes);
		u.IOParam.base += nbytes;
		u.IOParam.offset += nbytes;
		u.IOParam.count -= nbytes;

		if (u.u_error != User::U_NOERROR) {
			bufferManager.Brelse(pBuffer);
		}

		/* 将缓存标记为延迟写，不急于进行I/O操作将字符块输出到磁盘上 */
		bufferManager.Bdwrite(pBuffer);

		/* 普通文件长度增加 */
		if (this->i_size < u.IOParam.offset) {
			this->i_size = u.IOParam.offset;
		}
		this->i_flag |= INode::IUPD;
	}
}

/* 将包含外存Inode字符块中信息拷贝到内存Inode中 */
void INode::ICopy(Buffer* pb, int inumber) {
    DiskINode& dINode = *(DiskINode*)(pb->addr + (inumber%FileSystem::INODE_NUMBER_PER_SECTOR)*sizeof(DiskINode));
    i_mode = dINode.d_mode;
    i_nlink = dINode.d_nlink;
    i_uid = dINode.d_uid;
    i_gid = dINode.d_gid;
    i_size = dINode.d_size;
    Utility::memcpy(i_addr, dINode.d_addr, sizeof(i_addr));
}

/* 将文件的逻辑块号转换成对应的物理盘块号 */
int INode::Bmap(int lbn) {
	/*
	* Unix V6++的文件索引结构：(小型、大型和巨型文件)
	* (1) i_addr[0] - i_addr[5]为直接索引表，文件长度范围是0 - 6个盘块；
	*
	* (2) i_addr[6] - i_addr[7]存放一次间接索引表所在磁盘块号，每磁盘块
	* 上存放128个文件数据盘块号，此类文件长度范围是7 - (128 * 2 + 6)个盘块；
	*
	* (3) i_addr[8] - i_addr[9]存放二次间接索引表所在磁盘块号，每个二次间接
	* 索引表记录128个一次间接索引表所在磁盘块号，此类文件长度范围是
	* (128 * 2 + 6 ) < size <= (128 * 128 * 2 + 128 * 2 + 6)
	*/
	User& u = g_User;
	BufferManager& bufferManager = g_BufferManager;
	FileSystem& fileSystem = g_FileSystem;
	Buffer* pFirstBuffer, *pSecondBuffer;
	int phyBlkno, index;
	int *iTable;

	if (lbn >= INode::HUGE_FILE_BLOCK) {
		u.u_error = User::U_EFBIG;
		return 0;
	}

	/* 如果是小型文件，从基本索引表i_addr[0-5]中获得物理盘块号即可 */
	if (lbn < 6) {
		phyBlkno = this->i_addr[lbn];

		/* 如果该逻辑块号还没有相应的物理盘块号与之对应，则分配一个物理块。*/
		if (phyBlkno == 0 && (pFirstBuffer = fileSystem.Alloc()) != NULL) {
			phyBlkno = pFirstBuffer->blkno;
			bufferManager.Bdwrite(pFirstBuffer);
			this->i_addr[lbn] = phyBlkno;
			this->i_flag |= INode::IUPD;  //该iNode被修改过
		}
		return phyBlkno;
	}

	/* lbn >= 6 大型、巨型文件 */
	if (lbn < INode::LARGE_FILE_BLOCK) {
		index = (lbn - INode::SMALL_FILE_BLOCK) / INode::ADDRESS_PER_INDEX_BLOCK + 6;
	}
	else {
		/* 巨型文件: 长度介于263 - (128 * 128 * 2 + 128 * 2 + 6)个盘块之间 */
		index = (lbn - INode::LARGE_FILE_BLOCK) / (INode::ADDRESS_PER_INDEX_BLOCK * INode::ADDRESS_PER_INDEX_BLOCK) + 8;
	}

	phyBlkno = this->i_addr[index];  //找到索引所在块的地址然后读入到FirstBuffer中
	if (phyBlkno) {
		pFirstBuffer = bufferManager.Bread(phyBlkno);
	}
	else {
		/* 若该项为零，则表示不存在相应的间接索引表块 */
		this->i_flag |= INode::IUPD;
		if ((pFirstBuffer = fileSystem.Alloc()) == 0) {
			return 0;
		}
		this->i_addr[index] = pFirstBuffer->blkno;
	}

	iTable = (int *)pFirstBuffer->addr;  //用数组指针的形式获得之前读入的索引表
	if (index >= 8) {
		/*
		* 对于巨型文件的情况，pFirstBuf中是二次间接索引表，
		* 还需根据逻辑块号，经由二次间接索引表找到一次间接索引表
		*/
		index = ((lbn - INode::LARGE_FILE_BLOCK) / INode::ADDRESS_PER_INDEX_BLOCK) % INode::ADDRESS_PER_INDEX_BLOCK;
		phyBlkno = iTable[index];

		if (phyBlkno) {
			bufferManager.Brelse(pFirstBuffer);
			pSecondBuffer = bufferManager.Bread(phyBlkno);
		}
		else {
			if ((pSecondBuffer = fileSystem.Alloc()) == NULL) {
				bufferManager.Brelse(pFirstBuffer);
				return 0;
			}
			iTable[index] = pSecondBuffer->blkno;
			bufferManager.Bdwrite(pFirstBuffer);
		}

		pFirstBuffer = pSecondBuffer;
		iTable = (int *)pSecondBuffer->addr;
	}
	//获取想要找的物理块在索引当中的位置index
	if (lbn < INode::LARGE_FILE_BLOCK) {
		index = (lbn - INode::SMALL_FILE_BLOCK) % INode::ADDRESS_PER_INDEX_BLOCK;
	}
	else {
		index = (lbn - INode::LARGE_FILE_BLOCK) % INode::ADDRESS_PER_INDEX_BLOCK;
	}

	if ((phyBlkno = iTable[index]) == 0 && (pSecondBuffer = fileSystem.Alloc()) != NULL) {
		phyBlkno = pSecondBuffer->blkno;
		iTable[index] = phyBlkno;
		bufferManager.Bdwrite(pSecondBuffer);
		bufferManager.Bdwrite(pFirstBuffer);
	}
	else {
		bufferManager.Brelse(pFirstBuffer);
	}
	return phyBlkno;
}

/* 清空Inode对象中的数据 */
void INode::Clean() {
	/*
	* Inode::Clean()特定用于IAlloc()中清空新分配DiskInode的原有数据，
	* 即旧文件信息。Clean()函数中不应当清除i_dev, i_number, i_flag, i_count,
	* 这是属于内存Inode而非DiskInode包含的旧文件信息，而Inode类构造函数需要
	* 将其初始化为无效值。
	*/

	// this->i_flag = 0;
	this->i_mode = 0;
	//this->i_count = 0;
	this->i_nlink = 0;
	//this->i_dev = -1;
	//this->i_number = -1;
	this->i_uid = -1;
	this->i_gid = -1;
	this->i_size = 0;
	this->i_lastr = -1;
	Utility::memset(i_addr, 0, sizeof(i_addr));
}

/* 更新外存Inode的最后的访问时间、修改时间 */
void INode::IUpdate(int time) {
	Buffer* pBuffer;
	DiskINode dINode;
	FileSystem& fileSystem = g_FileSystem;
	BufferManager& bufferManager = g_BufferManager;

	/* 
	 *当IUPD和IACC标志之一被设置，才需要更新相应DiskInode
	 * 目录搜索，不会设置所途径的目录文件的IACC和IUPD标志 
	 */
	if (this->i_flag&(INode::IUPD | INode::IACC)) {
		pBuffer = bufferManager.Bread(FileSystem::INODE_ZONE_START_SECTOR + this->i_number / FileSystem::INODE_NUMBER_PER_SECTOR);
		dINode.d_mode = this->i_mode;
		dINode.d_nlink = this->i_nlink;
		dINode.d_uid = this->i_uid;
		dINode.d_gid = this->i_gid;
		dINode.d_size = this->i_size;
		memcpy(dINode.d_addr, i_addr, sizeof(dINode.d_addr));
		if (this->i_flag & INode::IACC) {
			dINode.d_atime = time;
		}
		if (this->i_flag & INode::IUPD) {
			dINode.d_mtime = time;
		}

		unsigned char* p = pBuffer->addr + (this->i_number % FileSystem::INODE_NUMBER_PER_SECTOR) * sizeof(DiskINode);
		DiskINode* pNode = &dINode;
		Utility::memcpy(p, pNode, sizeof(DiskINode));
		bufferManager.Bwrite(pBuffer);
	}
}

void INode::ITrunc() {
	BufferManager &bufferManager = g_BufferManager;
	FileSystem& fileSystem = g_FileSystem;
	Buffer* pFirstBuffer, *pSecondBuffer;

	for (int i = 9; i >= 0; --i) {
		if (this->i_addr[i]) {
			if (i >= 6) {
				pFirstBuffer = bufferManager.Bread(this->i_addr[i]);
				int *pFirst = (int*)pFirstBuffer->addr;
				for (int j = BLOCK_SIZE / sizeof(int) - 1; j >= 0; --j) {
					if (pFirst[j]) {
						if (i >= 8) {
							pSecondBuffer = bufferManager.Bread(pFirst[j]);
							int* pSecond = (int*)pSecondBuffer->addr;
							for (int k = BLOCK_SIZE / sizeof(int) - 1; k >= 0; --k) {
								if (pSecond[k]) {
									fileSystem.Free(pSecond[k]);
								}
							}
							bufferManager.Brelse(pSecondBuffer);
						}
						fileSystem.Free(pFirst[j]);
					}
				}
				bufferManager.Brelse(pFirstBuffer);
			}
			fileSystem.Free(this->i_addr[i]);
			this->i_addr[i] = 0;
		}
	}
	this->i_size = 0;
	this->i_flag |= INode::IUPD;
	this->i_mode &= ~(INode::ILARG);
	this->i_nlink = 1;
}

/* 更新外存Inode的用户信息 */
void INode::IUpdate_user() {
	Buffer* pBuffer;
	DiskINode dINode;
	FileSystem& fileSystem = g_FileSystem;
	BufferManager& bufferManager = g_BufferManager;

	pBuffer = bufferManager.Bread(FileSystem::INODE_ZONE_START_SECTOR + this->i_number / FileSystem::INODE_NUMBER_PER_SECTOR);
	dINode.d_mode = this->i_mode;
	dINode.d_nlink = this->i_nlink;
	dINode.d_uid = g_User.pw_uid;
	dINode.d_gid = g_User.pw_gid;
	dINode.d_size = this->i_size;
	memcpy(dINode.d_addr, i_addr, sizeof(dINode.d_addr));

	unsigned char* p = pBuffer->addr + (this->i_number % FileSystem::INODE_NUMBER_PER_SECTOR) * sizeof(DiskINode);
	DiskINode* pNode = &dINode;
	Utility::memcpy(p, pNode, sizeof(DiskINode));
	bufferManager.Bwrite(pBuffer);

}