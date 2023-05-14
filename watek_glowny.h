#ifndef WATEK_GLOWNY_H
#define WATEK_GLOWNY_H

/* pętla główna aplikacji: zmiany stanów itd */
void mainLoop();

/* funkcje stanów */
void start();
void prepare();
void awaitDevice();
void hunt();
void awaitLab();
void workLab();

/* funkcja zwalniania sprzetu po czasie */
void *releaseDevice(void *ptr);

#endif
