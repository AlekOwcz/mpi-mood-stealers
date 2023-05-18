#ifndef THREAD_MAIN_H
#define THREAD_MAIN_H

/* Main applicaion loop: handles state change and executes appropriate state actions */
void mainLoop();

/* State functions */
void start();
void prepare();
void awaitDevice();
void hunt();
void awaitLab();
void workLab();

/* Function handling returning device (critical section) after random time */
void *releaseDevice(void *ptr);

#endif
