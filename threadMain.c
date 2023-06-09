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
		switch (state){
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
	// ?.?.? wew. requestDevice dodaje sie do kolejki zeby mial jak sie porownac dla innych requestow
	// 5.3.2
	requestDevice(rank);
	pthread_mutex_unlock(&devReqsLock);
	pthread_mutex_unlock(&tsLock);

	// 5.3.3
	pthread_mutex_lock(&ackLock);
	while(!(ackNum >= numThief - 1)){
		pthread_cond_wait(&condLock, &ackLock);
	}
	pthread_mutex_unlock(&ackLock);
	pthread_mutex_lock(&counterLock);
	while(!(counterDev >= 0)){
		pthread_cond_wait(&condLock, &counterLock);
	}
	pthread_mutex_unlock(&counterLock);
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
	pthread_mutex_lock(&devReqsLock);
	for (int i = 0; i < numThief; i++){
		if(devReqs[i] != INF){
			pthread_mutex_lock(&counterLock);
			sendPacket(i, ACK_DEV, -1);
			counterDev--;
			pthread_mutex_unlock(&counterLock);
		}
	}
	// ?.?.? clear queue
	resetQueue(devReqs);
	pthread_mutex_unlock(&devReqsLock);
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
			sendPacket(i, RELEASE, -1);
		// 5.4.3.2.2
		changeState(InPrepare);
	}
};

//5.5
void awaitLab(){
	println("I'm awaiting a lab station!");
	//5.5.1
	pthread_mutex_lock(&tsLock);
	requestStation(rank); //reqlab
	pthread_mutex_unlock(&tsLock);
	//5.5.2
	pthread_mutex_lock(&ackLock);
	while(!(ackNum >= numThief - numLab)){
		pthread_cond_wait(&condLock, &ackLock);
	}
	pthread_mutex_unlock(&ackLock);
	//5.5.3
	pthread_mutex_lock(&ackLock);
	ackNum = 0;
	pthread_mutex_unlock(&ackLock);
	//5.5.4
	changeState(InWorkLab);
};

//5.6
void workLab(){
	println("I have acquired a lab station!");
	//5.6.1
	int workDuration = random()%1000;
	sleep(workDuration/100);
	//5.6.2
	pthread_mutex_lock(&labReqsLock);
	for (int i = 0; i < numThief; i++){
		if(labReqs[i] != INF){
			sendPacket(i, ACK_LAB, labReqs[i]);
		}
	}
	resetQueue(labReqs);
	pthread_mutex_unlock(&labReqsLock);
	//5.6.3
	println("I'm leaving the lab!");
	changeState(InPrepare);
}

void *releaseDevice(void *ptr){
	// 5.4.3.1.1.1
	sleep(CHARGE_TIME);
	println("The device finished charging!");
	// 5.4.3.1.1.2
	for (int i = 0; i < numThief; i++)
		sendPacket(i, RELEASE, -1);
	// 5.4.3.1.1.3
	pthread_exit(NULL);
}
