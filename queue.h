#ifndef QUEUE_H
#define QUEUE_H
#include "types.h"

void pushBack(packet_t*, int, int ts);
int popFirst(packet_t*);
void resetQueue(packet_t*);
void sortQueue(packet_t*);
int getTsByRank(packet_t* queue, int rank);

#endif