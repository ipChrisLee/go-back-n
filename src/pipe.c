#include "player.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>

int pipe_main(int argc, char ** argv) {
	assert(strcmp(argv[1], "pipe") == 0 && "This is not pipe?");
	printf("Pipe started...");
	TODO("pipe.");


	return 0;
}