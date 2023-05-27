#include "Utility.h"
#include "FileManager.h"
#include "BufferManager.h"
#include "User.h"

extern BufferManager g_BufferManager;
extern FileSystem g_FileSystem;
extern INodeTable g_INodeTable;
extern OpenFileTable g_OpenFileTable;
extern User g_User;
extern int ffor_flag;

FileManager::FileManager() {
	fileSystem = &g_FileSystem;
	openFileTable = &g_OpenFileTable;
	inodeTable = &g_INodeTable;
	rootDirINode = inodeTable->IGet(FileSystem::ROOT_INODE_NO);
	user1DirINode = inodeTable->IGet(FileSystem::USER1_INODE_NO);
	user2DirINode = inodeTable->IGet(FileSystem::USER2_INODE_NO);
	homeDirINode = inodeTable->IGet(FileSystem::HOME_INODE_NO);

	rootDirINode->i_count += 0xff;
	//rootDirINode = NULL;
}

FileManager::~FileManager() {

}

/*
* ���ܣ����ļ�
* Ч�����������ļ��ṹ���ڴ�i�ڵ㿪�� ��i_count Ϊ������i_count ++��
* */
void FileManager::Open() {
	User& u = g_User;
	INode* pINode;
	long temp1 = 1, temp2 = 2, temp3;
	temp1 = temp1 & u.arg[1];  //��ȡ���λ��������temp1��
	temp2 = temp2 & u.arg[1];  //��ȡ�ε�λ��������temp2��
	temp3 = (temp1 << 1) | (temp2 >> 1);  //���ε�λ�����λ�ֱ��ƶ�һλ, Ȼ���ٺϲ�, ʵ�ִε�λ�����λ����

	pINode = this->NameI(FileManager::OPEN);
	if (NULL == pINode) {
		return;
	}

	else if (!u.CheckPerm(*pINode, temp3 << 7, ffor_flag)) {
		return;
	}
	if ((pINode->i_mode & INode::IFMT) == INode::IFDIR) {
		u.u_error = User::U_NOTFILE;
		return;
	}
	this->Open1(pINode, u.arg[1], 0);
}

void FileManager::Creat() {
	INode* pINode;
	User& u = g_User;
	//unsigned int newACCMode = u.arg[1] & (INode::IRWXU | INode::IRWXG | INode::IRWXO);
	unsigned int newACCMode = u.arg[1];

	/* ����Ŀ¼��ģʽΪ1����ʾ����������Ŀ¼����д�������� */
	pINode = this->NameI(FileManager::CREATE);

	/* û���ҵ���Ӧ��INode����NameI���� */
	if (NULL == pINode) {
		if (u.u_error)
			return;
		pINode = this->MakNode(newACCMode);
		if (NULL == pINode)
			return;

		/* ������������ֲ����ڣ�ʹ�ò���trf = 2������open1()��*/
		this->Open1(pINode, File::FWRITE, 2);
		return;
	}

	/* ���NameI()�������Ѿ�����Ҫ�������ļ�������ո��ļ������㷨ITrunc()��*/
	this->Open1(pINode, File::FWRITE, 1);
	pINode->i_mode |= newACCMode;
}

/* ����NULL��ʾĿ¼����ʧ�ܣ�δ�ҵ�u.dirp��ָ��Ŀ¼ȫ·��
 * �����Ǹ�ָ�룬ָ���ļ����ڴ��i�ڵ�
 */
