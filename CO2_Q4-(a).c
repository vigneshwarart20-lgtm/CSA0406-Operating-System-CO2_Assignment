
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>

int main(int argc, char *argv[])
{
    HANDLE readPipe;
    HANDLE writePipe;
    HANDLE processHandle;
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES sa;
    char exePath[MAX_PATH];
    char command[MAX_PATH + 20];

    int readings[6] = {31, 33, 36, 34, 41, 38};

    if (argc > 1 && strcmp(argv[1], "child") == 0)
    {
        int values[6];
        DWORD bytesRead;
        int total = 0;
        int maximum = 0;
        int i;

        readPipe = GetStdHandle(STD_INPUT_HANDLE);

        if (!ReadFile(readPipe, values, sizeof(values),
                      &bytesRead, NULL))
        {
            printf("Pipe reading failed.\n");
            return 1;
        }

        if (bytesRead != sizeof(values))
        {
            printf("Incomplete sensor data received.\n");
            return 1;
        }

        for (i = 0; i < 6; i++)
        {
            total += values[i];

            if (values[i] > maximum)
                maximum = values[i];
        }

        printf("Child Process - Sensor Analysis\n");
        printf("Number of readings: 6\n");
        printf("Average: %.2f\n", total / 6.0);
        printf("Maximum reading: %d\n", maximum);

        return 0;
    }

    ZeroMemory(&sa, sizeof(sa));
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;

    if (!CreatePipe(&readPipe, &writePipe, &sa, 0))
    {
        printf("Pipe creation failed.\n");
        return 1;
    }

    SetHandleInformation(writePipe, HANDLE_FLAG_INHERIT, 0);

    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    sprintf(command, "\"%s\" child", exePath);

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = readPipe;
    si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

    if (!CreateProcessA(
            NULL, command, NULL, NULL, TRUE,
            0, NULL, NULL, &si, &pi))
    {
        printf("Child process creation failed. Error: %lu\n",
               GetLastError());

        CloseHandle(readPipe);
        CloseHandle(writePipe);
        return 1;
    }

    CloseHandle(readPipe);

    printf("Parent Process: Sending sensor readings.\n");

    {
        DWORD bytesWritten;

        if (!WriteFile(writePipe, readings, sizeof(readings),
                       &bytesWritten, NULL))
        {
            printf("Pipe writing failed.\n");
        }
        else
        {
            printf("Sensor readings sent successfully.\n");
        }
    }

    CloseHandle(writePipe);

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    printf("IPC communication completed.\n");

    return 0;
}

