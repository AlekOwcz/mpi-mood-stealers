#include "variables.h"
#include "constants.h"
#include "types.h"
#include <stdlib.h>
#include "queue.h"

void addToQueue(int* queue, int rank, int ts){
    queue[rank] = ts;
}

void leaveQueue(int* queue, int rank){
    queue[rank] = INF;
}

void resetQueue(int* queue){
    for(int i = 0; i < numThief; i++){
        queue[i] = INF;
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

int getTsByRank(int* queue, int rank){
    return queue[rank];
}