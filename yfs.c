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

struct all_file_and_dirs {
	struct all_file_and_dirs *next;
	struct dir_entry *entry;
};

struct dir_entry_storage {
	struct dir_entry *cur;
	struct dir_entry *next;
};

FBlock *fbh;
FNode *fnh;
struct all_file_and_dirs *files_and_dirs;

int file_inode_num[MAX_OPEN_FILES];
int last_write_idx[MAX_OPEN_FILES];


int
main(int argc, char **argv)
{
    /** 
     * Fork the server and user processes 
     */
    int result = Register(1);
    int pid = Fork();
    if (pid == 0) Exec(argv[1], argv + 1);


    TracePrintf(0, "[Server Process] argument count is %s\n", argv[1]);


    /**
     * Retrieve the header and get total number of inodes and blocks
     */
	struct fs_header *header = (struct fs_header *) malloc(sizeof(struct fs_header));
    void *buf = malloc(SECTORSIZE);
    int i, j;

    fbh = (FBlock *) malloc(sizeof(FBlock));
    fnh = (FNode *) malloc(sizeof(FNode));
    files_and_dirs = (struct all_file_and_dirs*) malloc(sizeof(struct all_file_and_dirs));

    TracePrintf(0, "[Server Process] Starting...\n");

    int ret = ReadSector(1, buf);
    memcpy(header, buf, sizeof(struct fs_header));

    total_inodes = header->num_inodes;
    total_blocks = header->num_blocks;

    TracePrintf(0, "[Server Process] num_inodes %i, num_blocks %i\n", header->num_inodes, header->num_blocks);
	
    node_list = malloc(sizeof(struct inode) * (total_inodes));

    /* Update local cache */
    for (i = 0; i < MAX_OPEN_FILES; i++) {
    	file_inode_num[i] = -1;
    	last_write_idx[i] = -1;
    }


	
	/**
	 * Initialize the root note 
	 */
	node_list[0].type = INODE_DIRECTORY;
	node_list[0].nlink = 0;
	node_list[0].reuse = 0;
	
	struct dir_entry_storage *root_dir_entries = (struct dir_entry*) malloc(sizeof(struct dir_entry_storage));
	struct dir_entry *root_dot = (struct dir_entry*) malloc(sizeof(struct dir_entry));
	struct dir_entry *root_dot_dot = (struct dir_entry*) malloc(sizeof(struct dir_entry));

	root_dot->inum = 0;//fbh->num;
	root_dot_dot->inum = 0;//fbh->num;

	root_dot->name[0] = '.';
	root_dot_dot->name[0] = '.';
	root_dot_dot->name[1] = '.';

	root_dir_entries->cur = root_dot_dot;
	root_dir_entries->next = malloc(sizeof(struct dir_entry_storage));

	struct dir_entry_storage *root_dir_temp = root_dir_entries->next;
	root_dir_temp->cur = root_dot;

	void *dir_buf = malloc(SECTORSIZE);
	memcpy(dir_buf, root_dir_entries, 2 * sizeof(struct dir_entry_storage));

	WriteSector(total_inodes * sizeof(struct inode) / BLOCKSIZE, dir_buf);



	/** 
	 * Initialize rest of inodes and free blocks
	 */
    for (i = 1; i < total_inodes; i++) {

    	/* Set properties of node*/
		node_list[i].type = INODE_FREE;
		node_list[i].nlink = 0;

		node_list[i].reuse = 0;
		node_list[i].size = 0;
		for (j = 0; j < NUM_DIRECT; j++) {
		    node_list[i].direct[j] = 0;
		}
		node_list[i].indirect = 0;


		/* Update free block list */
		FNode *temp = fnh;
		while (temp->next != NULL) {
		    temp = temp->next;
		}

		temp->next = (FNode *) malloc(sizeof(FNode));
		temp->num = i + ((total_inodes * sizeof(struct inode) + sizeof(struct fs_header)) / BLOCKSIZE);
    }
	



    /**
	 * Begin waiting for messages from user
     */
	void *msg = malloc(sizeof(struct my_msg));
	
    TracePrintf(0, "[Server Process] Waiting on message...\n");

    while (1) {
    	int pid = Receive(msg);
    	if (pid != 0) {
    		TracePrintf(0, "[Server Process] received message from %i\n", pid);
    		void *temp_msg = malloc(sizeof(struct my_msg));
    		switchRequest(pid, msg);
    	}
    }


}

void switchRequest(int pid, void *msg) {

	/**
	 * Get the message format from input buffer and execute procedure
	 */
    struct my_msg *rec_msg = (struct my_msg*) msg;
   	void *copy = malloc(8*sizeof(char));

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
			YFSRead(pid, rec_msg);
    		break;
    	case 4:
		    CopyFrom(pid, copy, rec_msg->addr_ptr, sizeof(struct my_msg));
		    rec_msg->addr_ptr = copy;
    		YFSWrite(pid, rec_msg);
    		break;
    	case 5:
    		YFSSeek(pid, rec_msg);
    		break;
		case 6:
			YFSLink(pid, rec_msg);
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
    Reply(msg, pid);
    TracePrintf(0, "replied\n");
}

void YFSOpen(struct my_msg *msg) {
}

void YFSClose(struct my_msg *msg) {
	TracePrintf(0, "[Server Process] Closing file\n");

	
}

