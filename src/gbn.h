#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
	Gbn_Unknown,
	Gbn_Ack,
	Gbn_Corrupt,
} GbnPktStat;

/**
 * gbn packet format:
 *  | head | : 4 bytes head, "ACK\0" for ack, "SND\0" for snd.
 *  | seqnum | : a 32-bit (4 bytes) unsigned int, sequent number.
 *  | chksum | : a 16-bit (2 bytes) unsigned int, chksum.
 *  | buflen | : a 16-bit (2 bytes) unsigned int, buffer length in byte this packet contains.
 *  | buf | : buffer. This buffer occupies `buflen` bytes. Note this buf is not needed to end with '\0'.
 */
typedef struct {
	char head[4];
	uint32_t seqnum;
	uint16_t chksum;
	uint16_t buflen;
	char buf[0];
} __attribute__((packed)) RawGbnPacket;

RawGbnPacket * new_raw_gbd_packet(const char * p);

typedef struct {
	GbnPktStat stat;
	char * buf;
} GbnPacket;

GbnPacket * new_packet(void * /*place holder*/);

GbnPacket * new_packet(GbnPktStat s, char * b) __attribute__((overloadable));

void delete_packet(GbnPacket * p, bool recursive);