INode* FileManager::NameI(enum DirectorySearchMode mode) {
	BufferManager& bufferManager = g_BufferManager;
	User& u = g_User;
	INode* pINode = u.cdir;
	Buffer* pBuffer;
	int freeEntryOffset;
	unsigned int index = 0, nindex = 0;//index: ·������  nindex:·����������

	if ('/' == u.dirp[0]) {
		nindex = ++index + 1;
		pINode = this->rootDirINode;
	}

	/* ���ѭ��ÿ�δ���pathname��һ��·������ */
	while (1) {
		if (u.u_error != User::U_NOERROR) {
			break;
		}
		if (nindex >= u.dirp.length()) {
			return pINode;
		}

		/* ���Ҫ���������Ĳ���Ŀ¼�ļ����ͷ����INode��Դ���˳� */
		if ((pINode->i_mode & INode::IFMT) != INode::IFDIR) {
			u.u_error = User::U_ENOTDIR;
			break;
		}

		nindex = u.dirp.find_first_of('/', index);
		Utility::memset(u.dbuf, 0, sizeof(u.dbuf));
		Utility::memcpy(u.dbuf, u.dirp.data() + index, (nindex == (unsigned int)string::npos ? u.dirp.length() : nindex) - index);
		index = nindex + 1;

		/* �ڲ�ѭ�����ֶ���u.dbuf[]�е�·���������������Ѱƥ���Ŀ¼�� */
		u.IOParam.offset = 0;
		/* ����ΪĿ¼����� �����հ׵�Ŀ¼��*/
		u.IOParam.count = pINode->i_size / sizeof(DirectoryEntry);
		freeEntryOffset = 0;
		pBuffer = NULL;

		/* ��һ��Ŀ¼��Ѱ�� */
		while (1) {

			/* ��Ŀ¼���Ѿ�������� */
			if (0 == u.IOParam.count) {
				if (NULL != pBuffer) {
					bufferManager.Brelse(pBuffer);
				}

				/* ����Ǵ������ļ� */
				if (FileManager::CREATE == mode && nindex >= u.dirp.length()) {
					u.pdir = pINode;
					if (freeEntryOffset) {
						u.IOParam.offset = freeEntryOffset - sizeof(DirectoryEntry);
					}
					else {
						pINode->i_flag |= INode::IUPD;
					}
					return NULL;
				}
				u.u_error = User::U_ENOENT;
				goto out;
			}

			/* �Ѷ���Ŀ¼�ļ��ĵ�ǰ�̿飬��Ҫ������һĿ¼�������̿� */
			if (0 == u.IOParam.offset % INode::BLOCK_SIZE) {
				if (pBuffer) {
					bufferManager.Brelse(pBuffer);
				}
				int phyBlkno = pINode->Bmap(u.IOParam.offset / INode::BLOCK_SIZE);
				pBuffer = bufferManager.Bread(phyBlkno);
				//pBuffer->debug();
			}

			Utility::memcpy(&u.dent, pBuffer->addr + (u.IOParam.offset % INode::BLOCK_SIZE), sizeof(u.dent));
			u.IOParam.offset += sizeof(DirectoryEntry);
			u.IOParam.count--;

			/* ����ǿ���Ŀ¼���¼����λ��Ŀ¼�ļ���ƫ���� */
			if (0 == u.dent.m_ino) {
				if (0 == freeEntryOffset) {
					freeEntryOffset = u.IOParam.offset;
				}
				continue;
			}

			if (!Utility::memcmp(u.dbuf, &u.dent.name, sizeof(DirectoryEntry) - 4)) {
				break;
			}
		}

		if (pBuffer) {
			bufferManager.Brelse(pBuffer);
		}

		/* �����ɾ���������򷵻ظ�Ŀ¼INode����Ҫɾ���ļ���INode����u.dent.m_ino�� */
		if (FileManager::DELETE == mode && nindex >= u.dirp.length()) {
			return pINode;
		}

		/*
		* ƥ��Ŀ¼��ɹ������ͷŵ�ǰĿ¼INode������ƥ��ɹ���
		* Ŀ¼��m_ino�ֶλ�ȡ��Ӧ��һ��Ŀ¼���ļ���INode��
		*/
		this->inodeTable->IPut(pINode);
		pINode = this->inodeTable->IGet(u.dent.m_ino);

		if (NULL == pINode) {
			return NULL;
		}
	}

out:
	this->inodeTable->IPut(pINode);
	return NULL;
}

