#ifndef VARIABLES_H
#define VARIABLES_H

#include "types.h"
#include <pthread.h>

extern int counterDev;
extern int rank;
extern int num_thief;
extern int num_dev;
extern int num_lab; 
extern int charge;

extern packet_t devReqs[QUEUE_CAPACITY];
extern packet_t labReqs[QUEUE_CAPACITY];

extern MPI_Datatype MPI_PAKIET_T;
extern int ackNum;
extern int ts;

extern pthread_mutex_t tsLock;
extern pthread_mutex_t counterLock;
extern pthread_mutex_t ackLock;
extern pthread_mutex_t devReqsLock;
extern pthread_mutex_t stateMut;

extern state_t stan;

extern pthread_t threadKom, threadDevice;

extern tagNames_t tagNames[];
#endif