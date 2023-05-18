#include <mpi.h>
#include "types.h"
#include "constants.h"
#include <pthread.h>

int counterDev;
int rank, numThief, numDev, numLab; 

int* devReqs;
int* labReqs;

MPI_Datatype MPI_PACKET_T;
int ackNum;
int ts = 0;

pthread_mutex_t tsLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t counterLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ackLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t devReqsLock = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t stateMut = PTHREAD_MUTEX_INITIALIZER;

state_t state = InStart;

pthread_t threadCom, threadDevice; //threadCom

tagNames_t tagNames[MESSAGE_TYPE_COUNT] = { 
    {"application packet", APP_PKT }, 
    {"finish", FINISH}, 
    {"accept device request", ACK_DEV},
    {"accept laboratory request", ACK_LAB},
    {"request device", REQUEST_DEV},
    {"request laboratory", REQUEST_LAB},
    {"release device", RELEASE}
};