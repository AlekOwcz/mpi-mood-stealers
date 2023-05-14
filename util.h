#ifndef UTILH
#define UTILH
#include "main.h"
#include <stdbool.h>

/* typ pakietu */
typedef struct {
    int ts;       /* timestamp (zegar lamporta */
    int rank;      /* rank */
} packet_t;
/* packet_t ma dwa pola, więc NITEMS=2. Wykorzystane w inicjuj_typ_pakietu */
#define NITEMS 2

/* Typy wiadomości */
/* TYPY PAKIETÓW */
#define ACK_DEV     1
#define ACK_LAB     2
#define REQUEST_DEV 3
#define REQUEST_LAB 4
#define RELEASE     5
#define APP_PKT     6
#define FINISH      7

/* Wartosc symbolizujaca nulla w kolejkach */
#define INF 999999999
/* zarzadzanie kolejkami */
void pushBack(packet_t* queue, int rank, int ts);
int popFirst(packet_t* queue);
void resetQueue(packet_t* queue);
void sortQueue(packet_t* queue);

/* Rozstrzyga priorytet miedzy soba a innym procesem */
bool hasPriority(int myRank, int myTs, int otherRank, int otherTs);
int getTsByRank(packet_t* queue, int rank);

extern MPI_Datatype MPI_PAKIET_T;
void inicjuj_typ_pakietu();

/* wysyłanie pakietu, skrót: wskaźnik do pakietu (0 oznacza stwórz pusty pakiet), do kogo, z jakim typem */
void sendPacket(packet_t *pkt, int destination, int tag);

typedef enum {InStart, InPrepare, InAwaitDevice, InHunt, InAwaitLab, InWorkLab, InFinish} state_t;
extern state_t stan;
extern pthread_mutex_t stateMut;
/* zmiana stanu, obwarowana muteksem */
void changeState( state_t );

#endif
