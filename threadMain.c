#include "variables.h"
#include "constants.h"
#include "types.h"
#include "util.h"
#include "threadMain.h"
#include "queue.h"
#include <stdlib.h>
#include <unistd.h>


void mainLoop()
{
	changeState(InStart);
    srandom(rank);
    // int tag;
    // int perc;/

	while (1){
		switch (stan){
			case InStart:
				start();
				break;
			case InPrepare:
				prepare();
				break;
			case InAwaitDevice:
				awaitDevice();
				break;
			case InHunt:
				hunt();
				break;
			case InAwaitLab:
				awaitLab();
				break;
			case InWorkLab:
				workLab();
				break;
			case InFinish:
				return; //
		}
		sleep(SEC_IN_STATE);
	}
}

// 5.1
void start(){
	// 5.1.1
	counterDev = num_dev;
	// 5.1.2
	ackNum = 0;
	resetQueue(devReqs);
	resetQueue(labReqs);
	// 5.1.3
	changeState(InPrepare);
};

// 5.2
void prepare(){
	// 5.2.1
	println("Nic nie robie");
	int perc;
	do{
		perc = random()%100;
		sleep(SEC_IN_STATE);
	} while(perc < 90); // 10% chance to break
	// 5.2.2
	pthread_mutex_lock(&devReqsLock); // dodanie sie do kolejki musi miec ten sam ts (i kolejke?)
	pthread_mutex_lock(&tsLock);	  // co mial w chwili zmiany stanu
	changeState(InAwaitDevice);		  // inaczej dochodzilo do zakleszczenia 
};									  // (jeden mial ts = -1 bo juz byl w stanie await a jeszcze nie byl w kolejce)

// 5.3
void awaitDevice(){
	println("Czekam na wejście do sekcji krytycznej");
	// 5.3.1
	pthread_mutex_lock(&counterLock);
	counterDev--;
	pthread_mutex_unlock(&counterLock);
	// ?.?.? Dodaje sie do kolejki zeby mial jak sie porownac dla innych requestow
	pushBack(devReqs, rank, ts);
	pthread_mutex_unlock(&devReqsLock);
	pthread_mutex_unlock(&tsLock);
	// 5.3.2
	packet_t *pkt = malloc(sizeof(packet_t));
	pthread_mutex_lock(&ackLock);
	//ackNum = 0; // moja inwencja (na wszelki wypadek, nw czy cos zmienia)
	pthread_mutex_unlock(&ackLock);
	for (int i=0;i<=num_thief-1;i++)
		if (i!=rank)
			sendPacket( pkt, i, REQUEST_DEV);
	free(pkt);
	// 5.3.3
	while(1) {
		if(ackNum == num_thief - 1 && counterDev >= 0)
			break;
	}
	// ?.?.? Usuwa sie z kolejki
	pthread_mutex_lock(&devReqsLock);
	popFirst(devReqs); // self
	pthread_mutex_unlock(&devReqsLock);
	// 5.3.4
	pthread_mutex_lock(&ackLock);
	ackNum = 0;
	pthread_mutex_unlock(&ackLock);
	// 5.3.5
	changeState(InHunt);
};

// 5.4
void hunt(){
	// 5.4.1
	packet_t *pkt = malloc(sizeof(packet_t));
	for (int i=0;i<=QUEUE_CAPACITY;i++){
		if(devReqs[i].rank != INF){
			sendPacket(pkt, devReqs[i].rank, ACK_DEV);
		}
	}
	// 5.4.2
	println("Jestem w sekcji krytycznej");
	int huntDuration = 5 + random()%5;
	sleep(huntDuration);
	// 5.4.3
	int prayFoundChance = 80;
	if(random()%100 < prayFoundChance){	// 5.4.3.1
		println("Wychodze z sekcji krytycznej - Znalazlem ofiare");
		// 5.4.3.1.1
		pthread_create( &threadDevice, NULL, releaseDevice, 0);
		// 5.4.3.1.2
		changeState(InAwaitLab);
	} else{	// 5.4.3.2
		println("Wychodze z sekcji krytycznej - Nie znalazlem ofiary");
		// 5.4.3.2.1
		packet_t *pkt = malloc(sizeof(packet_t));
		for (int i=0;i<=num_thief-1;i++)
			sendPacket(pkt, i, RELEASE);
		free(pkt);
		// 5.4.3.2.2
		changeState(InPrepare);
	}
};

// TODO
void awaitLab(){
	println("Robotaju w Labie");
	sleep(5);
	changeState(InPrepare);
};

// TODO
void workLab(){
	
}

void *releaseDevice(void *ptr){
	// 5.4.3.1.1.1 TODO
	sleep(5);
	println("Sprzet zostal naladowany");
	// 5.4.3.1.1.2
	packet_t *pkt = malloc(sizeof(packet_t));
	for (int i=0;i<=num_thief-1;i++)
		sendPacket(pkt, i, RELEASE);
	free(pkt);
	// 5.4.3.1.1.3
	pthread_exit(NULL);
}