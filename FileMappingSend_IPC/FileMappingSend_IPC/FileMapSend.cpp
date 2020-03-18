#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include"TracerFM.h"

Tracer g_Tracer;

#define BUF_SIZE 256
TCHAR szName[] = TEXT("Global\\MyFileMappingObject");
TCHAR szMsg[] = TEXT("Message from first process.");

int _tmain()
{
    g_Tracer.Trace(L"~myfmipc~: In Sender Main()");
    HANDLE hMapFile;
    LPCTSTR pBuf;
   
    HANDLE hTxtFile = CreateFile(L"myfilemaping.txt", GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, NULL, NULL);
    g_Tracer.Trace(L"~myfmipc~: In Main() CreateFile succeeded.");
    hMapFile = CreateFileMapping(
        hTxtFile,                // use paging file
        NULL,                    // default security
        PAGE_READWRITE,          // read/write access
        0,                       // maximum object size (high-order DWORD)
        BUF_SIZE,                // maximum object size (low-order DWORD)
        szName);                 // name of mapping object

    if (hMapFile ==FALSE)
    {
        g_Tracer.Trace(L"~myfmipc~: CreateFileMapping() Failed Error:%d", GetLastError());
        _tprintf(TEXT("Could not create file mapping object (%d).\n"),
            GetLastError());
        return 1;
    }
    g_Tracer.Trace(L"~myfmipc~: CreateFileMapping() Succeeded.");
    pBuf = (LPTSTR)MapViewOfFile(       //Maps a view of a file mapping into the address space of a calling process.
        hMapFile,                       // handle to map object
        FILE_MAP_ALL_ACCESS,            // read/write permission
        0,
        0,
        BUF_SIZE);

    if (pBuf == NULL)
    {
        g_Tracer.Trace(L"~myfmipc~: MapViewOfFile() Failed Error:%d", GetLastError());
        _tprintf(TEXT("Could not map view of file (%d).\n"),
            GetLastError());

        CloseHandle(hMapFile);

        return 1;
    }
    g_Tracer.Trace(L"~myfmipc~: MapViewOfFile() Succeeded.");

    
    CopyMemory((PVOID)pBuf, szMsg, (_tcslen(szMsg) * sizeof(TCHAR)));
    g_Tracer.Trace(L"~myfmipc~: Message Send from first process");
    wprintf(L"send msg from 1st process");
    _getch();

    UnmapViewOfFile(pBuf);

    CloseHandle(hMapFile);

    g_Tracer.Trace(L"~myfmipc~: Out Sender Main()");
    return 0;
}