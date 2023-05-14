#include "main.h"
#include "watek_komunikacyjny.h"

/* wątek komunikacyjny; zajmuje się odbiorem i reakcją na komunikaty */
void *startKomWatek(void *ptr)
{
    MPI_Status status;
    int is_message = FALSE;
    packet_t pakiet;
    /* Obrazuje pętlę odbierającą pakiety o różnych typach */
    while ( stan!=InFinish ) {
	    debug("czekam na recv");
        MPI_Recv( &pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        pthread_mutex_lock(&tsLock);
        ts = max(ts, pakiet.ts) + 1;
        pthread_mutex_unlock(&tsLock);

        switch ( status.MPI_TAG ) {
            // 6.1
            case RELEASE:
                println("Otrzymalem RELEASE")
                pthread_mutex_lock(&counterLock);
                // 6.1.1
                counterDev++;
                pthread_mutex_unlock(&counterLock);
                break;
            // 6.2
            case REQUEST_DEV:
                println("Otrzymalem REQUEST_DEV")
                // 6.2.1
                if(stan == InAwaitDevice){
                    pthread_mutex_lock(&devReqsLock);
                    int myTsInQ = getTsByRank(devReqs, rank);
                    // 6.2.1.1.1.1
                    if(hasPriority(rank, myTsInQ, status.MPI_SOURCE, pakiet.ts)){
                        pthread_mutex_lock(&counterLock);
                        counterDev--;
                        pthread_mutex_unlock(&counterLock);
                        sendPacket(0, status.MPI_SOURCE, ACK_DEV);
                    } else { // 6.2.1.1.1.2
                        pushBack(devReqs, status.MPI_SOURCE, pakiet.ts);
                    }
                    pthread_mutex_unlock(&devReqsLock);
                } else {
                    //  6.2.1.2
                    pthread_mutex_lock(&counterLock);
                    counterDev--;
                    pthread_mutex_unlock(&counterLock);
                    sendPacket(0, status.MPI_SOURCE, ACK_DEV);
                }
                break;
            // 6.4
            case ACK_DEV:
                // 6.4.1
                if(stan == InAwaitDevice) {
                    println("Otrzymalem ACK_DEV")
                    pthread_mutex_lock(&ackLock);
                    ackNum++;
                    pthread_mutex_unlock(&ackLock);
                }
                break;
            default:
                break;
        }
    }
}

int max(int val1, int val2) {
    if (val1 > val2)
        return val1;
    else
        return val2;
}
