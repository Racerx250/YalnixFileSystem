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
	return 0;
}

/* FID 2 */
int Create(char *pathname) {
	TracePrintf(0, "cool?\n");
	struct my_msg create_msg;//= (struct my_msg) malloc(sizeof(struct my_msg));
	create_msg.fid = 2;
	create_msg.num_data = 0;
	// new_msg.data = "this is a test";

	void *msg = &create_msg;

	int send_result = Send(msg, 1);

	return 0;
}

/* FID 3 */
int Read(int fd, void *buf, int size) {
	return 0;
}

/* FID 4 */
int Write(int fd, void *buf, int size) {
	return 0;
}

/* FID 5 */
int Seek(int fd, int offset, int whence) {
	return 0;
}

/* FID 6 */
int Link(char *oldname, char *newname) {
	return 0;
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

