#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include "types.h"
#include <stdio.h>
/* Sends REQUEST_DEV to all other processes */
void requestDevice(int rank);
/* Sends REQUEST_LAB to all other processes */
void requestStation(int rank);
/* Decides priority between processes */
bool hasPriority(int myRank, int myTs, int otherRank, int otherTs);

/* Creates a message type for packet_t */
void initPacketType();

/* wysyłanie pakietu, skrót: wskaźnik do pakietu (0 oznacza stwórz pusty pakiet), do kogo, z jakim typem */
void sendPacket(int destination, int tag, int requestTS);

/* State change, uses mutex */
void changeState(state_t);

/* Debug - works like printf when difined - else works as an empty instruction*/
#ifdef DEBUG
#define debug(FORMAT,...) printf("%c[%d;%dm [%02d]::TS[%06d]::ACK[%02d]::DEV[%06d]::STATE[%02d]::::" FORMAT "%c[%d;%dm\n",  27, (1+(rank/7))%2, 31+(6+rank)%7, rank, ts, ackNum, counterDev, state, ##__VA_ARGS__, 27,0,37);
#else
#define debug(...) ;
#endif
/* Used like printf, adds color and tags for timestamp, current ackCounter value and devCounter*/
#define println(FORMAT,...) printf("%c[%d;%dm [%02d]::TS[%06d]::ACK[%02d]::DEV[%06d]::STATE[%02d]::::" FORMAT "%c[%d;%dm\n",  27, (1+(rank/7))%2, 31+(6+rank)%7, rank, ts, ackNum, counterDev, state, ##__VA_ARGS__, 27,0,37);

#endif
