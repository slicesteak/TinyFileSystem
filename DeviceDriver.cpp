#include "DeviceDriver.h"
#include"FileSystem.h"

const char* DeviceDriver::DISK_FILE_NAME = "1453381-fs.img";


extern User g_User;

DeviceDriver::DeviceDriver() {
	fp = fopen(DISK_FILE_NAME, "rb+");
}

DeviceDriver::~DeviceDriver() {
	if (fp) {
		fclose(fp);
	}
}

/* 检查镜像文件是否存在 */
bool DeviceDriver::Exists() {
	return fp != NULL;
}

/* 打开镜像文件 */
void DeviceDriver::Construct() {
    fp = fopen(DISK_FILE_NAME, "wb+");  //以读/写方式打开一个二进制文件，只允许读/写数据。
	if (fp == NULL) {
		printf("打开或新建文件%s失败！", DISK_FILE_NAME);
		exit(-1);
	}
}

/* 实际写磁盘函数 */
void DeviceDriver::write(const void* buffer, unsigned int size, int offset, unsigned int origin) {
	if (offset >= 0) {
		fseek(fp, offset, origin);
	}
	fwrite(buffer, size, 1, fp);
}

/* 实际写磁盘函数 */
void DeviceDriver::read(void* buffer, unsigned int size, int offset, unsigned int origin) {
	if (offset >= 0) {
		fseek(fp, offset, origin);
	}
	fread(buffer, size, 1, fp);
}

/*读取用户信息*/
User* DeviceDriver::readPW() {
	fseek(fp, 16377 * FileSystem::BLOCK_SIZE, 0);
	User* temp = &g_User;
	char* str = new char;
	fread(str, sizeof(User*), 1, fp);
	return temp;
}