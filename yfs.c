#include <comp421/yalnix.h>
#include <comp421/iolib.h>
#include <comp421/filesystem.h>
#include <stdio.h>

#define NODES_PER_BLOCK BLOCKSIZE / INODESIZE

struct inode *node_list;

int total_inodes;
int total_blocks;

typedef struct free_block {
    int num;
    struct free_block *next;
} FBlock;

typedef struct free_node {
    int num;
    struct free_node *next;
} FNode;

struct my_msg {
	int fid;
	int num_data;
	char data[16]; 
	void *addr_ptr;
};

FBlock *fbh;
FNode *fnh;

int
main(int argc, char **argv)
{
      
    int result = Register(1);
    int pid = Fork();
    if (pid == 0) Exec(argv[1], argv + 1);

    TracePrintf(0, "[Server Process] argument count is %s\n", argv[1]);

    struct fs_header *header = (struct fs_header *) malloc(sizeof(struct fs_header));
    size_t buf[SECTORSIZE];
    int i, j;

    fbh = (FBlock *) malloc(sizeof(FBlock));
    fnh = (FNode *) malloc(sizeof(FNode));

    TracePrintf(0, "[Server Process] Starting...\n");

    ReadSector(1, buf);
    memcpy(header, buf, sizeof(struct fs_header));
    total_inodes = header->num_inodes;
    total_blocks = header->num_blocks;
    //free(buf);
    printf("num_inodes %i, num_blocks %i\n", total_inodes, total_blocks);
	
    node_list = malloc(sizeof(struct inode) * (total_inodes + 1));
	
    for (i = 2; i < total_inodes + 1; i++) {
		node_list[i].type = INODE_FREE;
		node_list[i].nlink = 0;

		node_list[i].reuse = 0;
		node_list[i].size = 0;
		for (j = 0; j < NUM_DIRECT; j++) {
		    node_list[i].direct[j] = 0;
		}
		node_list[i].indirect = 0;

		FNode *temp = fnh;
		while (temp->next != NULL) {
		    temp = temp->next;
		}
		temp->next = (FNode *) malloc(sizeof(FNode));
		temp->num = i;
    }
	
	void *msg = malloc(8*sizeof(char));
	
    TracePrintf(0, "[Server Process] Waiting on message... \n");

    while (1) {
    	int pid = Receive(msg);
    	if (pid != 0) {
    		TracePrintf(0, "[Server Process] received message from %i\n", pid);
    	}
    }
}

void switchRequest(void *msg) {
    struct my_msg *rec_msg = (struct my_msg*) msg;

    switch(rec_msg->fid) {
    	case 0:
    		YFSOpen(rec_msg);
    		break;
    	case 1:
    		YFSClose(rec_msg);
    		break;
    	case 2:
    		YFSCreate(rec_msg);
    		break;
		case 3:
			YFSRead(rec_msg);
    		break;
    	case 4:
    		YFSWrite(rec_msg);
    		break;
    	case 5:
    		YFSSeek(rec_msg);
    		break;
		case 6:
			YFSLink(rec_msg);
    		break;
    	case 7:
    		YFSUnlink(rec_msg);
    		break;
    	case 8:
    		YFSSymLink(rec_msg);
    		break;
		case 9:
			YFSReadLink(rec_msg);
    		break;
    	case 10:
    		YFSMkDir(rec_msg);
    		break;
    	case 11:
    		YFSRmDir(rec_msg);
    		break;
		case 12:
			YFSChDir(rec_msg);
    		break;
    	case 13:
    		YFSStat(rec_msg);
    		break;
    	case 14:
    		YFSSync(rec_msg);
    		break;
		case 15:
			YFSShutdown(rec_msg);
    		break;
    	default:
    		TracePrintf(0, "[Server Process] received invalid procedure request of %i\n", rec_msg->fid);
    		break;
    }
}

void YFSOpen(struct my_msg msg) {

}

void YFSClose(struct my_msg msg) {

}

void YFSCreate(struct my_msg msg) {

}

void YFSRead(struct my_msg msg) {

}

void YFSWrite(struct my_msg msg) {

}

void YFSSeek(struct my_msg msg) {

}

void YFSLink(struct my_msg msg) {

}

void YFSUnlink(struct my_msg msg) {

}

void YFSSymLink(struct my_msg msg) {

}

void YFSReadLink(struct my_msg msg) {

}

void YFSMkDir(struct my_msg msg) {

}

void YFSRmDir(struct my_msg msg) {

}

void YFSChDir(struct my_msg msg) {

}

void YFSStat(struct my_msg msg) {

}

void YFSSync(struct my_msg msg) {

}

void YFSShutdown(struct my_msg msg) {

}



