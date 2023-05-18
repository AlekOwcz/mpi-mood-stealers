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

int getTsByRank(int* queue, int rank){
    return queue[rank];
}