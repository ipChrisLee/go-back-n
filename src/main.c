#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "player.h"

int main(int argc, char ** argv) {
	char * player = argv[1];
	if (strcmp(player, "sender") == 0) {
		return sender_main(argc, argv);
	} else if (strcmp(player, "receiver") == 0) {
		return receiver_main(argc, argv);
	} else if (strcmp(player, "pipe") == 0) {
		return pipe_main(argc, argv);
	} else {
		fprintf(stderr, "Don't have player %s", player);
		perror("Can't understand my rule.");
		return EXIT_FAILURE;
	}
}
