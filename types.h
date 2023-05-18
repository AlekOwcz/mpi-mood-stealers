#ifndef TYPES_H
#define TYPES_H

#include <mpi.h>
#include "constants.h"

typedef struct {
    int ts;       /* timestamp (lamport clock) */
    int rank;     /* rank */
} packet_t;

extern MPI_Datatype MPI_PACKET_T;

typedef enum {InStart, InPrepare, InAwaitDevice, InHunt, InAwaitLab, InWorkLab, InFinish} state_t;

typedef struct{
    const char *name;
    int tag;
} tagNames_t;

#endif