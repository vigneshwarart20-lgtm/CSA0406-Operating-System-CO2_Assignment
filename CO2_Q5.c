
#include <stdio.h>
#include <windows.h>

#define BUFFER_SIZE 5
#define ITEMS 20

int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;

int produced = 0;
int consumed = 0;
int total = 0;

HANDLE emptySlots;
HANDLE fullSlots;
CRITICAL_SECTION mutex;

DWORD WINAPI Producer(LPVOID parameter)
{
    int i;
    int item;

    for (i = 1; i <= ITEMS; i++)
    {
        item = i;

        WaitForSingleObject(emptySlots, INFINITE);
        EnterCriticalSection(&mutex);

        buffer[in] = item;
        in = (in + 1) % BUFFER_SIZE;
        produced++;

        printf("Producer produced: %d\n", item);

        LeaveCriticalSection(&mutex);
        ReleaseSemaphore(fullSlots, 1, NULL);
    }

    return 0;
}

DWORD WINAPI Consumer(LPVOID parameter)
{
    int item;

    while (1)
    {
        WaitForSingleObject(fullSlots, INFINITE);
        EnterCriticalSection(&mutex);

        item = buffer[out];
        out = (out + 1) % BUFFER_SIZE;

        consumed++;
        total += item;

        printf("Consumer consumed: %d\n", item);

        LeaveCriticalSection(&mutex);
        ReleaseSemaphore(emptySlots, 1, NULL);

        if (consumed == ITEMS)
            break;
    }

    return 0;
}

int main(void)
{
    HANDLE producerThread;
    HANDLE consumerThread;

    printf("PRODUCER-CONSUMER SYNCHRONISATION\n");
    printf("---------------------------------\n");

    emptySlots = CreateSemaphore(
        NULL, BUFFER_SIZE, BUFFER_SIZE, NULL);

    fullSlots = CreateSemaphore(
        NULL, 0, BUFFER_SIZE, NULL);

    if (emptySlots == NULL || fullSlots == NULL)
    {
        printf("Semaphore creation failed.\n");
        return 1;
    }

    InitializeCriticalSection(&mutex);

    producerThread = CreateThread(
        NULL, 0, Producer, NULL, 0, NULL);

    consumerThread = CreateThread(
        NULL, 0, Consumer, NULL, 0, NULL);

    if (producerThread == NULL || consumerThread == NULL)
    {
        printf("Thread creation failed.\n");
        return 1;
    }

    WaitForSingleObject(producerThread, INFINITE);
    WaitForSingleObject(consumerThread, INFINITE);

    printf("\nSynchronization Results\n");
    printf("Items produced: %d\n", produced);
    printf("Items consumed: %d\n", consumed);
    printf("Sum of consumed items: %d\n", total);
    printf("Expected sum: 210\n");

    if (total == 210 && consumed == ITEMS)
        printf("Producer-Consumer synchronization successful.\n");

    CloseHandle(producerThread);
    CloseHandle(consumerThread);

    CloseHandle(emptySlots);
    CloseHandle(fullSlots);

    DeleteCriticalSection(&mutex);

    return 0;
}

