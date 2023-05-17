#include <mpi.h>
#include "types.h"
#include "constants.h"
#include <pthread.h>

int counterDev;
int rank, num_thief, num_dev = 2, num_lab = 2; //numStealer, numDev, numLab
int charge;

packet_t devReqs[QUEUE_CAPACITY];
packet_t labReqs[QUEUE_CAPACITY];

MPI_Datatype MPI_PAKIET_T;
int ackNum;
int ts = 0;

pthread_mutex_t tsLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t counterLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ackLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t devReqsLock = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t stateMut = PTHREAD_MUTEX_INITIALIZER;

state_t stan=InStart;

pthread_t threadKom, threadDevice; //threadCom

tagNames_t tagNames[MESSAGE_TYPE_COUNT] = { { "pakiet aplikacyjny", APP_PKT }, { "finish", FINISH}, 
                { "potwierdzenie", ACK_DEV}, {"prośbę o sekcję krytyczną", REQUEST_DEV}, {"zwolnienie sekcji krytycznej", RELEASE} };