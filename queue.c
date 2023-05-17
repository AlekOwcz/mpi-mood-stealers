#include "variables.h"
#include "constants.h"
#include "types.h"
#include <stdlib.h>
#include "queue.h"

void pushBack(packet_t* queue, int rank, int ts){
    packet_t *q = malloc(sizeof(packet_t));
	q->rank = rank;
    q->ts = ts;
    for(int i=0; i<QUEUE_CAPACITY; i++){
        if(queue[i].rank == INF){
            queue[i] = *q;
            break;
        }
    }
}

int popFirst(packet_t* queue){
    sortQueue(queue);
    int rank = queue[0].rank;
    for(int i=1; i<QUEUE_CAPACITY; i++){
        queue[i-1].rank = queue[i].rank;
        queue[i-1].rank = queue[i].rank;
        if(queue[i].rank == INF)
            break;
    }
    return rank;
}

void resetQueue(packet_t* queue){
    for(int i=0; i<QUEUE_CAPACITY; i++){
        queue[i].rank = INF;
        queue[i].ts = INF;
    }
}

void sortQueue(packet_t* queue){
    for(int i=0; i<QUEUE_CAPACITY; i++){
        for(int j=i+1; j<QUEUE_CAPACITY; j++){
            if(queue[i].ts > queue[j].ts){
                packet_t q1 = queue[i];
                queue[i] = queue[j];
                queue[j] = q1;
            }
        }
    }
}

int getTsByRank(packet_t* queue, int rank){
    for(int i=0; i<QUEUE_CAPACITY; i++){
        if(queue[i].rank == rank)
            return queue[i].ts;
    }
    return -1;
}