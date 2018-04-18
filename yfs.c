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
    		TracePrintf(0, "[Server Process] received message\n");
    	}
    }
}
