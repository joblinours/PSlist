#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <time.h>

void DisplayHelp() {
    printf("Usage: plist.exe [options]\n");
    printf("Options:\n");
    printf("  -h             Display this help message\n");
    printf("  [ProcessName]  Display information for the specified process\n");
    printf("  -d [PID]       Display detailed information for the threads of the process with the specified PID\n");
}

void FormatCPUTime(FILETIME ftKernel, FILETIME ftUser, char *buffer, size_t size) {
    ULARGE_INTEGER kernelTime, userTime;
    kernelTime.LowPart = ftKernel.dwLowDateTime;
    kernelTime.HighPart = ftKernel.dwHighDateTime;

    userTime.LowPart = ftUser.dwLowDateTime;
    userTime.HighPart = ftUser.dwHighDateTime;

    double totalSeconds = (kernelTime.QuadPart + userTime.QuadPart) / 10000000.0;

    int hours = (int)(totalSeconds / 3600);
    totalSeconds -= hours * 3600;
    int minutes = (int)(totalSeconds / 60);
    totalSeconds -= minutes * 60;

    snprintf(buffer, size, "%d:%02d:%06.3f", hours, minutes, totalSeconds);
}

void FormatElapsedTime(FILETIME ftCreation, char *buffer, size_t size) {
    FILETIME currentTimeFile;
    GetSystemTimeAsFileTime(&currentTimeFile);

    ULARGE_INTEGER uCreationTime, uCurrentTime, uElapsedTime;
    uCreationTime.LowPart = ftCreation.dwLowDateTime;
    uCreationTime.HighPart = ftCreation.dwHighDateTime;

    uCurrentTime.LowPart = currentTimeFile.dwLowDateTime;
    uCurrentTime.HighPart = currentTimeFile.dwHighDateTime;

    uElapsedTime.QuadPart = uCurrentTime.QuadPart - uCreationTime.QuadPart;

    unsigned long long elapsedMillis = uElapsedTime.QuadPart / 10000;
    unsigned long elapsedSeconds = (unsigned long)(elapsedMillis / 1000);
    unsigned long elapsedMinutes = elapsedSeconds / 60;
    unsigned long elapsedHours = elapsedMinutes / 60;

    snprintf(buffer, size, "%lu:%02lu:%02lu.%03llu",
             elapsedHours,
             elapsedMinutes % 60,
             elapsedSeconds % 60,
             elapsedMillis % 1000);
}

void ListThreads(DWORD processID) {
    THREADENTRY32 threadEntry = { sizeof(THREADENTRY32) };
    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap == INVALID_HANDLE_VALUE) {
        printf("\tFailed to create thread snapshot.\n");
        return;
    }

    printf("-------------------------------------------------------------------------------\n\n");
    printf("--------------------------------------------------------------------------\n");
    printf("Tid   Pri   Cswtch   State           User Time   Kernel Time   Elapsed Time\n");
    printf("--------------------------------------------------------------------------\n");

    if (Thread32First(hThreadSnap, &threadEntry)) {
        do {
            if (threadEntry.th32OwnerProcessID == processID) {
                printf("%5lu %5d %8d   Wait:Unknown     0:00:00      0:00:00      0:00:00\n",
                       threadEntry.th32ThreadID, threadEntry.tpBasePri, 0);
            }
        } while (Thread32Next(hThreadSnap, &threadEntry));
    }
    CloseHandle(hThreadSnap);
}

void ListProcesses(const char *filterName, DWORD detailedPID) {
    PROCESSENTRY32 processEntry = { sizeof(PROCESSENTRY32) };
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        printf("Failed to create process snapshot. Exiting...\n");
        return;
    }

    printf("Name                Pid   Pri  Thd   Hnd   Priv      CPU Time    Elapsed Time\n");
    printf("-------------------------------------------------------------------------------\n");

    if (Process32First(hProcessSnap, &processEntry)) {
        do {
            if (filterName && _stricmp(processEntry.szExeFile, filterName) != 0) {
                continue;
            }

            if (detailedPID != 0 && processEntry.th32ProcessID != detailedPID) {
                continue;
            }

            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processEntry.th32ProcessID);
            if (hProcess) {
                FILETIME ftCreation, ftExit, ftKernel, ftUser;
                if (GetProcessTimes(hProcess, &ftCreation, &ftExit, &ftKernel, &ftUser)) {
                    char elapsedTime[20];
                    char cpuTime[20];
                    FormatElapsedTime(ftCreation, elapsedTime, sizeof(elapsedTime));
                    FormatCPUTime(ftKernel, ftUser, cpuTime, sizeof(cpuTime));

                    printf("%-20s %-6lu %-4lu %-4lu %-5lu %-8lu  %s      %s\n",
                           processEntry.szExeFile,
                           processEntry.th32ProcessID,
                           processEntry.pcPriClassBase,
                           processEntry.cntThreads,
                           processEntry.cntUsage,
                           processEntry.dwSize,
                           cpuTime,
                           elapsedTime);
                }
                CloseHandle(hProcess);
            }

            if (detailedPID != 0) {
                ListThreads(processEntry.th32ProcessID);
            }
        } while (Process32Next(hProcessSnap, &processEntry));
    }
    CloseHandle(hProcessSnap);
}

int main(int argc, char *argv[]) {
    if (argc == 2 && strcmp(argv[1], "-h") == 0) {
        DisplayHelp();
        return 0;
    }

    DWORD detailedPID = 0;
    const char *filterName = NULL;

    if (argc == 3 && strcmp(argv[1], "-d") == 0) {
        detailedPID = atoi(argv[2]);
    } else if (argc == 2) {
        filterName = argv[1];
    }

    if (argc == 1) {
        printf("Listing all processes:\n");
    }

    printf("-------------------------------------------------------------------------------\n");
    ListProcesses(filterName, detailedPID);

    return 0;
}
