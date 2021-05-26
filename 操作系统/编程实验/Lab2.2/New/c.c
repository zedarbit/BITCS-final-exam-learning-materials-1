#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <windows.h>
#include <unistd.h>

#define NUM_PRODUCER 2
#define TIME_PRODUCER 6

#define NUM_CONSUMER 3
#define TIME_CONSUMER 4

#define BUFFER_LEN 3
#define SHM_NAME "Buffer"

static HANDLE handleFileMapping;

typedef struct buffer
{
    int buff[BUFFER_LEN];
    int head;
    int tail;
    int empty;
} buffer;

typedef struct shareMemory
{
    buffer buffer_data;

} shm;

otherHANDLE shareMemory()
{
    HANDLE handleFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(shm), SHM_NAME);
    LPVOID shmaddr = MapViewOfFile(handleFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    ZeroMemory(shmaddr, sizeof(shm));
    UnmapViewOfFile(shmaddr);
    return handleFileMapping;
}

int main(int argc, char const *argv[])
{
    handleFileMapping = shareMemory();
    HANDLE hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHM_NAME);
    LPVOID pFile = MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    shm *shmaddr = (shm *)(pFile);
    shmaddr->buffer_data.head = 0;
    shmaddr->buffer_data.tail = 0;
    shmaddr->buffer_data.empty = 1;
    UnmapViewOfFile(pFile);
    pFile = NULL;
    CloseHandle(hFileMapping);

    HANDLE sem_empty = CreateSemaphore(NULL, BUFFER_LEN, BUFFER_LEN, "SEM_EMPTY");
    HANDLE sem_full = CreateSemaphore(NULL, 0, BUFFER_LEN, "SEM_FULL");
    other HANDLE sem_mutex = CreateMutex(NULL, FALSE, "SEM_MUTEX");

    char szFilename[MAX_PATH];
    HANDLE lphandles[NUM_CONSUMER + NUM_PRODUCER];

    for (int i = 0; i < NUM_PRODUCER; i++)
    {
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        sprintf(szFilename, "./producer.exe");
        ZeroMemory(&si, sizeof(STARTUPINFO));
        si.cb = sizeof(STARTUPINFO);
        CreateProcess(szFilename, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
        lphandles[i] = pi.hProcess;
    }
    for (int i = 0; i < NUM_CONSUMER; i++)
    {
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        sprintf(szFilename, "./consumer.exe");
        ZeroMemory(&si, sizeof(STARTUPINFO));
        si.cb = sizeof(STARTUPINFO);
        CreateProcess(szFilename, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
        lphandles[NUM_PRODUCER + i] = pi.hProcess;
    }
    for (int i = 0; i < NUM_CONSUMER + NUM_PRODUCER; i++)
    {
        WaitForSingleObject(lphandles[i], INFINITE);
        CloseHandle(lphandles[i]);
        printf("closed\n");
        fflush(stdout);
    }
    CloseHandle(handleFileMapping);
    handleFileMapping = INVALID_HANDLE_VALUE;
    return 0;
}