/*
* trf == 0��open����
* trf == 1��creat���ã�creat�ļ���ʱ��������ͬ�ļ������ļ�
* trf == 2��creat���ã�creat�ļ���ʱ��δ������ͬ�ļ������ļ��������ļ�����ʱ��һ������
* mode���������ļ�ģʽ����ʾ�ļ������� ����д���Ƕ�д
*/
void FileManager::Open1(INode* pINode, int mode, int trf) {
	User& u = g_User;

	/* ��creat�ļ���ʱ��������ͬ�ļ������ļ����ͷŸ��ļ���ռ�ݵ������̿� */
	if (1 == trf) {
		pINode->ITrunc();
	}

	/* ������ļ����ƿ�File�ṹ */
	File* pFile = this->openFileTable->FAlloc();
	if (NULL == pFile) {
		this->inodeTable->IPut(pINode);
		return;
	}

	/* ���ô��ļ���ʽ������File�ṹ���ڴ�INode�Ĺ�����ϵ */
	pFile->flag = mode & (File::FREAD | File::FWRITE);
	pFile->inode = pINode;

	/* Ϊ�򿪻��ߴ����ļ��ĸ�����Դ���ѳɹ����䣬�������� */
	if (u.u_error == 0) {
		return;
	}
	else {  /* ����������ͷ���Դ */
		/* �ͷŴ��ļ������� */
		int fd = u.ar0[User::EAX];
		if (fd != -1) {
			u.ofiles.SetF(fd, NULL);
			/* �ݼ�File�ṹ��INode�����ü��� ,File�ṹû���� f_countΪ0�����ͷ�File�ṹ��*/
			pFile->count--;
		}
		this->inodeTable->IPut(pINode);
	}
}

/* ��creat���á�
 * Ϊ�´������ļ�д�µ�i�ڵ�͸�Ŀ¼���µ�Ŀ¼��(��Ӧ������User�ṹ��)
 * ���ص�pINode�����������ڴ�i�ڵ㣬���е�i_count�� 1��
 */
INode* FileManager::MakNode(unsigned int mode) {
	INode* pINode;
	User& u = g_User;

	/* ����һ������DiskInode������������ȫ����� */
	pINode = this->fileSystem->IAlloc();
	if (NULL == pINode) {
		return NULL;
	}

	pINode->i_flag |= (INode::IACC | INode::IUPD);
	pINode->i_mode = mode | INode::IALLOC;
	pINode->i_nlink = 1;
	pINode->i_uid = g_User.pw_uid;
	pINode->i_gid = g_User.pw_gid;
	/* ��Ŀ¼��д��u.u_u_dent�����д��Ŀ¼�ļ� */
	this->WriteDir(pINode);
	return pINode;
}

/* ��creat���ӵ��á�
 * �������Լ���Ŀ¼��д����Ŀ¼���޸ĸ�Ŀ¼�ļ���i�ڵ� ������д�ش��̡�
 */
void FileManager::WriteDir(INode* pINode) {
	User& u = g_User;
	//cout << "i_number=" << pINode->i_number << endl;
	/* ����Ŀ¼����INode��Ų��� */
	u.dent.m_ino = pINode->i_number;

	/* ����Ŀ¼����pathname�������� */
	Utility::memcpy(u.dent.name, u.dbuf, DirectoryEntry::DIRSIZ);

	u.IOParam.count = DirectoryEntry::DIRSIZ + 4;
	u.IOParam.base = (unsigned char*)&u.dent;

	/* ��Ŀ¼��д�븸Ŀ¼�ļ� */
	u.pdir->WriteI();
	this->inodeTable->IPut(u.pdir);
}

void FileManager::Close() {
	User& u = g_User;
	int fd = u.arg[0];

	/* ��ȡ���ļ����ƿ�File�ṹ */
	File* pFile = u.ofiles.GetF(fd);
	if (NULL == pFile) {
		return;
	}

	/* �ͷŴ��ļ�������fd���ݼ�File�ṹ���ü��� */
	u.ofiles.SetF(fd, NULL);
	this->openFileTable->CloseF(pFile);
}

