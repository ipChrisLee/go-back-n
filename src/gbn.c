#include "gbn.h"
#include <stdlib.h>

RawGbnPacket * new_raw_gbd_packet(const char * p) {

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