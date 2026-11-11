
#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include <string.h>

DWORD GetParentProcessId(void)
{
    PROCESSENTRY32 entry;
    HANDLE snapshot;
    DWORD parentPID;
    DWORD currentPID;

    entry.dwSize = sizeof(PROCESSENTRY32);
    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshot == INVALID_HANDLE_VALUE)
        return 0;

    parentPID = 0;
    currentPID = GetCurrentProcessId();

    if (Process32First(snapshot, &entry))
    {
        do
        {
            if (entry.th32ProcessID == currentPID)
            {
                parentPID = entry.th32ParentProcessID;
                break;
            }
        }
        while (Process32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return parentPID;
}

int main(int argc, char *argv[])
{
    int i;
    STARTUPINFOA si[3];
    PROCESS_INFORMATION pi[3];
    HANDLE handles[3];
    char exePath[MAX_PATH];
    char command[MAX_PATH + 20];

    if (argc > 1 && strcmp(argv[1], "child") == 0)
    {
        printf("Child Process Started\n");
        printf("Child PID: %lu\n", GetCurrentProcessId());
        printf("Parent PID: %lu\n", GetParentProcessId());

        Sleep(5000);

        printf("Child Process Completed\n");
        return 0;
    }

    printf("Parent Process Started\n");
    printf("Parent PID: %lu\n\n", GetCurrentProcessId());

    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    sprintf(command, "\"%s\" child", exePath);

    for (i = 0; i < 3; i++)
    {
        ZeroMemory(&si[i], sizeof(si[i]));
        ZeroMemory(&pi[i], sizeof(pi[i]));

        si[i].cb = sizeof(si[i]);

        if (!CreateProcessA(
                NULL,
                command,
                NULL,
                NULL,
                FALSE,
                CREATE_NEW_CONSOLE,
                NULL,
                NULL,
                &si[i],
                &pi[i]))
        {
            printf("Failed to create child %d. Error: %lu\n",
                   i + 1, GetLastError());
            return 1;
        }

        handles[i] = pi[i].hProcess;

        printf("Created Child %d | PID: %lu\n",
               i + 1, pi[i].dwProcessId);
    }

    printf("\nParent waiting for child processes...\n");

    WaitForMultipleObjects(3, handles, TRUE, INFINITE);

    printf("\nAll child processes completed.\n");

    for (i = 0; i < 3; i++)
    {
        CloseHandle(pi[i].hProcess);
        CloseHandle(pi[i].hThread);
    }

    printf("Parent Process Terminated.\n");

    return 0;
}

