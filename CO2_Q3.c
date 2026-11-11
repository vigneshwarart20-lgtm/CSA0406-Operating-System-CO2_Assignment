
#include <stdio.h>
#include <windows.h>

#define CAMERAS 64
#define THREADS 4
#define WORK 500000

volatile LONG nextCamera = -1;
volatile LONG processed = 0;

DWORD WINAPI CameraWorker(LPVOID parameter)
{
    LONG threadNumber;
    LONG camera;
    int j;
    volatile double calculation;

    threadNumber = (LONG)(LONG_PTR)parameter;

    while (1)
    {
        camera = InterlockedIncrement(&nextCamera);

        if (camera >= CAMERAS)
            break;

        calculation = 0;

        for (j = 0; j < WORK; j++)
        {
            calculation += (j % 100) * 0.01;
        }

        InterlockedIncrement(&processed);

        printf("Thread %ld processed Camera %ld\n",
               threadNumber, camera + 1);
    }

    return 0;
}

int main(void)
{
    HANDLE threads[THREADS];
    int i;

    printf("CAMERA MULTITHREADING SIMULATION\n");
    printf("--------------------------------\n");

    for (i = 0; i < THREADS; i++)
    {
        threads[i] = CreateThread(
            NULL,
            0,
            CameraWorker,
            (LPVOID)(LONG_PTR)(i + 1),
            0,
            NULL
        );

        if (threads[i] == NULL)
        {
            printf("Thread creation failed.\n");
            return 1;
        }
    }

    WaitForMultipleObjects(THREADS, threads, TRUE, INFINITE);

    for (i = 0; i < THREADS; i++)
    {
        CloseHandle(threads[i]);
    }

    printf("\nTotal cameras processed: %ld\n", processed);
    printf("Number of threads used: %d\n", THREADS);

    if (processed == CAMERAS)
        printf("All camera tasks completed successfully.\n");

    return 0;
}

