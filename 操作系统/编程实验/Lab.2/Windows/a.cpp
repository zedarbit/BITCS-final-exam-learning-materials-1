#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <shellapi.h>

int main(void)
{
    HINSTANCE hNewExe = ShellExecuteA(NULL, "open", "d:\\tese.log", NULL, NULL, SW_SHOW);
    if ((DWORD)hNewExe <= 32)
    {
        printf("return value:%d\n", (DWORD)hNewExe);
    }
    else
    {
        printf("successed!\n");
    }
    printf("GetLastError: %d\n", GetLastError());
    system("pause");
    return 1;
}