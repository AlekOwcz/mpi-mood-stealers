#ifndef CONSTANTS_H
#define CONSTANTS_H

/* Used in initPacketType() - represents the two fields of packet_t */
#define NITEMS 2

/* Message types */
#define ACK_DEV     1
#define ACK_LAB     2
#define REQUEST_DEV 3
#define REQUEST_LAB 4
#define RELEASE     5
#define APP_PKT     6
#define FINISH      7

#define MESSAGE_TYPE_COUNT 5

/* Boolean */
#define TRUE 1
#define FALSE 0

#define SEC_IN_STATE 2
#define STATE_CHANGE_PROB 10
#define QUEUE_CAPACITY 100
#define ROOT 0

#define CHARGE_TIME 5
/* Represents NULL in queues */
#define INF 999999999

#endif