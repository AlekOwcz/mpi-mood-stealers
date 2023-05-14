#include "main.h"
#include "util.h"
#include <stdbool.h>
MPI_Datatype MPI_PAKIET_T;

/* 
 * w util.h extern state_t stan (czyli zapowiedź, że gdzieś tam jest definicja
 * tutaj w util.c state_t stan (czyli faktyczna definicja)
 */
state_t stan=InStart;

/* zamek wokół zmiennej współdzielonej między wątkami. 
 * Zwróćcie uwagę, że każdy proces ma osobą pamięć, ale w ramach jednego
 * procesu wątki współdzielą zmienne - więc dostęp do nich powinien
 * być obwarowany muteksami
 */
pthread_mutex_t stateMut = PTHREAD_MUTEX_INITIALIZER;

struct tagNames_t{
    const char *name;
    int tag;
} tagNames[] = { { "pakiet aplikacyjny", APP_PKT }, { "finish", FINISH}, 
                { "potwierdzenie", ACK_DEV}, {"prośbę o sekcję krytyczną", REQUEST_DEV}, {"zwolnienie sekcji krytycznej", RELEASE} };

const char *const tag2string( int tag )
{
    for (int i=0; i <sizeof(tagNames)/sizeof(struct tagNames_t);i++) {
	if ( tagNames[i].tag == tag )  return tagNames[i].name;
    }
    return "<unknown>";
}
/* tworzy typ MPI_PAKIET_T
*/
void inicjuj_typ_pakietu()
{
    /* Stworzenie typu */
    /* Poniższe (aż do MPI_Type_commit) potrzebne tylko, jeżeli
       brzydzimy się czymś w rodzaju MPI_Send(&typ, sizeof(pakiet_t), MPI_BYTE....
    */
    /* sklejone z stackoverflow */
    int       blocklengths[NITEMS] = {1, 1};
    MPI_Datatype typy[NITEMS] = {MPI_INT, MPI_INT};

    MPI_Aint     offsets[NITEMS]; 
    offsets[0] = offsetof(packet_t, ts);
    offsets[1] = offsetof(packet_t, rank);

    MPI_Type_create_struct(NITEMS, blocklengths, offsets, typy, &MPI_PAKIET_T);

    MPI_Type_commit(&MPI_PAKIET_T);
}

/* opis patrz util.h */
void sendPacket(packet_t *pkt, int destination, int tag)
{
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

void changeState( state_t newState )
{
    pthread_mutex_lock( &stateMut );
    if (stan==InFinish) { 
	    pthread_mutex_unlock( &stateMut );
        return;
    }
    stan = newState;
    pthread_mutex_unlock( &stateMut );
}

bool hasPriority(int myRank, int myTs, int otherRank, int otherTs){
    if(myTs > otherTs)
        return true;
    if(myTs == otherTs && myRank > otherRank)
        return true;
    return false;
}

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