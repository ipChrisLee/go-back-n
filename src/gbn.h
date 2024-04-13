#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>


// ------------------------ RawGbnPacket ------------------------

/**
 * gbn packet format:
 *  | head | : 4 bytes head, "ACK\0" for ack, "SND\0" for snd, "END\0" for end.
 *  | seqnum | : a 32-bit (4 bytes) unsigned int, sequent number.
 *  | chksum | : a 16-bit (2 bytes) unsigned int, chksum.
 *  | buflen | : a 16-bit (2 bytes) unsigned int, buffer length in byte this packet contains.
 *  | buf | : buffer. This buffer occupies `buflen` bytes. Note this buf is not required to end with '\0'.
 */
typedef struct {
	char head[4];
	uint32_t seqnum;
	uint16_t chksum;
	uint16_t buflen;
	char buf[0];
} __attribute__((packed)) RawGbnPacket;

// (1<<15) bytes. This includes tailing \0 of buf.
#define RAW_GBN_PKT_MAXLEN (1ull << 15)
// (1<<15)-4*3 bytes. This includes tailing \0 of buf.
#define RAW_GBN_PKT_MAXBUFLEN (RAW_GBN_PKT_MAXLEN - (sizeof(RawGbnPacket))) 

RawGbnPacket * new_raw_gbn_packet(const char * data, ssize_t size);
/**
 * @note: buf is deeply copied.
 */
RawGbnPacket * create_send_gbn_packet(uint32_t seqnum, const char * buf,
									  size_t buflen, size_t * pktLen);
RawGbnPacket * create_send_gbn_packet(uint32_t seqnum, const char * str,
									  size_t * pktLen)
	__attribute__((overloadable));
RawGbnPacket * create_end_gbn_packet(uint32_t seqnum, size_t * pktLen);
RawGbnPacket * create_ack_gbn_packet(uint32_t seqnum, size_t * pktLen);
RawGbnPacket * create_bye_gbn_packet(size_t * pktLen);
void delete_raw_gbn_pkt(RawGbnPacket * p);

void raw_gbn_pkt_dump(RawGbnPacket * p, bool printBuffer);

// N in go-back-n
#define GBN_N 15
#define GBN_TIMEOUT_TK (CLOCKS_PER_SEC * 2)

uint16_t chksum(void * p, size_t size);
bool chksum_ok(RawGbnPacket * p, size_t pktLen);
