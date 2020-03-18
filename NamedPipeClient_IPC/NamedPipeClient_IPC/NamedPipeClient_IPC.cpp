//Named Pipe Client

#include <windows.h> 
#include <stdio.h> 
#include <tchar.h>
#include <strsafe.h>
#include"TracerNP.h"

#define BUFSIZE 512

Tracer g_Tracer;

int wmain(int argc, TCHAR* argv[])
{
    g_Tracer.Trace(L"~mynpc~: In wmain()");
    HANDLE hPipe;
    LPTSTR lpvMessage = LPTSTR(TEXT("Default message from client."));
    TCHAR  chBuf[BUFSIZE];
    BOOL   fSuccess = FALSE;
    DWORD  cbRead, cbToWrite, cbWritten, dwMode;
    LPTSTR lpszPipename = LPTSTR(TEXT("\\\\.\\pipe\\mynamedpipe"));

    if (argc > 1)
        lpvMessage = argv[1];

    // Try to open a named pipe; wait for it, if necessary. 
    g_Tracer.Trace(L"~mynpc~: Try to open a Named Pipe.");

    while (1)
    {
        hPipe = CreateFile(
            lpszPipename,   // pipe name 
            GENERIC_READ |  // read and write access 
            GENERIC_WRITE,
            0,              // no sharing 
            NULL,           // default security attributes
            OPEN_EXISTING,  // opens existing pipe 
            0,              // default attributes 
            NULL);          // no template file 

      // Break if the pipe handle is valid. 

        if (hPipe != INVALID_HANDLE_VALUE) {
            g_Tracer.Trace(L"~mynpc~: CreateFile() : Open pipe Succeeded.");
            break;
        }

        // Exit if an error other than ERROR_PIPE_BUSY occurs. 

        if (GetLastError() != ERROR_PIPE_BUSY)
        {
            g_Tracer.Trace(L"~mynpc~: CreateFile() : Could not open pipe. GLE=%d", GetLastError());
            _tprintf(TEXT("Could not open pipe. GLE=%d\n"), GetLastError());
            return -1;
        }

        // All pipe instances are busy, so wait for 20 seconds. 

        if (!WaitNamedPipe(lpszPipename, 20000))
        {
            printf("Could not open pipe: 20 second wait timed out.");
            return -1;
        }
    }

    // The pipe connected; change to message-read mode. 
    g_Tracer.Trace(L"~mynpc~: Pipe connected and change to Message-Read mode.");


    dwMode = PIPE_READMODE_MESSAGE;
    fSuccess = SetNamedPipeHandleState(
        hPipe,    // pipe handle 
        &dwMode,  // new pipe mode 
        NULL,     // don't set maximum bytes 
        NULL);    // don't set maximum time 
    if (!fSuccess)
    {
        g_Tracer.Trace(L"~mynpc~: SetNamedPipeHandleState() Failed GLE=%d", GetLastError());
        _tprintf(TEXT("SetNamedPipeHandleState failed. GLE=%d\n"), GetLastError());
        return -1;
    }

    // Send a message to the pipe server. 

    cbToWrite = (lstrlen(lpvMessage) + 1) * sizeof(TCHAR);
    g_Tracer.Trace(L"~mynpc~: Sending %d byte message: \"%s\"", cbToWrite, lpvMessage);
    _tprintf(TEXT("Sending %d byte message: \"%s\"\n"), cbToWrite, lpvMessage);

    fSuccess = WriteFile(
        hPipe,                  // pipe handle 
        lpvMessage,             // message 
        cbToWrite,              // message length 
        &cbWritten,             // bytes written 
        NULL);                  // not overlapped 

    if (!fSuccess)
    {
        g_Tracer.Trace(L"~mynpc~: WriteFile(): WriteFile to pipe Failed GLE=%d", GetLastError());
        _tprintf(TEXT("WriteFile to pipe failed. GLE=%d\n"), GetLastError());
        return -1;
    }

    g_Tracer.Trace(L"~mynpc~: Message sent to server");
    printf("\nMessage sent to server, receiving reply as follows:\n");

    do
    {
        // Read from the pipe. 

        fSuccess = ReadFile(
            hPipe,    // pipe handle 
            chBuf,    // buffer to receive reply 
            BUFSIZE * sizeof(TCHAR),  // size of buffer 
            &cbRead,  // number of bytes read 
            NULL);    // not overlapped 

        if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
            break;

        _tprintf(TEXT("\"%s\"\n"), chBuf);
    } while (!fSuccess);  // repeat loop if ERROR_MORE_DATA 

    if (!fSuccess)
    {
        g_Tracer.Trace(L"~mynpc~: ReadFile(): ReadFile from pipe Failed GLE=%d", GetLastError());
        _tprintf(TEXT("ReadFile from pipe failed. GLE=%d\n"), GetLastError());
        return -1;
    }

    printf("\n<End of message, press ENTER to terminate connection and exit>");
    
    CloseHandle(hPipe);
    g_Tracer.Trace(L"~mynpc~: Out wmain()");

    return 0;
}
