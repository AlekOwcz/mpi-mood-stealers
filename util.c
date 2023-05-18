#include "util.h"
#include <stdbool.h>
#include <stdlib.h>
#include "variables.h"
#include "constants.h"
#include "types.h"
#include "queue.h"

const char *const tag2string(int tag) {
    for (int i = 0; i < MESSAGE_TYPE_COUNT; i++)
	    if (tagNames[i].tag == tag)  return tagNames[i].name;
    return "<unknown>";
}

void initPacketType() {
    int blocklengths[NITEMS] = {1, 1};
    MPI_Datatype typy[NITEMS] = {MPI_INT, MPI_INT};

    MPI_Aint offsets[NITEMS]; 
    offsets[0] = offsetof(packet_t, ts);
    offsets[1] = offsetof(packet_t, rank);

    MPI_Type_create_struct(NITEMS, blocklengths, offsets, typy, &MPI_PACKET_T);

    MPI_Type_commit(&MPI_PACKET_T);
    
    return;
}

void requestDevice(int rank){
    println("Sending REQUEST_DEV to all")
    packet_t* pkt = malloc(sizeof(packet_t));
    pkt->rank = rank;
    pkt->ts = ++ts;
    addToQueue(devReqs, rank, ts);
    for (int i = 0; i < numThief; i++)
		if (i != rank) {
            debug("Sending %s to %d", tag2string(REQUEST_DEV), i);
            MPI_Send(pkt, 1, MPI_PACKET_T, i, REQUEST_DEV, MPI_COMM_WORLD);
        }
    free(pkt);
}

void requestStation(int rank){
    println("Sending REQUEST_LAB to all")
    packet_t* pkt = malloc(sizeof(packet_t));
    pkt->rank = rank;
    pkt->ts = ++ts;
    for (int i = 0; i < numThief; i++)
		if (i != rank) {
            debug("Sending %s to %d", tag2string(REQUEST_LAB), i);
            MPI_Send(pkt, 1, MPI_PACKET_T, i, REQUEST_LAB, MPI_COMM_WORLD);
        }
    free(pkt);
}


void sendPacket(int destination, int tag) {
    println("Sending %s to %d", tag2string(tag), destination);
    packet_t* pkt = malloc(sizeof(packet_t));
    pkt->rank = rank;
    pthread_mutex_lock(&tsLock);
    pkt->ts = ++ts;
    pthread_mutex_unlock(&tsLock);
    MPI_Send(pkt, 1, MPI_PACKET_T, destination, tag, MPI_COMM_WORLD);
    free(pkt);
}

void changeState( state_t newState ) {
    pthread_mutex_lock( &stateMut );
    if (state==InFinish) { 
	    pthread_mutex_unlock( &stateMut );
        return;
    }
    state = newState;
    pthread_mutex_unlock( &stateMut );
}

bool hasPriority(int myRank, int myTs, int otherRank, int otherTs) {
    if(myTs > otherTs)
        return true;
    if(myTs == otherTs && myRank > otherRank)
        return true;
    return false;
}