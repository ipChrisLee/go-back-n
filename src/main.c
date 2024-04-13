#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "player.h"
#include "gbn.h"

int main(int argc, char ** argv) {
	char * player = argv[1];
	if (strcmp(player, "sender") == 0) {
		return sender_main(argc, argv);
	} else if (strcmp(player, "receiver") == 0) {
		return receiver_main(argc, argv);
	} else if (strcmp(player, "pipe") == 0) {
		return pipe_main(argc, argv);
	} else if (strcmp(player, "test") == 0) {
		uint16_t a[5] = {0x0, 0x1, 0x2, 0x3, 0x0};
		printf("%hu\n", chksum(a, sizeof(a)));
		a[4] = chksum(a, sizeof(a));
		printf("%hu\n", chksum(a, sizeof(a)));
		printf("%hu\n", a[4]);
		return 0;
	} else {
		fprintf(stderr, "Don't have player %s", player);
		perror("Can't understand my rule.");
		return EXIT_FAILURE;
	}
}
