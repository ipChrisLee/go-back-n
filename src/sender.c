#include "player.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "gbn.h"

int sender_main(int argc, char ** argv) {
	assert(strcmp(argv[1], "sender") == 0 && "This is not sender?");
	printf("Sender started at %s ...\n", get_current_time_str());

	// new socket.
	int fd = ({
		int r = socket(AF_INET, SOCK_DGRAM, 0);
		exit_if(r == -1, EXIT_FAILURE, "socket failed");
		r;
	});

	// connect to 127.0.0.1 and sender port.
	struct sockaddr_in addr;
	{
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(RECV_SOCK_PORT);
		int r;
		r = inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
		exit_if(r == -1, EXIT_FAILURE, "inet_pton failed");
	}

#define BUFFER_LEN 1025
	char buffer[BUFFER_LEN];
	memset(buffer, 0, BUFFER_LEN);

	strcpy(buffer, "Hello from sender");

	ssize_t n = sendto(fd, buffer, strlen(buffer), 0,
					   (const struct sockaddr *) &addr, sizeof(addr));
	printf("sender send %zd chars\n", n);

	close(fd);
	printf("sender ended.");
	return 0;
}