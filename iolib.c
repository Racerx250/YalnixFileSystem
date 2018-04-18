#include <comp421/filesystem.h>
#include <comp421/iolib.h>
#include <comp421/yalnix.h>

struct my_msg {
	int fid;
	int num_data;
	char data[16]; 
	void *addr_ptr;
};

/* FID 0 */
int Open(char *pathname) {
	return 0;
}

/* FID 1 */
int Close(int fd) { 
	struct my_msg create_msg;//= (struct my_msg) malloc(sizeof(struct my_msg));
	create_msg.fid = 1;
	create_msg.num_data = fd;

	void *msg = &create_msg;

	return Send(msg, 1);
}

/* FID 2 */
int Create(char *pathname) {

	if (sizeof(pathname) > 16) return ERROR;

	struct my_msg create_msg;//= (struct my_msg) malloc(sizeof(struct my_msg));
	create_msg.fid = 2;
	create_msg.num_data = 0;
	strcpy(create_msg.data, pathname);

	void *msg = &create_msg;

	int result = Send(msg, 1);
	printf("RESULT OF CREATE IS %i\n", result);
	return result;
}

/* FID 3 WEVE GOT PROBLEMS HERE*/
int Read(int fd, void *buf, int size) {
	struct my_msg create_msg;
	create_msg.fid = 3;
	create_msg.num_data = fd;
	create_msg.addr_ptr = buf;

	void *msg = &create_msg;
	printf("please %i\n", create_msg.addr_ptr);
	int result = Send(msg, 1);
    // void *copy = malloc(8*sizeof(char));
    // CopyFrom(1, copy, create_msg.addr_ptr, sizeof(struct my_msg));
    // buf = copy;
    printf("ASDFASDF %s", (char *) create_msg.addr_ptr);
    return result;
}

/* FID 4 WEVE GOT PROBLEMS HERE*/
int Write(int fd, void *buf, int size) {
	struct my_msg create_msg;//= (struct my_msg) malloc(sizeof(struct my_msg));
	create_msg.fid = 4;
	create_msg.num_data = fd;
	create_msg.data[0] = size;
	create_msg.addr_ptr = buf;

	void *msg = &create_msg;

	int result1 = Send(msg, 1);

	// struct my_msg create_size_msg;//= (struct my_msg) malloc(sizeof(struct my_msg));
	// create_size_msg.fid = 4;
	// create_size_msg.num_data = size;
	// create_size_msg.addr_ptr = buf;

	// void *size_msg = &create_size_msg;

	// int result2 = Send(size_msg, 1);

	return result1;
}

/* FID 5 */
int Seek(int fd, int offset, int whence) {
	struct my_msg create_msg;//= (struct my_msg) malloc(sizeof(struct my_msg));
	create_msg.fid = 5;
	create_msg.num_data = fd;
	create_msg.data[0] = offset;
	create_msg.data[1] = whence;

	void *msg = &create_msg;

	int result1 = Send(msg, 1);

	return result1;
}

/* FID 6 */
int Link(char *oldname, char *newname) {
	struct my_msg create_msg;//= (struct my_msg) malloc(sizeof(struct my_msg));
	create_msg.fid = 6;

	if (sizeof(oldname) > DIRNAMELEN || sizeof(newname) > DIRNAMELEN) return -1;
	
	create_msg.data[0] = sizeof(oldname);
	create_msg.data[1] = sizeof(newname);
	
	char *char_addr = (char *) malloc(sizeof(oldname) + sizeof(newname));
	int i = 0;
	for (i = 0; i < sizeof(oldname); i++) {
		char_addr[i] = oldname[i];
	}

	for (i = 0; i < sizeof(newname); i++) {
		char_addr[i + sizeof(oldname)] = newname[i];
	}

	void *temp_addr = malloc(sizeof(oldname) + sizeof(newname));
	memcpy(temp_addr, char_addr, sizeof(oldname) + sizeof(newname));

	create_msg.addr_ptr = temp_addr;

	void *msg = &create_msg;

	int result1 = Send(msg, 1);

	return result1;
}

/* FID 7 */
int Unlink(char *pathname) {
	return 0;
}

/* FID 8 */
int SymLink(char *oldname, char *newname) {
	return 0;
}

/* FID 9 */
int ReadLink(char *pathname, char *buf, int len) {
	return 0;
}

/* FID 10 */
int MkDir(char *pathname) {
	return 0;
}

/* FID 11 */
int RmDir(char *pathname) {
	return 0;
}

/* FID 12 */
int ChDir(char *pathname) {
	return 0;
}

/* FID 13 */
int Stat(char *pathname, struct Stat *statbuf) {
	return 0;
}

/* FID 14 */
int Sync(void) {
	return 0;
}

/* FID 15 */
int Shutdown(void) {
	return 0;
}

