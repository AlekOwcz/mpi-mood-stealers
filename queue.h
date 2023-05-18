#ifndef QUEUE_H
#define QUEUE_H
#include "types.h"

void addToQueue(int*, int, int);
void leaveQueue(int*, int);
void resetQueue(int*);
void sortQueue(packet_t*);
int getTsByRank(int* queue, int rank);

#endif