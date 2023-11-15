

This is simple thread library which can create, delete, suspend, resume, run &sleep threads. It can also show the status of any thread.
Threads  can be created with or without function arguments. It simply stores the TCB value of the thread and run it. To run please follow the following commands:-(Note:- only tested on ubuntu).
1) gcc -c MyThread.c
2) gcc -c Yourprogram.c
3) gcc MyThread.o Yourprogram.o -o executable
4) ./executable


Don’t forget to include MyThread.h
-----------------------------------------------------------------------------------------
Here's a brief description of the functions in your ThreadLibrary.h and ThreadLibrary.c files:

In ThreadLibrary.h:
initThreadLibrary:

Description: Initializes the thread library by setting the initial status of all threads, mutexes, and priorities.
Arguments: None.
Return: Void.
createThread:

Description: Creates a thread with a given function and priority.
Arguments:
function: Function pointer to the thread routine.
pri: Priority of the thread.
Return: Integer representing the thread ID.
createThreadWithArg:

Description: Creates a thread with a given function and argument, along with a priority.
Arguments:
function: Function pointer to the thread routine with arguments.
arg: Argument to pass to the thread routine.
pri: Priority of the thread.
Return: Integer representing the thread ID.
yieldThread:

Description: Yields the CPU to the next ready thread with the highest priority.
Arguments: None.
Return: Void.
setThreadPriority:

Description: Sets the priority of a specific thread.
Arguments:
threadID: ID of the thread to set the priority.
priority: Priority to set.
Return: Void.
runThread:

Description: Runs a specific thread.
Arguments:
threadID: ID of the thread to run.
Return: Void.
runScheduler:

Description: Runs the scheduler, managing the execution of threads based on priority.
Arguments: None.
Return: Void.
start:

Description: Starts the execution of the scheduler.
Arguments: None.
Return: Void.
suspendThread:

Description: Suspends a specific thread.
Arguments:
threadID: ID of the thread to suspend.
Return: Void.
resumeThread:

Description: Resumes a specific thread.
Arguments:
threadID: ID of the thread to resume.
Return: Void.
In ThreadLibrary.c:
deleteThread:

Description: Deletes (frees resources) a specific thread.
Arguments:
threadID: ID of the thread to delete.
Return: Void.
sleepThread:

Description: Blocks the current thread for a specified number of seconds.
Arguments:
sec: Number of seconds to sleep.
Return: Void.
getID:

Description: Gets the ID of the current running thread.
Arguments: None.
Return: Integer representing the current thread ID.
cleanThread:

Description: Cleans up (deletes) all threads that have finished their execution.
Arguments: None.
Return: Void.
joinThread:

Description: Blocks the current thread until the specified thread finishes execution.
Arguments:
threadID: ID of the thread to join.
Return: Void.
lockMutex:

Description: Locks the mutex associated with a specific thread.
Arguments:
threadID: ID of the thread whose mutex needs to be locked.
Return: Void.
unlockMutex:

Description: Unlocks the mutex associated with a specific thread.
Arguments:
threadID: ID of the thread whose mutex needs to be unlocked.
Return: Void.