void FileManager::UnLink() {
	//ע��ɾ���ļ����д���й¶
	INode* pINode;
	INode* pDeleteINode;
	User& u = g_User;

	pDeleteINode = this->NameI(FileManager::DELETE);
	if (NULL == pDeleteINode) {
		return;
	}

	pINode = this->inodeTable->IGet(u.dent.m_ino);
	if (NULL == pINode) {
		return;
	}

	/* д��������Ŀ¼�� */
	u.IOParam.offset -= (DirectoryEntry::DIRSIZ + 4);
	u.IOParam.base = (unsigned char*)&u.dent;
	u.IOParam.count = DirectoryEntry::DIRSIZ + 4;

	u.dent.m_ino = 0;
	pDeleteINode->WriteI();

	/* �޸�inode�� */
	pINode->i_nlink--;
	pINode->i_flag |= INode::IUPD;

	this->inodeTable->IPut(pDeleteINode);
	this->inodeTable->IPut(pINode);
}

void FileManager::Seek() {
	File* pFile;
	User& u = g_User;
	int fd = u.arg[0];

	pFile = u.ofiles.GetF(fd);
	if (NULL == pFile) {
		return;  /* ��FILE�����ڣ�GetF��������� */
	}

	int offset = u.arg[1];

	switch (u.arg[2]) {
		/* ��дλ������Ϊoffset */
	case 0:
		pFile->offset = offset;
		break;
		/* ��дλ�ü�offset(�����ɸ�) */
	case 1:
		pFile->offset += offset;
		break;
		/* ��дλ�õ���Ϊ�ļ����ȼ�offset */
	case 2:
		pFile->offset = pFile->inode->i_size + offset;
		break;
	default:
		cout << " origin " << u.arg[2] << " is undefined ! \n";
		break;
	}
}

void FileManager::Read() {
	/* ֱ�ӵ���Rdwr()�������� */
	this->Rdwr(File::FREAD);
}

void FileManager::WritefromFile() {
	/* ֱ�ӵ���Rdwr()�������� */
	this->Rdwr(File::FWRITE);
}

void FileManager::Rdwr(enum File::FileFlags mode) {
	File* pFile;
	User& u = g_User;

	/* ����Read()/Write()��ϵͳ���ò���fd��ȡ���ļ����ƿ�ṹ */
	pFile = u.ofiles.GetF(u.arg[0]);	/* fd */
	if (NULL == pFile) {
		/* �����ڸô��ļ���GetF�Ѿ����ù������룬�������ﲻ��Ҫ�������� */
		/*	u.u_error = User::EBADF;	*/
		return;
	}

	/* ��д��ģʽ����ȷ */
	if ((pFile->flag & mode) == 0) {
		u.u_error = User::U_EACCES;
		return;
	}

	u.IOParam.base = (unsigned char*)u.arg[1];     /* Ŀ�껺������ַ */
	u.IOParam.count = u.arg[2];		/* Ҫ���/д���ֽ��� */

	u.IOParam.offset = pFile->offset;   /* �����ļ���ʼ��λ�� */
	if (File::FREAD == mode) {
		pFile->inode->ReadI();
	}
	else {
		pFile->inode->WriteI();
	}

	/* ���ݶ�д�������ƶ��ļ���дƫ��ָ�� */
	pFile->offset += (u.arg[2] - u.IOParam.count);

	/* ����ʵ�ʶ�д���ֽ������޸Ĵ��ϵͳ���÷���ֵ�ĺ���ջ��Ԫ */
	u.ar0[User::EAX] = u.arg[2] - u.IOParam.count;
}

