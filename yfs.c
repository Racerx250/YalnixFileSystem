#include <comp421/yalnix.h>
#include <comp421/iolib.h>

int
main()
{
	TracePrintf(0, "[Server Process] Starting...\n");
	int result = Register(1);
	TracePrintf(0, "[Server Process] Waiting on message...\n");
	while (1) {
		/* RECEIVE LOGIC */
	}
}
