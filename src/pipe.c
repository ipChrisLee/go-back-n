#include "player.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "gbn.h"

/**
 * Usage: go_back_n pipe <s2r|r2s> <pLoss> <pCorrupt>
 */
int pipe_main(int argc, char ** argv) {
	assert(strcmp(argv[1], "pipe") == 0 && "This is not pipe?");
	printf("Pipe started at %s ...", get_current_time_str());
	stdout_endl();

	uint16_t srcPort, dstPort;

	if (strcmp(argv[2], "s2r") == 0) {
		srcPort = SENDER_SEND_SOCK_PORT;
		dstPort = RECEIVER_RECV_PORT;
	} else if (strcmp(argv[2], "r2s") == 0) {
		srcPort = RECEIVER_SEND_PORT;
		dstPort = SENDER_RECV_SOCK_PORT;
	} else {
		assert(0 && "Unsupported argv[2]");
	}

	double pLoss = strtod(argv[3], NULL);
	double pCorrupt = strtod(argv[4], NULL);

	srand(1);// fixed

	int fdFromSrc = ({
		int sock = socket(AF_INET, SOCK_DGRAM, 0);
		exit_if(sock == -1, EXIT_FAILURE, "socket failed");

		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));

		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port = htons(srcPort);
		int r;
		r = bind(sock, (const struct sockaddr *) &addr, sizeof(addr));
		exit_if(r == -1, EXIT_FAILURE, "bind failed");
		sock;
	});

	int fdToDst = ({
		int r = socket(AF_INET, SOCK_DGRAM, 0);
		exit_if(r == -1, EXIT_FAILURE, "socket failed");
		r;
	});

	struct sockaddr_in dstAddr;
	{
		memset(&dstAddr, 0, sizeof(dstAddr));
		dstAddr.sin_family = AF_INET;
		dstAddr.sin_port = htons(dstPort);
		int r;
		r = inet_pton(AF_INET, "127.0.0.1", &dstAddr.sin_addr);
		exit_if(r == -1, EXIT_FAILURE, "inet_pton failed");
	}

	char * data = (char *) malloc(RAW_GBN_PKT_MAXLEN);

	uint32_t packetCnt = 0;
	while (true) {
		ssize_t n = recvfrom(fdFromSrc, data, RAW_GBN_PKT_MAXLEN - 1, 0,
							 (struct sockaddr *) NULL, (socklen_t *) NULL);
		if (n == -1) {
			// printf("recvfrom error, %s", strerror(errno));
			// stdout_endl();
			continue;
		}
		printf("For packetCnt[%u], recvfrom returns %zd.", packetCnt, n);
		stdout_endl();
		++packetCnt;

		if (rand_uniform() < pLoss) {
			printf("Packet[%u] loss.", packetCnt);
			stdout_endl();
			continue;// loss
		}
		if (rand_uniform() < pCorrupt) {
			int idx = rand_int(0, (int) n);
			data[idx] = (signed char) ~data[idx];// flips for corrupt.
			printf("Packet[%u] is corrupted at idx=%i.", packetCnt, idx);
			stdout_endl();
		}

		n = sendto(fdToDst, data, n, 0, (const struct sockaddr *) &dstAddr,
				   sizeof(dstAddr));
		exit_if(n == -1, EXIT_FAILURE, "pipe send failed");
		printf("Packet[%u] is transferred.", packetCnt);
		stdout_endl();
	}

	return 0;
}