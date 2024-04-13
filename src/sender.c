#include "player.h"

#include <asm-generic/socket.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <fcntl.h>

#include "gbn.h"

typedef struct {
	RawGbnPacket * pkt;
	size_t pktLen;
} RawGbnPacketInfo;

static RawGbnPacketInfo * create_packets(const char * filePath,
										 size_t * packetCount) {
	FILE * fp = fopen(filePath, "r");
	exit_if(fp == NULL, EXIT_FAILURE, "Input file open failed.");
	fseek(fp, 0, SEEK_END);
	long sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	// includes END packet. So do ceil div plus two.
	*packetCount = (sz - 1) / (RAW_GBN_PKT_MAXBUFLEN - 1) + 1 /*ceil div*/ + 1;
	RawGbnPacketInfo * pp =
		(RawGbnPacketInfo *) malloc((*packetCount) * sizeof(RawGbnPacketInfo));

	char * buf = (char *) malloc(RAW_GBN_PKT_MAXBUFLEN);
	off_t offset = 0;
	for (size_t i = 0; i < *packetCount - 1; ++i) {
		size_t len = 0;
		if (sz - offset > RAW_GBN_PKT_MAXBUFLEN - 1) {
			len = RAW_GBN_PKT_MAXBUFLEN - 1;
		} else {
			len = sz - offset;
		}
		ssize_t szRead = pread(fileno(fp), buf, len, offset);
		exit_if(szRead == -1, EXIT_FAILURE, "Pread failed.");
		RawGbnPacket * p = create_send_gbn_packet(i, buf, len, &pp[i].pktLen);
		pp[i].pkt = p;
		offset += szRead;
	}
	pp[*packetCount - 1].pkt =
		create_end_gbn_packet(*packetCount - 1, &pp[*packetCount - 1].pktLen);
	free(buf);
	fclose(fp);
	return pp;
}

static void deep_free_packets(RawGbnPacketInfo * p, size_t packetCount) {
	for (size_t i = 0; i < packetCount; ++i) { delete_raw_gbn_pkt(p[i].pkt); }
	free(p);
}

/**
 * Usage: go_back_n sender <filepath-to-send>
 */
int sender_main(int argc, char ** argv) {
	assert(strcmp(argv[1], "sender") == 0 && "This is not sender?");
	printf("Sender started at %s ...", get_current_time_str());
	stdout_endl();

	size_t packetCount;
	RawGbnPacketInfo * packets = create_packets(argv[2], &packetCount);

	int sockForSend = ({
		int r = socket(AF_INET, SOCK_DGRAM, 0);
		exit_if(r == -1, EXIT_FAILURE, "socket failed");
		r;
	});

	struct sockaddr_in sendToAddr;
	{
		memset(&sendToAddr, 0, sizeof(sendToAddr));
		sendToAddr.sin_family = AF_INET;
		sendToAddr.sin_port = htons(SENDER_SEND_SOCK_PORT);
		int r;
		r = inet_pton(AF_INET, "127.0.0.1", &sendToAddr.sin_addr);
		exit_if(r == -1, EXIT_FAILURE, "inet_pton failed");
	}

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
		addr.sin_port = htons(SENDER_RECV_SOCK_PORT);
		int r;
		r = bind(s, (const struct sockaddr *) &addr, sizeof(addr));
		exit_if(r == -1, EXIT_FAILURE, "recv bind failed");
		s;
	});

	size_t base = 0;
	size_t nextseq = min_size(base + GBN_N, packetCount);
	const size_t NEXTSEQ_STEP = 5;
	const size_t RETRY_NUMBER = 10;
	size_t retry = RETRY_NUMBER;

	clock_t startClock = clock();

	char * buffer = malloc(RAW_GBN_PKT_MAXLEN);

	printf("Strat transfer packetsCount=%zu", packetCount);
	stdout_endl();
	while (true) {
		ssize_t n = recvfrom(sockForRecv, buffer, RAW_GBN_PKT_MAXLEN - 1, 0,
							 NULL, NULL);
		if (n != -1) {
			RawGbnPacket * recvPkt = new_raw_gbn_packet(buffer, n);
			if (strcmp(recvPkt->head, "ACK") == 0 && chksum_ok(recvPkt, n) &&
				recvPkt->seqnum >= base) {
				printf("Receive ack packet %u.", recvPkt->seqnum);
				stdout_endl();
				base = recvPkt->seqnum + 1;
				nextseq =
					min_size(min_size(nextseq + NEXTSEQ_STEP, packetCount),
							 base + GBN_N);
				retry = RETRY_NUMBER;
			}
			delete_raw_gbn_pkt(recvPkt);
		}
		// printf("%d :: %zd", __LINE__, n);
		// stdout_endl();
		if (clock() - startClock > GBN_TIMEOUT_TK) {
			if (retry == 0) {
				printf("Retry number ran out, break.");
				stdout_endl();
				break;
			}
			startClock = clock();
			for (size_t i = base; i < nextseq; ++i) {
				ssize_t n = -1;
				while (n == -1) {
					n = sendto(sockForSend, packets[i].pkt, packets[i].pktLen,
							   0, (const struct sockaddr *) &sendToAddr,
							   sizeof(sendToAddr));
				}
				// printf("%d :: %zd", __LINE__, n);
				// stdout_endl();
			}
			printf("Resend the [%zu, %zu) packets, retry=%zu.", base, nextseq,
				   retry);
			stdout_endl();
			retry--;
		}
		if (base == packetCount) { break; }// the last ack is received, break.
	}

	free(buffer);
	close(sockForSend);
	deep_free_packets(packets, packetCount);
	printf("Sender ended.");
	stdout_endl();
	return 0;
}