vector<string> FileManager::Ls() {
	vector<string> show;
	User& u = g_User;
	BufferManager& bufferManager = g_BufferManager;

	INode* pINode = u.cdir;
	Buffer* pBuffer = NULL;
	u.IOParam.offset = 0;
	u.IOParam.count = pINode->i_size / sizeof(DirectoryEntry);  //Ŀ¼�ļ���С ���� Ŀ¼��С = Ŀ¼�����

	while (u.IOParam.count) {
		if (0 == u.IOParam.offset % INode::BLOCK_SIZE) {
			if (pBuffer) {
				bufferManager.Brelse(pBuffer);
			}
			int phyBlkno = pINode->Bmap(u.IOParam.offset / INode::BLOCK_SIZE);//�ҵ���ǰ��Ҫ�����߼����Ӧ��������
			pBuffer = bufferManager.Bread(phyBlkno);
		}
		Utility::memcpy(&u.dent, pBuffer->addr + (u.IOParam.offset % INode::BLOCK_SIZE), sizeof(u.dent));
		u.IOParam.offset += sizeof(DirectoryEntry);
		u.IOParam.count--;

		if (0 == u.dent.m_ino)
			continue;

		INode* fileInode = this->inodeTable->IGet(u.dent.m_ino);
		string property = "----------";
		string size = "";
		int a[3];
		a[0] = fileInode->i_mode & INode::IRWXU;
		a[1] = fileInode->i_mode & INode::IRWXG;
		a[2] = fileInode->i_mode & INode::IRWXO;

		if ((fileInode->i_mode & INode::IFMT) == INode::IFDIR)
			property = "\tdir";
		else
			property = "\tfile";
		//u.ls += u.dent.name + property;
		//u.ls += "\n";
		u.ls = u.dent.name + property;
		show.push_back(u.dent.name);
		show.push_back(property);
	}

	if (pBuffer) {
		bufferManager.Brelse(pBuffer);
	}
	return show;
}

/* �ı䵱ǰ����Ŀ¼ */
void FileManager::ChDir() {
	INode* pINode;
	User& u = g_User;

	pINode = this->NameI(FileManager::OPEN);
	if (NULL == pINode) {
		return;
	}

	/* ���������ļ�����Ŀ¼�ļ� */
	if ((pINode->i_mode & INode::IFMT) != INode::IFDIR) {
		u.u_error = User::U_ENOTDIR;
		this->inodeTable->IPut(pINode);
		return;
	}

	u.cdir = pINode;
	/* ·�����ǴӸ�Ŀ¼'/'��ʼ����������u.u_curdir������ϵ�ǰ·������ */
	if (u.dirp[0] != '/') {
		u.curDirPath += u.dirp;
	}
	else {
		/* ����ǴӸ�Ŀ¼'/'��ʼ����ȡ��ԭ�й���Ŀ¼ */
		u.curDirPath = u.dirp;
	}
	if (u.curDirPath.back() != '/')
		u.curDirPath.push_back('/');
}

