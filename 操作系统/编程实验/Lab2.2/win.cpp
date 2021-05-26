#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define TIME_PRODUCER 6
#define TIME_CONSUMER 4

struct buffer //定义缓冲区
{
    int s[3];
    int head;
    int tail;
    int is_empty;
};

struct sharedmemory //定义共享内存
{
    struct buffer data;
    HANDLE full;
    HANDLE empty;
    HANDLE mutex;
};

//创建克隆进程
PROCESS_INFORMATION StartClone(int id)
{
    printf("-[INFO] Process %d created.\n");
    TCHAR szFilename[MAX_PATH];
    TCHAR szCmdLine[MAX_PATH];
    PROCESS_INFORMATION pi;
    GetModuleFileName(NULL, szFilename, MAX_PATH);
    sprintf(szCmdLine, "\"%s\" %d", szFilename, id);
    STARTUPINFO si;
    ZeroMemory(reinterpret_cast<void *>(&si), sizeof(si));
    si.cb = sizeof(si);
    //创建子进程
    BOOL bCreatOK = CreateProcess(szFilename, szCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    return pi;
}

//显示缓冲区数据
void CurrentStatus(struct sharedmemory *a)
{
    printf("Current Data: ");
    for (int i = a->data.head;;)
    {
        printf("%d ", a->data.s[i]);
        i++;
        i %= 3;
        if (i == a->data.tail)
        {
            printf("\n");
            return;
        }
    }
}

HANDLE MakeShared() //创建共享内存
{
    //创建一个临时文件映射对象
    HANDLE hMapping = CreateFileMapping(INVALID_HANDLE_VALUE,
                                        NULL, PAGE_READWRITE, 0, sizeof(struct sharedmemory), "BUFFER");
    if (hMapping == NULL)
    {
        printf("CreateFileMapping error!\n");
        exit(0);
    }
    //在文件映射上创建视图，返回起始虚地址
    LPVOID pData = MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (pData == NULL)
    {
        printf("MapViewOfFile error!\n");
        exit(0);
    }
    if (pData != NULL)
    {
        ZeroMemory(pData, sizeof(struct sharedmemory));
    }
    UnmapViewOfFile(pData);
    return (hMapping);
}

int getrandomnumber()
{
    int temp = 1000;
    int rn = rand();
    return rn % temp;
}

int main(int argc, char *argv[])
{
    srand(GetCurrentProcessId());
    int subid = 0;
    HANDLE hMapping;
    if (argc > 1)
    {
        //如果有参数就作为子进程id
        sscanf(argv[1], "%d", &subid);
    }
    if (subid == 0) //主进程
    {
        hMapping = MakeShared();
        //打开文件映射
        HANDLE hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "BUFFER");
        if (hFileMapping == NULL)
        {
            printf("OpenFileMapping error!\n");
            exit(0);
        }

        LPVOID pFile = MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        if (pFile == NULL)
        {
            printf("MapViewOfFile error!\n");
            exit(0);
        }

        // 创建共享内存
        struct sharedmemory *addr = (struct sharedmemory *)(pFile);
        addr->data.head = 0;
        addr->data.tail = 0;
        addr->data.is_empty = 1;
        HANDLE empty = CreateSemaphore(NULL, 3, 3, "EMPTY");
        HANDLE full = CreateSemaphore(NULL, 0, 3, "FULL");
        HANDLE mutex = CreateMutex(NULL, FALSE, "MUTEX");
        UnmapViewOfFile(pFile);
        pFile = NULL;
        CloseHandle(hFileMapping);
        //创建子进程
        PROCESS_INFORMATION sub[5];
        for (int i = 0; i < 5; i++)
            sub[i] = StartClone(++subid);
        //等待子进程结束
        for (int i = 0; i < 5; i++)
            WaitForSingleObject(sub[i].hProcess, INFINITE);
        //关闭子进程句柄
        for (int i = 0; i < 5; i++)
            CloseHandle(sub[i].hProcess);
    }
    else if (subid == 1 || subid == 2) //生产者
    {
        HANDLE hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "BUFFER");
        if (hMap == NULL)
        {
            printf("OpenFileMapping error!\n");
            exit(0);
        }
        LPVOID pFile = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        if (pFile == NULL)
        {
            printf("MapViewOfFile error!\n");
            exit(0);
        }
        struct sharedmemory *addr = (struct sharedmemory *)(pFile);
        HANDLE full = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "FULL"); // 为现有的一个已命名信号机对象创建一个新句柄
        HANDLE empty = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "EMPTY");
        HANDLE mutex = OpenMutex(SEMAPHORE_ALL_ACCESS, FALSE, "MUTEX"); // 为现有的一个已命名互斥体对象创建一个新句柄。
        for (int i = 0; i < TIME_PRODUCER; i++)
        {
            Sleep(getrandomnumber());
            WaitForSingleObject(empty, INFINITE); //P(empty) 申请空缓冲
            WaitForSingleObject(mutex, INFINITE); //P(mutex) 申请进入缓冲区
            //向缓冲区添加数据
            int num = getrandomnumber();
            addr->data.s[addr->data.tail] = num;
            addr->data.tail = (addr->data.tail + 1) % 3;
            addr->data.is_empty = 0;
            SYSTEMTIME time;
            GetLocalTime(&time);
            printf("\nTime: %02d:%02d:%02d\n", time.wHour, time.wMinute, time.wSecond);
            printf("Producer %d putting %d\n", subid - 1, num);

            if (addr->data.is_empty)
                printf("Empty!\n");
            else
                CurrentStatus(addr);

            ReleaseSemaphore(full, 1, NULL); //V(full) 释放一个产品
            ReleaseMutex(mutex);             //V(mutex) 退出缓冲区
        }
        UnmapViewOfFile(pFile); // 停止当前程序的一个内存映射
        pFile = NULL;
        CloseHandle(hMap); // 关闭句柄
        CloseHandle(mutex);
        CloseHandle(empty);
        CloseHandle(full);
    }
    else if (subid == 3 || subid == 4 || subid == 5) //消费者
    {
        HANDLE hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "BUFFER");
        if (hMap == NULL)
        {
            printf("OpenFileMapping error!\n");
            exit(0);
        }
        LPVOID pFile = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        if (pFile == NULL)
        {
            printf("MapViewOfFile error!\n");
            exit(0);
        }
        struct sharedmemory *addr = (struct sharedmemory *)(pFile);
        HANDLE full = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "FULL");
        HANDLE empty = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "EMPTY");
        HANDLE mutex = OpenMutex(SEMAPHORE_ALL_ACCESS, FALSE, "MUTEX");
        for (int i = 0; i < TIME_CONSUMER; i++)
        {
            Sleep(getrandomnumber());
            WaitForSingleObject(full, INFINITE);  //P(full) 申请一个产品
            WaitForSingleObject(mutex, INFINITE); //P(mutex) 申请进入缓冲区

            int num = addr->data.s[addr->data.head];
            addr->data.head = (addr->data.head + 1) % 3;
            if (addr->data.head == addr->data.tail)
                addr->data.is_empty = 1;
            else
                addr->data.is_empty = 0;
            SYSTEMTIME time;
            GetLocalTime(&time);
            printf("\nTime: %02d:%02d:%02d\n", time.wHour, time.wMinute, time.wSecond);
            printf("Consumer %d removing %d\n", subid, num);

            if (addr->data.is_empty)
                printf("Empty!\n");
            else
                CurrentStatus(addr);

            ReleaseSemaphore(empty, 1, NULL); //V(empty) 释放一个空缓冲
            ReleaseMutex(mutex);              //V(mutex) 退出缓冲区
        }
        UnmapViewOfFile(pFile);
        pFile = NULL;
        CloseHandle(hMap);
    }
    CloseHandle(hMapping);
    hMapping = INVALID_HANDLE_VALUE;
    return 0;
}
