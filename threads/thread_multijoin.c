#include <pthread.h>
#include "tlpi_hdr.h"

static pthread_cond_t threadDied = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t threadMutex = PTHREAD_MUTEX_INITIALIZER;

static int totThreads = 0;
static int numLive = 0;

static int numUnjoined = 0;

enum tstate
{
    TS_ALIVE,
    TS_TERMINATED,
    TS_JOINED
};

static struct
{
    pthread_t tid;
    enum tstate state;
    int sleepTime;
} *thread;

static void *threadFunc(void *arg)
{
    int id = *((int *) arg);
    int s;

    sleep(thread[id].sleepTime);
    printf("Thread %d sleeptime is %d\n", id, thread[id].sleepTime);
    printf("Thread %d terminating\n", id);

    s = pthread_mutex_lock(&threadMutex);
    if(s != 0)
        errExitEN(s, "pthread_mutex_lock");

    numUnjoined++;
    thread[id].state = TS_TERMINATED;
    free(arg);

    s = pthread_mutex_unlock(&threadMutex);
    if (s != 0)
        errExitEN(s, "pthread_mutex_unlock");
    s = pthread_cond_signal(&threadDied);
    if(s != 0)
        errExitEN(s, "pthread_cond_signal");

    return NULL;
}

int main(int argc, char *argv[])
{
    int s, idx;

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s nsecs...\n", argv[0]);

    thread = calloc(argc - 1, sizeof(*thread));
    if (thread == NULL)
        errExit("calloc");

    for ( idx = 0; idx < argc - 1;)
    {
        int *tmp = malloc(sizeof(int));
        *tmp = idx;
        thread[*tmp].sleepTime = getInt(argv[*tmp + 1], GN_NONEG, NULL);
        printf("Main: Thread %d sleepTime is %d\n", *tmp, thread[*tmp].sleepTime);

        s = pthread_mutex_lock(&threadMutex);
        if (s != 0)
            errExitEN(s, "pthread_mutex_lock");
        
        thread[*tmp].state = TS_ALIVE;
        s = pthread_create(&thread[*tmp].tid, NULL, threadFunc, tmp);
        if (s != 0)
            errExitEN(s, "pthread_create");

        s = pthread_mutex_unlock(&threadMutex);
        if (s != 0)
            errExitEN(s, "pthread_mutex_unlock");
//        sleep(1);
        idx++;
    }

    totThreads = argc - 1;
    numLive = totThreads;

    while(numLive > 0)
    {
        //printf("numLive = %d\n", numLive);
        //printf("numunjoined = %d\n", numUnjoined);
        s = pthread_mutex_lock(&threadMutex);
        if (s != 0)
            errExitEN(s, "pthread_mutex_lock");

        while (numUnjoined == 0)
        {
            printf("Waiting for thread\n");
            s = pthread_cond_wait(&threadDied, &threadMutex);
            if ( s != 0)
                errExitEN(s, "pthread_cond_wait");
            printf("exit waiting\n");
        }

        for ( idx = 0; idx < totThreads; idx++)
        {
    //        printf("thread[%d] state = %d\n", idx, thread[idx].state);
            if (thread[idx].state == TS_TERMINATED)
            {
                printf("Found a terminated thread\n");
                s = pthread_join(thread[idx].tid, NULL);
                if (s != 0)
                    errExitEN(s, "pthread_join");

                thread[idx].state = TS_JOINED;
                numLive--;
                numUnjoined--;

                printf("Reaped thread %d (numLive=%d)\n", idx, numLive);
            }
        }

        s = pthread_mutex_unlock(&threadMutex);
        if (s != 0)
            errExitEN(s, "pthread_mutex_unlock");
    }

    exit(EXIT_SUCCESS);
}