void FileManager::Mov(string srcFileName) {
	INode* oldINode = g_User.cdir;	//�洢ԭ����Ŀ¼�ڵ���Ϣ
	INode* oldpINode = g_User.pdir;
	INode* pINode;	/*�ƶ�Ŀ���ļ���*/
	BufferManager& bufferManager = g_BufferManager;
	pINode = this->NameI(FileManager::OPEN);
	if (NULL == pINode) {
		g_User.u_error = User::U_ENOTDIR;
		return;
	}
	int m_ino = 0;	//Ҫ�ƶ����ļ���INode�ڵ�
	//�ƶ���Ŀ�겻���ļ��У��׳�������Ϣ
	if ((pINode->i_mode & INode::IFDIR) != INode::IFDIR) {
		g_User.u_error = User::U_ENOTDIR;
		return;
	}
	INode* pDeleteINode;	/*��ǰĿ¼�ļ���*/

	string test_dirp = srcFileName;
	string file_dirp;
	string dir_dirp;
	int pos = test_dirp.find_last_of('/');
	if (pos <= test_dirp.length() && pos >= 0) {
		file_dirp = test_dirp.substr(pos + 1, test_dirp.length());
		dir_dirp = test_dirp.substr(0, pos + 1);
	}
	else {
		file_dirp = test_dirp;
		dir_dirp = g_User.curDirPath;
	}
	if (dir_dirp.back() == '/' && dir_dirp.length() != 1) {
		dir_dirp.pop_back();
	}


	string dirp = g_User.dirp;
	g_User.dirp = dir_dirp;
	//g_User.dirp = g_User.curDirPath;
	pDeleteINode = this->NameI(FileManager::OPEN);
	//�ҵ�Դ�ļ��е�Inode�ڵ�
	if (NULL == pDeleteINode) {
		g_User.u_error = User::U_ENOTDIR;
		return;
	}

	Buffer* pBuffer = NULL;
	User& u = g_User;
	int srcOffset = 0;	//ԴĿ¼Ŀ¼���е�ƫ�ƣ�ͷ������ɾ��ʱֱ����0
	u.IOParam.offset = 0;
	u.IOParam.count = pDeleteINode->i_size / sizeof(DirectoryEntry);  //Ŀ¼�ļ���С ���� Ŀ¼��С = Ŀ¼�����
	//Ѱ��Ҫ�ƶ����ļ����ļ����Լ�INode���
	while (u.IOParam.count) {
		if (0 == u.IOParam.offset % INode::BLOCK_SIZE) {
			if (pBuffer) {
				bufferManager.Brelse(pBuffer);
			}
			int phyBlkno = pDeleteINode->Bmap(u.IOParam.offset / INode::BLOCK_SIZE);//�ҵ���ǰ��Ҫ�����߼����Ӧ��������
			pBuffer = bufferManager.Bread(phyBlkno);
		}
		/*void* dest const ,void* src,size_t n */  //�ѵ�ǰ��һ��Ŀ¼��ӻ��������Ƶ��û���(u.dent)
		Utility::memcpy(&u.dent, pBuffer->addr + (u.IOParam.offset % INode::BLOCK_SIZE), sizeof(u.dent));
		u.IOParam.offset += sizeof(DirectoryEntry);
		u.IOParam.count--;

		if (0 == u.dent.m_ino)
			continue;

		if (u.dent.name == file_dirp) {//	�ҵ�Ҫ�ƶ����ļ�
			m_ino = u.dent.m_ino;
			srcOffset = u.IOParam.offset - sizeof(DirectoryEntry);
			break;
		}
	}
	if (pBuffer) {
		bufferManager.Brelse(pBuffer);
	}
	//û���ҵ�����ļ����׳�������Ϣ
	if (m_ino == 0) {
		g_User.u_error = User::U_ENOTDIR;
		return;
	}
	u.pdir = pINode;
	//�ҵ�Ŀ���ļ���Ľ�β

	u.IOParam.offset = 0;
	u.IOParam.count = pINode->i_size / sizeof(DirectoryEntry);  //Ŀ¼�ļ���С ���� Ŀ¼��С = Ŀ¼�����
	pBuffer = NULL;
	DirectoryEntry targ_File;	//���Ŀ���ļ��е�Ŀ¼��Ա����ļ����Ƿ��ظ�

	int freeEntryOffset = 0;	//��¼Ŀ���ļ��еĿ���Ŀ¼��ƫ����

	while (u.IOParam.count) {
		if (0 == u.IOParam.offset % INode::BLOCK_SIZE) {
			if (pBuffer) {
				bufferManager.Brelse(pBuffer);
			}
			int phyBlkno = pINode->Bmap(u.IOParam.offset / INode::BLOCK_SIZE);//�ҵ���ǰ��Ҫ�����߼����Ӧ��������
			pBuffer = bufferManager.Bread(phyBlkno);
		}
		/*void* dest const ,void* src,size_t n */  //�ѵ�ǰ��һ��Ŀ¼��ӻ��������Ƶ���ʱĿ¼��(targ_File)
		Utility::memcpy(&targ_File, pBuffer->addr + (u.IOParam.offset % INode::BLOCK_SIZE), sizeof(targ_File));
		u.IOParam.offset += sizeof(DirectoryEntry);
		u.IOParam.count--;

		/* ����ǿ���Ŀ¼���¼����λ��Ŀ¼�ļ���ƫ���� */
		if (0 == targ_File.m_ino) {
			if (0 == freeEntryOffset) {
				freeEntryOffset = u.IOParam.offset;
			}
			continue;
		}

		if (targ_File.name == file_dirp) {//	Ŀ���ļ��������ļ����ظ�
			m_ino = u.dent.m_ino;
			g_User.u_error = User::U_EEXIST;
			return;
		}
	}

	if (freeEntryOffset) {
		u.IOParam.offset = freeEntryOffset - sizeof(DirectoryEntry);
	}

	u.IOParam.count = DirectoryEntry::DIRSIZ + 4;
	u.IOParam.base = (unsigned char*)&u.dent;

	/* ��Ŀ¼��д�븸Ŀ¼�ļ� */
	u.pdir->WriteI();
	this->inodeTable->IPut(u.pdir);
	//���ˣ����Ŀ¼����ϣ�������ɾ��ԴĿ¼��Ŀ¼��


	INode* filedeleteINode = this->inodeTable->IGet(u.dent.m_ino);//Ҫɾ�����ļ���INode
	if (NULL == filedeleteINode) {
		return;
	}

	g_User.dirp = file_dirp;
	g_User.cdir = pDeleteINode;
	pDeleteINode = this->NameI(FileManager::DELETE);

	/* д��������Ŀ¼�� */
	u.dent.m_ino = 0;
	u.IOParam.offset -= (DirectoryEntry::DIRSIZ + 4);
	u.IOParam.base = (unsigned char*)&u.dent;
	u.IOParam.count = DirectoryEntry::DIRSIZ + 4;

	pDeleteINode->WriteI();
	//pINode->i_nlink--;
	filedeleteINode->i_flag |= INode::IUPD;

	this->inodeTable->IPut(pDeleteINode);
	this->inodeTable->IPut(pINode);

	g_User.cdir = oldINode;//�洢ԭ����Ŀ¼�ڵ���Ϣ
	g_User.pdir = oldpINode;

}

