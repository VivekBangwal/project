#include "ThreadLibrary.h"
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#define SECOND 1
#define STACK_SIZE 4096

static int currentThread = -1;
static int nThreads = 0;
static TCB threadList[MAX_THREADS];
static int ran = 0;

#ifdef __x86_64__
/* code for 64 bit Intel arch */

typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7

address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%fs:0x30,%0\n"
                 "rol    $0x11,%0\n"
                 : "=g" (ret)
                 : "0" (addr));
    return ret;
}

#else
/* code for 32 bit Intel arch */

typedef unsigned int address_t;
#define JB_SP 4
#define JB_PC 5

address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%gs:0x18,%0\n"
                 "rol    $0x9,%0\n"
                 : "=g" (ret)
                 : "0" (addr));
    return ret;
}

#endif


static void wrapper();

void alarm_handler(int sig) {
    signal(SIGALRM, alarm_handler);
    dispatch(SIGALRM);
}

void initThreadLibrary(void) {
    for (int i = 0; i < MAX_THREADS; ++i) {
        threadList[i].status = FINISHED;
        threadList[i].mutex = 0;  // unlocked
        threadList[i].priority = DEFAULT_PRIORITY;
    }
}

int createThread(void (*function)(void), int pri) {
    for (int i = 0; i < MAX_THREADS; ++i) {
        if (threadList[i].status == FINISHED) {
            getcontext(&threadList[i].context);
            threadList[i].context.uc_stack.ss_sp = malloc(STACK_SIZE);
            threadList[i].context.uc_stack.ss_size = STACK_SIZE;
            threadList[i].context.uc_link = 0;
            threadList[i].id = i;
            threadList[i].status = READY;
            threadList[i].priority = pri;
            threadList[i].function = function;

            makecontext(&threadList[i].context, function, 0);

            return i;  // Return thread ID
        }
    }
    return -1;
}

int createThreadWithArg(void *(*function)(void *), void *arg, int pri) {
    for (int i = 0; i < MAX_THREADS; ++i) {
        if (threadList[i].status == FINISHED) {
            getcontext(&threadList[i].context);
            threadList[i].context.uc_stack.ss_sp = malloc(STACK_SIZE);
            threadList[i].context.uc_stack.ss_size = STACK_SIZE;
            threadList[i].context.uc_link = 0;
            threadList[i].id = i;
            threadList[i].status = READY;
            threadList[i].priority = pri;
            threadList[i].argFunction = function;
            threadList[i].arg = arg;

            makecontext(&threadList[i].context, (void (*)(void))function, 1, arg);

            return i;  // Return thread ID
        }
    }

    return -1;  // No available thread slot
}

void yieldThread(void) {
    int highestPriority = INT_MIN;
    int prevThread = currentThread;
    int nextThread = -1;

    // Find the next ready thread with the highest priority
    for (int i = 0; i < MAX_THREADS; ++i) {
        if (i != currentThread && threadList[i].status == READY && threadList[i].priority > highestPriority) {
            highestPriority = threadList[i].priority;
            nextThread = i;
        }
    }

    currentThread = nextThread;

    if (prevThread != -1 && currentThread != -1 && prevThread != currentThread) {
        threadList[prevThread].status = READY;
        swapcontext(&threadList[prevThread].context, &threadList[currentThread].context);
    }
}

void setThreadPriority(int threadID, int priority) {
    if (threadID >= 0 && threadID < MAX_THREADS) {
        threadList[threadID].priority = priority;
    }
}
void runThread(int threadID) {
    threadList[threadID].status = READY;
    yieldThread();
}


void runScheduler(void) {
    while (1) {
        int highestPriority = INT_MIN;
        int nextThread = -1;

        for (int i = 0; i < MAX_THREADS; ++i) {
            if (threadList[i].status == READY && threadList[i].priority > highestPriority) {
                highestPriority = threadList[i].priority;
                nextThread = i;
            }
        }

        if (nextThread != -1) {
            currentThread = nextThread;
            threadList[currentThread].status = RUNNING;

            if (threadList[currentThread].argFunction != NULL) {
                // Thread with arguments
                threadList[currentThread].result = threadList[currentThread].argFunction(threadList[currentThread].arg);
            } else {
                // Simple thread
                threadList[currentThread].function();
            }
            threadList[currentThread].status = FINISHED;
        }
    }
}


void start(void) {
    runScheduler();
}

void suspendThread(int threadID) {
    threadList[threadID].status = BLOCKED;
    yieldThread();
}

void resumeThread(int threadID) {
    threadList[threadID].status = READY;
    yieldThread();
}

void deleteThread(int threadID) {
    free(threadList[threadID].context.uc_stack.ss_sp);
    threadList[threadID].status = FINISHED;
}

void sleepThread(int sec) {
    threadList[currentThread].status = BLOCKED;
    threadList[currentThread].result = NULL;  // Reset result
    sleep(sec);
    threadList[currentThread].status = READY;
    yieldThread();
}

int getID(void) {
    if (currentThread >= 0 && currentThread < MAX_THREADS) {
        return threadList[currentThread].id;
    }
    return -1;  // Invalid ID
}

void cleanThread(void) {
    // ... (unchanged logic for cleanThread)
}

void cleanThread(void) {
    for (int i = 0; i < MAX_THREADS; ++i) {
        if (threadList[i].status != FINISHED) {
            deleteThread(i);
        }
    }
}

void lockMutex(int threadID) {
    if (threadID >= 0 && threadID < MAX_THREADS) {
        while (threadList[threadID].mutex == 1) {
            yieldThread();  // Wait until the mutex is unlocked
        }
        threadList[threadID].mutex = 1;  // Lock the mutex
    }
}

void unlockMutex(int threadID) {
    if (threadID >= 0 && threadID < MAX_THREADS) {
        threadList[threadID].mutex = 0;  // Unlock the mutex
    }
}