void YFSCreate(struct my_msg *msg) {

	/*
	* TODO: FIND FREE BLOCKS
	* RESTRUCTURE CACHE
	* HARDCODED
	*
	* ADD CHECK IF THE FILE ALREADY EXISTS
	*/

	TracePrintf(0, "[Server Process] Creating file\n");
	int i = 0;

	if (fbh == NULL) return;

	/* CHECK IF THERES ROOM IN CACHE */
	int free_idx = -1;
	for (i = 0; i < MAX_OPEN_FILES; i++) {
		if (file_inode_num[i] == -1) {
			free_idx = i;
			break;
		}
	}

	if (free_idx == -1) return;

	/* ASSIGN TO FREE INODE */
	i = 0;
	while (node_list[i].type != INODE_FREE) {
		i++;
		if (i >= total_inodes) return; /* THIS IS ERROR */
	}

	node_list[i].type = INODE_REGULAR;
	node_list[i].nlink = 0;	
	node_list[i].reuse = 0;
	node_list[i].size = 0;

	node_list[i].direct[0] = fbh->num;

	struct dir_entry *new_entry = (struct dir_entry*) malloc(sizeof(struct dir_entry));
	new_entry->inum = i;//fbh->num;

	/* NEED PATHNAME PARSING LOGIC AND GOING THROUGH INODE TO FORM DIRECTORY*/
	memcpy(new_entry->name, msg->data, 30);

	fbh = fbh->next;

// 	truct all_file_and_dirs {
// 	struct all_file_and_dirs *next;
// 	struct dir_entry *entry;
// };

	struct all_file_and_dirs *new_file = (struct all_file_and_dirs*) malloc(sizeof(struct all_file_and_dirs));
	new_file->entry = new_entry;
	new_file->next = files_and_dirs;
	files_and_dirs = new_file;

	node_list[i].indirect = -1;

	file_inode_num[free_idx] = i;
	last_write_idx[free_idx] = 0;

	/* CLEAR SPACE IN FREE BLOCK */
}

void YFSRead(int pid, struct my_msg *msg) {
	TracePrintf(0, "[Server Process] Reading from file %i\n", msg->addr_ptr);

	void *copy = malloc(SECTORSIZE);

	int fd = msg->num_data;
	int inode_num = file_inode_num[fd];

	ReadSector(node_list[inode_num].direct[0], copy);
	CopyTo(pid, msg->addr_ptr, copy, sizeof(struct my_msg));
	TracePrintf(0, "read: %s\n", (char *) copy);
}

void YFSWrite(int pid, struct my_msg *msg) {
	TracePrintf(0, "[Server Process] Writing to file %s, %d\n", (char *) msg->addr_ptr, msg->data[0]);
	void *buf = malloc(SECTORSIZE);

	memcpy(buf, msg->addr_ptr, (int) msg->data[0]);

	int fd = msg->num_data;
	int inode_num = file_inode_num[fd];

	WriteSector(node_list[inode_num].direct[0], buf);
	ReadSector(node_list[inode_num].direct[0], buf);
	TracePrintf(0, "written: %s\n", (char *) buf);
}

void YFSSeek(int pid, struct my_msg *msg) {
	/* ADD CATCHES IF OFFSET IS NEGATIVE */
	switch ((int) msg->data[1]) {
		case SEEK_SET:
			last_write_idx[msg->num_data] = (int) msg->data;
			break;
		case SEEK_CUR:
			last_write_idx[msg->num_data] += (int) msg->data;
			break;
		case SEEK_END:
			/* FIGURE THIS OUT */
			break;
		default:
			TracePrintf(0, "[Server Process] Invalid whence in YFSSeek");
			return;
	}
}

void YFSLink(int pid, struct my_msg *msg) {
	int sizeold = (int) msg->data[0];
	int sizenew = (int) msg->data[1];

	char *copy = malloc(sizeold + sizenew);
	CopyFrom(pid, copy, msg->addr_ptr, sizeold + sizenew);

	char *oldname = malloc(sizeold);
	char *newname = malloc(sizenew);

	int i = 0;
	for (i = 0; i < sizeold; i++) {
		oldname[i] = copy[i];
	}

	for (i = 0; i < sizenew; i++) {
		newname[i] = copy[i + sizeold];
	}

	struct all_file_and_dirs *temp = files_and_dirs;
	while (temp != NULL) {
		if ((temp->entry)->name == oldname) break;
		temp = temp->next;
	}

	if (temp == NULL) return;

	int old_inum = (temp->entry)->inum;

	/* THIS SECTION IS NOT RIGHT FIX THIS */
	i = 0;
	while (node_list[i].type != INODE_FREE) {
		i++;
		if (i >= total_inodes) return; /* THIS IS ERROR */
	}

	node_list[i].type = INODE_REGULAR;
	node_list[i].nlink = 0;	
	node_list[i].reuse = 0;
	node_list[i].size = node_list[old_inum].size;

	node_list[i].direct[0] = node_list[old_inum].direct[0];

	node_list[old_inum].nlink++;

}

void YFSUnlink(struct my_msg *msg) {

}

void YFSSymLink(struct my_msg *msg) {

}

void YFSReadLink(struct my_msg *msg) {

}

void YFSMkDir(struct my_msg *msg) {

}

void YFSRmDir(struct my_msg *msg) {

}

void YFSChDir(struct my_msg *msg) {

}

void YFSStat(struct my_msg *msg) {

}

void YFSSync(struct my_msg *msg) {

}

void YFSShutdown(struct my_msg *msg) {

}



