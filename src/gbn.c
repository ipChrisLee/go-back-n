#include "gbn.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint16_t chksum(void * p, size_t size) {
	uint8_t * data = (uint8_t *) p;
	uint32_t sum = 0;

	while (size > 1) {
		sum = sum + *((uint16_t *) data);
		if (sum >> 16) { sum = (sum & 0xFFFF) + (sum >> 16); }
		data += 2;
		size -= 2;
	}

	if (size > 0) {
		sum = sum + (((uint16_t) *data));
		if (sum >> 16) { sum = (sum & 0xFFFF) + (sum >> 16); }
	}

	return ~sum;
}

RawGbnPacket * new_raw_gbn_packet(const char * data, ssize_t size) {
	RawGbnPacket * p = (RawGbnPacket *) malloc(size);
	memcpy(p, data, size);
	return p;
}

RawGbnPacket * create_send_gbn_packet(uint32_t seqnum, const char * buf,
									  size_t buflen, size_t * pktLen) {
	assert(buflen < RAW_GBN_PKT_MAXBUFLEN &&
		   "buflen is larger than max buf len of RawGbnPacket.");
	RawGbnPacket * p = (RawGbnPacket *) malloc(12 + buflen);
	if (pktLen) { *pktLen = 12 + buflen; }
	strcpy(p->head, "SND\0");
	p->seqnum = seqnum;
	p->chksum = 0;
	p->buflen = (uint16_t) buflen;
	memcpy(p->buf, buf, buflen);
	p->chksum = chksum(p, 12 + buflen);
	return p;
}

RawGbnPacket * create_send_gbn_packet(uint32_t seqnum, const char * str,
									  size_t * pktLen)
	__attribute__((overloadable)) {
	size_t strLen = strlen(str);
	return create_send_gbn_packet(seqnum, str, strLen, pktLen);
}

RawGbnPacket * create_end_gbn_packet(uint32_t seqnum, size_t * pktLen) {
	RawGbnPacket * p = (RawGbnPacket *) malloc(12);
	strcpy(p->head, "END");
	p->seqnum = seqnum;
	p->chksum = 0;
	p->buflen = 0;
	if (pktLen) { *pktLen = 12; }
	p->chksum = chksum(p, 12);
	return p;
}

RawGbnPacket * create_ack_gbn_packet(uint32_t seqnum, size_t * pktLen) {
	RawGbnPacket * p = (RawGbnPacket *) malloc(12);
	strcpy(p->head, "ACK");
	p->seqnum = seqnum;
	p->chksum = 0;
	p->buflen = 0;
	if (pktLen) { *pktLen = 12; }
	p->chksum = chksum(p, 12);
	return p;
}

void delete_raw_gbn_pkt(RawGbnPacket * p) {
	if (p) free(p);
}

void raw_gbn_pkt_dump(RawGbnPacket * p, bool printBuffer) {
	if (!p) {
		printf("RawGbnPacket <NULL>");
	} else {
		printf("RawGbnPacket {head=%s seqnum=%u chksum=%hx buflen=%hu}",
			   p->head, p->seqnum, p->chksum, p->buflen);
		if (printBuffer) {
			char * b = malloc(p->buflen + 1);
			memcpy(b, p->buf, p->buflen);
			b[p->buflen - 1] = 0;
			printf(" \"%s\"", b);
			free(b);
		}
	}
}

bool chksum_ok(RawGbnPacket * p, size_t pktLen) {
	return chksum(p, pktLen) == 0;
}

GbnPacket * new_packet(void * _) {
	GbnPacket * p = malloc(sizeof(GbnPacket));
	p->stat = Gbn_Unknown;
	p->buf = NULL;
	return p;
}

GbnPacket * new_packet(GbnPktStat s, char * b) __attribute__((overloadable)) {
	GbnPacket * p = malloc(sizeof(GbnPacket));
	p->stat = s;
	p->buf = b;
	return p;
}

void delete_packet(GbnPacket * p, bool recursive) {
	if (recursive) { free(p->buf); }
	free(p);
}