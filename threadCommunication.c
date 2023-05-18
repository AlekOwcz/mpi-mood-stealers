#include "threadCommunication.h"

#include "util.h"
#include "variables.h"
#include "constants.h"
#include "types.h"
#include "queue.h"
#include <pthread.h>


/* Communication thread: Receiving and handling messages */
void *startComThread(void *ptr)
{
    MPI_Status status;
    packet_t packet;
    while (state!=InFinish) {
	    debug("Waiting on MPI_Recv");
        MPI_Recv( &packet, 1, MPI_PACKET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        pthread_mutex_lock(&tsLock);
        ts = max(ts, packet.ts) + 1;
        pthread_mutex_unlock(&tsLock);

        switch (status.MPI_TAG) {
            // 6.1
            case RELEASE:
                println("Received RELEASE");
                pthread_mutex_lock(&counterLock);
                // 6.1.1
                counterDev++;
                pthread_mutex_unlock(&counterLock);
                break;
            // 6.2
            case REQUEST_DEV:
                println("Received REQUEST_DEV");
                // 6.2.1
                pthread_mutex_lock( &stateMut );

                if(state == InAwaitDevice){
                    pthread_mutex_lock(&devReqsLock);
                    int myTsInQ = getTsByRank(devReqs, rank);
                    // 6.2.1.1.1.1
                    if(hasPriority(rank, myTsInQ, status.MPI_SOURCE, packet.ts)){
                        pthread_mutex_lock(&counterLock);
                        counterDev--;
                        pthread_mutex_unlock(&counterLock);
                        sendPacket(status.MPI_SOURCE, ACK_DEV);
                    } else { // 6.2.1.1.1.2
                        addToQueue(devReqs, status.MPI_SOURCE, packet.ts);
                    }
                    pthread_mutex_unlock(&devReqsLock);
                } else {
                    //  6.2.1.2
                    pthread_mutex_lock(&counterLock);
                    counterDev--;
                    pthread_mutex_unlock(&counterLock);
                    sendPacket(status.MPI_SOURCE, ACK_DEV);
                }

                pthread_mutex_unlock( &stateMut );
                break;
            // 6.4
            case ACK_DEV:
                // 6.4.1
                pthread_mutex_lock( &stateMut );
                if(state == InAwaitDevice) {
                    println("Received ACK_DEV");
                    pthread_mutex_lock(&ackLock);
                    ackNum++;
                    pthread_mutex_unlock(&ackLock);
                }
                pthread_mutex_unlock( &stateMut );
                break;
            default:
                break;
        }
    }
    pthread_exit(NULL);
}

int max(int val1, int val2) {
    if (val1 > val2)
        return val1;
    else
        return val2;
}
