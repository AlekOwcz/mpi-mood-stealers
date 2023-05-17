#include "main.h"
#include "util.h"
#include <stdbool.h>
#include <stdlib.h>
#include "variables.h"
#include "constants.h"
#include "types.h"


const char *const tag2string(int tag) {
    for (int i = 0; i < MESSAGE_TYPE_COUNT; i++)
	    if ( tagNames[i].tag == tag )  return tagNames[i].name;
    return "<unknown>";
}

void initPacketType() {
    int blocklengths[NITEMS] = {1, 1};
    MPI_Datatype typy[NITEMS] = {MPI_INT, MPI_INT};

    MPI_Aint offsets[NITEMS]; 
    offsets[0] = offsetof(packet_t, ts);
    offsets[1] = offsetof(packet_t, rank);

    MPI_Type_create_struct(NITEMS, blocklengths, offsets, typy, &MPI_PAKIET_T);

    MPI_Type_commit(&MPI_PAKIET_T);
    
    return;
}


void sendPacket(packet_t *pkt, int destination, int tag) {
    println("Wysyłam %s do %d", tag2string(tag), destination);
    int freepkt=0;
    if (pkt==0) { pkt = malloc(sizeof(packet_t)); freepkt=1;}
    pkt->ts = ts;
    pkt->rank = rank;
    pthread_mutex_lock(&tsLock);
    ts++;
    pthread_mutex_unlock(&tsLock);
    MPI_Send( pkt, 1, MPI_PAKIET_T, destination, tag, MPI_COMM_WORLD);
    if (freepkt) free(pkt);
}

void changeState( state_t newState ) {
    pthread_mutex_lock( &stateMut );
    if (stan==InFinish) { 
	    pthread_mutex_unlock( &stateMut );
        return;
    }
    stan = newState;
    pthread_mutex_unlock( &stateMut );
}

bool hasPriority(int myRank, int myTs, int otherRank, int otherTs) {
    if(myTs > otherTs)
        return true;
    if(myTs == otherTs && myRank > otherRank)
        return true;
    return false;
}