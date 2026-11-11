
#include <stdio.h>
#include <windows.h>
#include <string.h>

#define SHARED_NAME "Local\\CSA0406_SensorMemory"

typedef struct
{
    int readings[6];
    int count;
} SharedData;

int main(int argc, char *argv[])
{
    HANDLE mapping;
    SharedData *data;
    int i;

    if (argc > 1 && strcmp(argv[1], "child") == 0)
    {
        mapping = OpenFileMappingA(
            FILE_MAP_READ, FALSE, SHARED_NAME);

        if (mapping == NULL)
        {
            printf("Unable to open shared memory.\n");
            return 1;
        }

        data = (SharedData *)MapViewOfFile(
            mapping, FILE_MAP_READ, 0, 0, sizeof(SharedData));

        if (data == NULL)
        {
            printf("Unable to map shared memory.\n");
            CloseHandle(mapping);
            return 1;
        }

        {
            int total = 0;
            int maximum = 0;

            for (i = 0; i < data->count; i++)
            {
                total += data->readings[i];

                if (data->readings[i] > maximum)
                    maximum = data->readings[i];
            }

            printf("Child Process - Shared Memory Analysis\n");
            printf("Number of readings: %d\n", data->count);
            printf("Average: %.2f\n",
                   total / (double)data->count);
            printf("Maximum reading: %d\n", maximum);
        }

        UnmapViewOfFile(data);
        CloseHandle(mapping);

        return 0;
    }

    mapping = CreateFileMappingA(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        sizeof(SharedData),
        SHARED_NAME
    );

    if (mapping == NULL)
    {
        printf("Shared memory creation failed.\n");
        return 1;
    }

    data = (SharedData *)MapViewOfFile(
        mapping,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        sizeof(SharedData)
    );

    if (data == NULL)
    {
        printf("Unable to map shared memory.\n");
        CloseHandle(mapping);
        return 1;
    }

    {
        int values[6] = {31, 33, 36, 34, 41, 38};

        data->count = 6;

        for (i = 0; i < 6; i++)
            data->readings[i] = values[i];
    }

    printf("Parent Process: Sensor data stored in shared memory.\n");

    {
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        char exePath[MAX_PATH];
        char command[MAX_PATH + 20];

        GetModuleFileNameA(NULL, exePath, MAX_PATH);
        sprintf(command, "\"%s\" child", exePath);

        ZeroMemory(&si, sizeof(si));
        ZeroMemory(&pi, sizeof(pi));
        si.cb = sizeof(si);

        if (!CreateProcessA(
                NULL, command, NULL, NULL, FALSE,
                0, NULL, NULL, &si, &pi))
        {
            printf("Child process creation failed.\n");
            UnmapViewOfFile(data);
            CloseHandle(mapping);
            return 1;
        }

        WaitForSingleObject(pi.hProcess, INFINITE);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    UnmapViewOfFile(data);
    CloseHandle(mapping);

    printf("Shared memory communication completed.\n");

    return 0;
}

