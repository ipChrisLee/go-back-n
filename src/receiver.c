#include "gbn.h"
#include "player.h"

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

/**
 * Usage: go_back_n receiver <filepath-to-save>
 */
int receiver_main(int argc, char ** argv) {
	assert(strcmp(argv[1], "receiver") == 0 && "This is not receiver?");
	printf("Receiver started at %s ...", get_current_time_str());
	stdout_endl();

	FILE * fp = ({
		FILE * f = fopen(argv[2], "w");
		exit_if(f == NULL, EXIT_FAILURE, "fopen failed");
		f;
	});

	int sockForRecv = ({
		int s = socket(AF_INET, SOCK_DGRAM, 0);
		exit_if(s == -1, EXIT_FAILURE, "socket failed");

		// non-blocking
		int flags = fcntl(s, F_GETFL, 0);
		flags = flags | O_NONBLOCK;
		fcntl(s, F_SETFL, flags);

		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));

		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port = htons(RECEIVER_RECV_PORT);
		int r;
		r = bind(s, (const struct sockaddr *) &addr, sizeof(addr));
		exit_if(r == -1, EXIT_FAILURE, "bind failed");
		s;
	});

	int sockForSend = ({
		int r = socket(AF_INET, SOCK_DGRAM, 0);
		exit_if(r == -1, EXIT_FAILURE, "socket failed");
		r;
	});

	struct sockaddr_in sendBackAddr;
	{
		memset(&sendBackAddr, 0, sizeof(sendBackAddr));
		sendBackAddr.sin_family = AF_INET;
		sendBackAddr.sin_port = htons(RECEIVER_SEND_PORT);
		int r;
		r = inet_pton(AF_INET, "127.0.0.1", &sendBackAddr.sin_addr);
		exit_if(r == -1, EXIT_FAILURE, "inet_pton failed");
	}

	char * data = (char *) malloc(RAW_GBN_PKT_MAXLEN);

	const size_t END_ACK_RETRY_NUMBER = 1000;
	// send ack per x times try to recvfrom
	const size_t RECV_ACK_RETRY_NUMBER = 10000;
	uint16_t expectedSeqnum = 0;
	size_t ackPktLen;
	RawGbnPacket * ackPkt = NULL;

	while (true) {
		size_t recvRetryCount = RECV_ACK_RETRY_NUMBER;
		ssize_t n = -1;
		while (n == -1) {
			n = recvfrom(sockForRecv, data, RAW_GBN_PKT_MAXLEN - 1, 0,
						 (struct sockaddr *) NULL, (socklen_t *) NULL);
			if (recvRetryCount == 0 && ackPkt) {
				sendto(sockForSend, ackPkt, ackPktLen, 0,
					   (const struct sockaddr *) &sendBackAddr,
					   sizeof(sendBackAddr));
				printf("Send ack packet %u.", ackPkt->seqnum);
				stdout_endl();
				recvRetryCount = RECV_ACK_RETRY_NUMBER;
			}
			--recvRetryCount;
		}
		delete_raw_gbn_pkt(ackPkt);
		ackPkt = NULL;
		RawGbnPacket * pkt = new_raw_gbn_packet(data, n);

		bool leave = false;
		if (!chksum_ok(pkt, n)) {
			printf("Received packet chksum failed, skip.");
			stdout_endl();
		} else if (strcmp(pkt->head, "SND") == 0 ||
				   strcmp(pkt->head, "END") == 0) {
			if (pkt->seqnum == expectedSeqnum) {
				fwrite(pkt->buf, sizeof(char), pkt->buflen, fp);
				ackPkt = create_ack_gbn_packet(expectedSeqnum, &ackPktLen);
				++expectedSeqnum;
				leave = (strcmp(pkt->head, "END") == 0);
				printf("Received expected packet %u.", pkt->seqnum);
				stdout_endl();
			} else {
				printf("Seqnum expected %u, got %u.", expectedSeqnum,
					   pkt->seqnum);
				stdout_endl();
			}
		} else {
			printf("Unknown packet.");
			stdout_endl();
		}
		delete_raw_gbn_pkt(pkt);
		pkt = NULL;
		if (leave) {
			// flush out the last ack to avoid to much retry on sender.
			for (size_t i = 0; i < END_ACK_RETRY_NUMBER; ++i) {
				sendto(sockForSend, ackPkt, ackPktLen, 0,
					   (const struct sockaddr *) &sendBackAddr,
					   sizeof(sendBackAddr));
			}
			delete_raw_gbn_pkt(ackPkt);
			ackPkt = NULL;
			break;
		}
	}

	close(sockForRecv);
	free(data);
	fclose(fp);
	printf("Receiver ended.");
	stdout_endl();
	return 0;
}