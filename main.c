#include "main.h" //All necessary includes


void finalize()
{
    pthread_mutex_destroy(&tsLock);
    pthread_mutex_destroy(&counterLock);
    pthread_mutex_destroy(&ackLock);
    pthread_mutex_destroy(&devReqsLock);
    pthread_mutex_destroy(&labReqsLock);
    pthread_mutex_destroy(&stateMut);
    pthread_cond_destroy(&condLock);

    /* Wait for child threads */
    println("Awaiting communication thread\n" );
    pthread_join(threadCom,NULL);
    println("Awaiting device freeing thread\n" );
    pthread_join(threadDevice, NULL);
    MPI_Type_free(&MPI_PACKET_T);
    MPI_Finalize();
}

void checkThreadSupport(int provided)
{
    printf("THREAD SUPPORT: Expected: %d, Provided:\n", provided);
    switch (provided) {
        case MPI_THREAD_SINGLE: 
            printf("\t Lacking thread support - exitting...\n");
            fprintf(stderr, "Lacking thread support\n");
            MPI_Finalize();
            exit(-1);
            break;
        case MPI_THREAD_FUNNELED: 
            printf("\t Only threads that execute mpi_init_thread can make calls to MPI library\n");
	        break;
        case MPI_THREAD_SERIALIZED: 
            printf("\t Only one thread at a time can make calls to MPI library\n");
	        break;
        case MPI_THREAD_MULTIPLE: 
            printf("\t Full thread support\n");
	        break;
        default: 
            printf("\t Unknown\n");
    }
    return;
}


int main(int argc, char **argv)
{
    if (argc != 3){
        printf("\n======\nFormat: make run THIEF={number of mood stealers} DEV={number of devices} LAB={number of stations in the laboratory}\n======\n\n");
        return -1;
    }

    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    checkThreadSupport(provided);

    srand(rank);

    initPacketType(); 
    MPI_Comm_size(MPI_COMM_WORLD, &numThief);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    numDev = atoi(argv[1]);
    numLab = atoi(argv[2]);

    devReqs = malloc(sizeof(int)*numThief);
    labReqs = malloc(sizeof(int)*numThief);

    pthread_create(&threadCom, NULL, startComThread , 0);

    mainLoop();

    finalize();

    return 0;
}

