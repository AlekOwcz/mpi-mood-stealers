#ifndef VARIABLES_H
#define VARIABLES_H

#include "types.h"
#include <pthread.h>

extern int counterDev;
extern int rank;
extern int numThief;
extern int numDev;
extern int numLab; 

extern int* devReqs;
extern int* labReqs;

extern MPI_Datatype MPI_PACKET_T;
extern int ackNum;
extern int ts;

extern pthread_mutex_t tsLock;
extern pthread_mutex_t counterLock;
extern pthread_mutex_t ackLock;
extern pthread_mutex_t devReqsLock;
extern pthread_mutex_t stateMut;

extern state_t stan;

extern pthread_t threadCom, threadDevice;

extern tagNames_t tagNames[];
#endif