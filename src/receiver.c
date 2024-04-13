#include "player.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int receiver_main(int argc, char ** argv) {
	assert(strcmp(argv[1], "receiver") == 0 && "This is not receiver?");
	printf("Receiver started at %s ...\n", get_current_time_str());

	int fd = ({
		int r = socket(AF_INET, SOCK_DGRAM, 0);
		exit_if(r == -1, EXIT_FAILURE, "socket failed");
		r;
	});

	{
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));

		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port = htons(RECV_SOCK_PORT);
		int r;
		r = bind(fd, (const struct sockaddr *) &addr, sizeof(addr));
		exit_if(r == -1, EXIT_FAILURE, "bind failed");
		// r = listen(fd, 3);
		// exit_if(r == -1, EXIT_FAILURE, "listen failed");
	}

#define BUFFER_LEN 1024
	char buffer[BUFFER_LEN] = {0};

	// subtract 1 for the null terminator at the end
	ssize_t n = recvfrom(fd, buffer, BUFFER_LEN, 0, (struct sockaddr *) NULL,
						 (socklen_t *) NULL);
	exit_if(n == -1, EXIT_FAILURE, "read failed");

	printf("receiver received %zd chars.\n", n);

	close(fd);
	printf("receiver ended.\n");
	return 0;
}