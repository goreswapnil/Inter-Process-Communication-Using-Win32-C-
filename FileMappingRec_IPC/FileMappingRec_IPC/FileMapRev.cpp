#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include"TracerFM.h"
#pragma comment(lib, "user32.lib")

Tracer g_Tracer;

#define BUF_SIZE 256
TCHAR szName[] = TEXT("Global\\MyFileMappingObject");

int _tmain()
{
    g_Tracer.Trace(L"~myfmipc~: In Receiver Main()");
    HANDLE hMapFile;
    LPCTSTR pBuf;

    hMapFile = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,   // read/write access
        FALSE,                 // do not inherit the name
        szName);               // name of mapping object

    if (hMapFile == NULL)
    {
        g_Tracer.Trace(L"~myfmipc~: OpenFileMapping() Failed Error:%d", GetLastError());
        _tprintf(TEXT("Could not open file mapping object (%d).\n"),
            GetLastError());
        return 1;
    }
    g_Tracer.Trace(L"~myfmipc~: OpenFileMapping() Succeeded.");

    pBuf = (LPTSTR)MapViewOfFile(   //Maps a view of a file mapping into the address space of a calling process.
        hMapFile,                   // handle to map object
        FILE_MAP_ALL_ACCESS,        // read/write permission
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

    wprintf(pBuf);
    g_Tracer.Trace(L"~myfmipc~: Message get from sender Succeeded.");


    UnmapViewOfFile(pBuf);

    CloseHandle(hMapFile);
    g_Tracer.Trace(L"~myfmipc~: Out Receiver Main()");

    return 0;
}