/*�ļ�����*/
void FileManager::Link(string srcFileName) {
	INode* oldINode = g_User.cdir;	//�洢ԭ����Ŀ¼�ڵ���Ϣ
	INode* oldpINode = g_User.pdir;
	INode* pINode;	/*�ƶ�Ŀ���ļ���*/
	BufferManager& bufferManager = g_BufferManager;
	pINode = this->NameI(FileManager::OPEN);
	if (NULL == pINode) {
		g_User.u_error = User::U_ENOTDIR;
		return;
	}
	int m_ino = 0;	//Ҫ�ƶ����ļ���INode�ڵ�
	//�ƶ���Ŀ�겻���ļ��У��׳�������Ϣ
	if ((pINode->i_mode & INode::IFDIR) != INode::IFDIR) {
		g_User.u_error = User::U_ENOTDIR;
		return;
	}
	INode* pDeleteINode;	/*��ǰĿ¼�ļ���*/

	string test_dirp = srcFileName;
	string file_dirp;
	string dir_dirp;
	int pos = test_dirp.find_last_of('/');
	if (pos <= test_dirp.length() && pos >= 0) {
		file_dirp = test_dirp.substr(pos + 1, test_dirp.length());
		dir_dirp = test_dirp.substr(0, pos + 1);
	}
	else {
		file_dirp = test_dirp;
		dir_dirp = g_User.curDirPath;
	}
	if (dir_dirp.back() == '/' && dir_dirp.length() != 1) {
		dir_dirp.pop_back();
	}


	string dirp = g_User.dirp;
	g_User.dirp = dir_dirp;
	//g_User.dirp = g_User.curDirPath;
	pDeleteINode = this->NameI(FileManager::OPEN);
	//�ҵ�Դ�ļ��е�Inode�ڵ�
	if (NULL == pDeleteINode) {
		g_User.u_error = User::U_ENOTDIR;
		return;
	}

	Buffer* pBuffer = NULL;
	User& u = g_User;
	int srcOffset = 0;	//ԴĿ¼Ŀ¼���е�ƫ�ƣ�ͷ������ɾ��ʱֱ����0
	u.IOParam.offset = 0;
	u.IOParam.count = pDeleteINode->i_size / sizeof(DirectoryEntry);  //Ŀ¼�ļ���С ���� Ŀ¼��С = Ŀ¼�����
	//Ѱ��Ҫ�ƶ����ļ����ļ����Լ�INode���
	while (u.IOParam.count) {
		if (0 == u.IOParam.offset % INode::BLOCK_SIZE) {
			if (pBuffer) {
				bufferManager.Brelse(pBuffer);
			}
			int phyBlkno = pDeleteINode->Bmap(u.IOParam.offset / INode::BLOCK_SIZE);//�ҵ���ǰ��Ҫ�����߼����Ӧ��������
			pBuffer = bufferManager.Bread(phyBlkno);
		}
		/*void* dest const ,void* src,size_t n */  //�ѵ�ǰ��һ��Ŀ¼��ӻ��������Ƶ��û���(u.dent)
		Utility::memcpy(&u.dent, pBuffer->addr + (u.IOParam.offset % INode::BLOCK_SIZE), sizeof(u.dent));
		u.IOParam.offset += sizeof(DirectoryEntry);
		u.IOParam.count--;

		if (0 == u.dent.m_ino)
			continue;

		if (u.dent.name == file_dirp) {//	�ҵ�Ҫ�ƶ����ļ�
			m_ino = u.dent.m_ino;
			srcOffset = u.IOParam.offset - sizeof(DirectoryEntry);
			break;
		}
	}
	if (pBuffer) {
		bufferManager.Brelse(pBuffer);
	}
	//û���ҵ�����ļ����׳�������Ϣ
	if (m_ino == 0) {
		g_User.u_error = User::U_ENOTDIR;
		return;
	}
	u.pdir = pINode;
	//�ҵ�Ŀ���ļ���Ľ�β

	u.IOParam.offset = 0;
	u.IOParam.count = pINode->i_size / sizeof(DirectoryEntry);  //Ŀ¼�ļ���С ���� Ŀ¼��С = Ŀ¼�����
	pBuffer = NULL;
	DirectoryEntry targ_File;	//���Ŀ���ļ��е�Ŀ¼��Ա����ļ����Ƿ��ظ�

	int freeEntryOffset = 0;	//��¼Ŀ���ļ��еĿ���Ŀ¼��ƫ����

	while (u.IOParam.count) {
		if (0 == u.IOParam.offset % INode::BLOCK_SIZE) {
			if (pBuffer) {
				bufferManager.Brelse(pBuffer);
			}
			int phyBlkno = pINode->Bmap(u.IOParam.offset / INode::BLOCK_SIZE);//�ҵ���ǰ��Ҫ�����߼����Ӧ��������
			pBuffer = bufferManager.Bread(phyBlkno);
		}
		/*void* dest const ,void* src,size_t n */  //�ѵ�ǰ��һ��Ŀ¼��ӻ��������Ƶ���ʱĿ¼��(targ_File)
		Utility::memcpy(&targ_File, pBuffer->addr + (u.IOParam.offset % INode::BLOCK_SIZE), sizeof(targ_File));
		u.IOParam.offset += sizeof(DirectoryEntry);
		u.IOParam.count--;

		/* ����ǿ���Ŀ¼���¼����λ��Ŀ¼�ļ���ƫ���� */
		if (0 == targ_File.m_ino) {
			if (0 == freeEntryOffset) {
				freeEntryOffset = u.IOParam.offset;
			}
			continue;
		}

		if (targ_File.name == file_dirp) {//	Ŀ���ļ��������ļ����ظ�
			m_ino = u.dent.m_ino;
			g_User.u_error = User::U_EEXIST;
			return;
		}
	}

	if (freeEntryOffset) {
		u.IOParam.offset = freeEntryOffset - sizeof(DirectoryEntry);
	}

	u.IOParam.count = DirectoryEntry::DIRSIZ + 4;
	u.IOParam.base = (unsigned char*)&u.dent;

	/* ��Ŀ¼��д�븸Ŀ¼�ļ� */
	u.pdir->WriteI();
	this->inodeTable->IPut(u.pdir);
	//���ˣ����Ŀ¼����ϣ�������ɾ��ԴĿ¼��Ŀ¼��

	g_User.cdir = oldINode;//�洢ԭ����Ŀ¼�ڵ���Ϣ
	g_User.pdir = oldpINode;
}
