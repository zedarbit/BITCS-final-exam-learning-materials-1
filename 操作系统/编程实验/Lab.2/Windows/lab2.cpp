#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <iostream>

using namespace std;

DWORD WINAPI ThreadFun(LPVOID pM)
{
    printf("-[INFO] Thread ID: %d\n-[INFO] Thread output: Hello World\n", GetCurrentThreadId());
    Sleep(200);
    return 0;
}

int main(int argc, char *argv[])
{
    SYSTEMTIME systime;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    GetSystemTime(&systime); //系统时间
    cout << "-[TIME] Current time: ";
    cout << systime.wHour << ":" << systime.wMinute << ":" << systime.wSecond << ":" << systime.wMilliseconds << endl;

    DWORD t_start, t_end;
    t_start = GetTickCount();
    cout << "-[INFO] Thread start time: " << t_start << " ms" << endl;

    bool handle = CreateProcess(NULL, argv[1], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    WaitForSingleObject(pi.hProcess, INFINITE);

    t_end = GetTickCount();
    cout << "-[INFO] Thread finish time: " << t_end << " ms" << endl;
    cout << "-[INFO] Thread consume time: " << t_end - t_start << " ms" << endl;

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}