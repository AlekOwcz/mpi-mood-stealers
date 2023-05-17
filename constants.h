#ifndef CONSTANTS_H
#define CONSTANTS_H

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

#define MESSAGE_TYPE_COUNT 5

#define TRUE 1
#define FALSE 0
#define SEC_IN_STATE 1
#define STATE_CHANGE_PROB 10
#define QUEUE_CAPACITY 100
#define ROOT 0

/* Wartosc symbolizujaca nulla w kolejkach */
#define INF 999999999

#endif