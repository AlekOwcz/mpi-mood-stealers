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
	counterDev = numDev;
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
	println("I'm preparing");
	int perc = random()%5;
	sleep(perc);
	// 5.2.2
	pthread_mutex_lock(&devReqsLock); // dodanie sie do kolejki musi miec ten sam ts (i kolejke?)
	pthread_mutex_lock(&tsLock);	  // co mial w chwili zmiany stanu
	changeState(InAwaitDevice);		  // inaczej dochodzilo do zakleszczenia 
};									  // (jeden mial ts = -1 bo juz byl w stanie await a jeszcze nie byl w kolejce)

// 5.3
void awaitDevice(){
	println("I'm awaiting a device");
	// 5.3.1
	pthread_mutex_lock(&counterLock);
	counterDev--;
	pthread_mutex_unlock(&counterLock);
	// ?.?.? Dodaje sie do kolejki zeby mial jak sie porownac dla innych requestow
	addToQueue(devReqs, rank, ts);
	pthread_mutex_unlock(&devReqsLock);
	pthread_mutex_unlock(&tsLock);

	// pthread_mutex_lock(&ackLock);
	// //ackNum = 0; // moja inwencja (na wszelki wypadek, nw czy cos zmienia)
	// pthread_mutex_unlock(&ackLock);
	// 5.3.2
	requestDevice(rank);


	// 5.3.3
	while(1) {
		if(ackNum == numThief - 1 && counterDev >= 0) ////TODO: CHANGE ACTIVE WAIT - MUTEX AND MESSAGE
			break;
	}
	// ?.?.? Usuwa sie z kolejki
	pthread_mutex_lock(&devReqsLock);
	leaveQueue(devReqs, rank); // self
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
	for (int i = 0; i < numThief; i++){
		if(devReqs[i] != INF){
			sendPacket(i, ACK_DEV);
		}
	}
	// 5.4.2
	println("I have acquired a device");
	int huntDuration = random()%1000;
	sleep(huntDuration/100);
	// 5.4.3
	if(random()%1000 <= huntDuration){	// 5.4.3.1
		println("Returning device - I found prey!");
		// 5.4.3.1.1
		pthread_create(&threadDevice, NULL, releaseDevice, 0);
		// 5.4.3.1.2
		changeState(InAwaitLab);
	} else{	// 5.4.3.2
		println("Returning device - I couldn't find prey!");
		// 5.4.3.2.1
		for (int i = 0; i < numThief; i++)
			sendPacket(i, RELEASE);
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
	sleep(CHARGE_TIME);
	println("The device finished charging!");
	// 5.4.3.1.1.2
	for (int i = 0; i < numThief; i++)
		sendPacket(i, RELEASE);
	// 5.4.3.1.1.3
	pthread_exit(NULL);
}