//Multithreaded Pipe Server

#include <windows.h> 
#include <stdio.h> 
#include <tchar.h>
#include <strsafe.h>
#include"TracerNP.h"

#define BUFSIZE 512

Tracer g_Tracer;

DWORD WINAPI InstanceThread(LPVOID);
VOID GetAnswerToRequest(LPTSTR, LPTSTR, LPDWORD);

int wmain()
{
    g_Tracer.Trace(L"~mynps~: In wmain()");

    BOOL   fConnected = FALSE;
    DWORD  dwThreadId = 0;
    HANDLE hPipe = INVALID_HANDLE_VALUE, hThread = NULL;
    LPCTSTR lpszPipename = TEXT("\\\\.\\pipe\\mynamedpipe");

    // The main loop creates an instance of the named pipe and 
    // then waits for a client to connect to it. When the client 
    // connects, a thread is created to handle communications 
    // with that client, and this loop is free to wait for the
    // next client connect request. It is an infinite loop.

    for (;;)
    {
        g_Tracer.Trace(L"~mynps~: Main thread awaiting client connection on %s", lpszPipename);
        _tprintf(TEXT("\nPipe Server: Main thread awaiting client connection on %s\n"), lpszPipename);
        hPipe = CreateNamedPipe(
            lpszPipename,             // pipe name 
            PIPE_ACCESS_DUPLEX,       // read/write access 
            PIPE_TYPE_MESSAGE |       // message type pipe 
            PIPE_READMODE_MESSAGE |   // message-read mode 
            PIPE_WAIT,                // blocking mode 
            PIPE_UNLIMITED_INSTANCES, // max. instances  
            BUFSIZE,                  // output buffer size 
            BUFSIZE,                  // input buffer size 
            0,                        // client time-out 
            NULL);                    // default security attribute 

        if (hPipe == INVALID_HANDLE_VALUE)
        {
            g_Tracer.Trace(L"~mynps~: CreateNamedPipe() Failed, GLE=%d.", GetLastError());
            _tprintf(TEXT("CreateNamedPipe failed, GLE=%d.\n"), GetLastError());
            return -1;
        }
        g_Tracer.Trace(L"~mynps~: CreateNamedPipe() Succeeded.");

        // Wait for the client to connect; if it succeeds, 
        // the function returns a nonzero value. If the function
        // returns zero, GetLastError returns ERROR_PIPE_CONNECTED. 
        g_Tracer.Trace(L"~mynps~: Wait for Client to connect.");

        fConnected = ConnectNamedPipe(hPipe, NULL) ?
            TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

        if (fConnected)
        {
            g_Tracer.Trace(L"~mynps~: Client connected, creating a processing thread");
            printf("Client connected, creating a processing thread.\n");

            // Create a thread for this client. 
            hThread = CreateThread(
                NULL,              // no security attribute 
                0,                 // default stack size 
                InstanceThread,    // thread proc
                (LPVOID)hPipe,    // thread parameter 
                0,                 // not suspended 
                &dwThreadId);      // returns thread ID 

            if (hThread == NULL)
            {
                g_Tracer.Trace(L"~mynps~: CreateThread() Failed, GLE=%d.", GetLastError());
                _tprintf(TEXT("CreateThread failed, GLE=%d.\n"), GetLastError());
                return -1;
            }
            else CloseHandle(hThread);
        }
        else
            // The client could not connect, so close the pipe. 
            CloseHandle(hPipe);
    }
    g_Tracer.Trace(L"~mynps~: Out wmain()");

    return 0;
}

DWORD WINAPI InstanceThread(LPVOID lpvParam)
// This routine is a thread processing function to read from and reply to a client
// via the open pipe connection passed from the main loop. Note this allows
// the main loop to continue executing, potentially creating more threads of
// of this procedure to run concurrently, depending on the number of incoming
// client connections.
{
   
    HANDLE hHeap = GetProcessHeap();
    TCHAR* pchRequest = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE * sizeof(TCHAR));
    TCHAR* pchReply = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE * sizeof(TCHAR));

    DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0;
    BOOL fSuccess = FALSE;
    HANDLE hPipe = NULL;

    // Do some extra error checking since the app will keep running even if this
    // thread fails.

    if (lpvParam == NULL)
    {
        g_Tracer.Trace(L"~mynpc~: InstanceThread() : ERROR - Pipe Server Failure.");
        _tprintf(L"\nERROR - Pipe Server Failure:\n");
        g_Tracer.Trace(L"~mynpc~: InstanceThread() : InstanceThread got an unexpected NULL value in lpvParam.");
        _tprintf(L"   InstanceThread got an unexpected NULL value in lpvParam.\n");
        g_Tracer.Trace(L"~mynpc~: InstanceThread() : InstanceThread exitting.");
        _tprintf(L"   InstanceThread exitting.\n");
        if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
        if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
        return (DWORD)-1;
    }

    if (pchRequest == NULL)
    {
        g_Tracer.Trace(L"~mynpc~: InstanceThread() : ERROR - Pipe Server Failure:");
        _tprintf(L"\nERROR - Pipe Server Failure:\n");
        g_Tracer.Trace(L"~mynpc~: InstanceThread() : InstanceThread got an unexpected NULL heap allocation.");
        _tprintf(L"   InstanceThread got an unexpected NULL heap allocation.\n");
        g_Tracer.Trace(L"~mynpc~: InstanceThread() : InstanceThread exitting.");
        _tprintf(L"   InstanceThread exitting.\n");
        if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
        return (DWORD)-1;
    }

    if (pchReply == NULL)
    {
        g_Tracer.Trace(L"~mynpc~: InstanceThread() : ERROR - Pipe Server Failure:");
        printf("\nERROR - Pipe Server Failure:\n");
        g_Tracer.Trace(L"~mynpc~: () : InstanceThread got an unexpected NULL heap allocation.");
        printf("   InstanceThread got an unexpected NULL heap allocation.\n");
        g_Tracer.Trace(L"~mynpc~: InstanceThread() : InstanceThread exitting.");
        printf("   InstanceThread exitting.\n");
        if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
        return (DWORD)-1;
    }

    // Print verbose messages. In production code, this should be for debugging only.
    g_Tracer.Trace(L"~mynpc~: InstanceThread() InstanceThread created, receiving and processing messages.");
    printf("InstanceThread created, receiving and processing messages.\n");

    // The thread's parameter is a handle to a pipe object instance. 

    hPipe = (HANDLE)lpvParam;

    // Loop until done reading
    while (1)
    {
        // Read client requests from the pipe. This simplistic code only allows messages
        // up to BUFSIZE characters in length.
        fSuccess = ReadFile(
            hPipe,        // handle to pipe 
            pchRequest,    // buffer to receive data 
            BUFSIZE * sizeof(TCHAR), // size of buffer 
            &cbBytesRead, // number of bytes read 
            NULL);        // not overlapped I/O 

        if (!fSuccess || cbBytesRead == 0)
        {
            if (GetLastError() == ERROR_BROKEN_PIPE)
            {
                g_Tracer.Trace(L"~mynpc~: InstanceThread() : client disconnected.");
                _tprintf(TEXT("InstanceThread: client disconnected.\n"));
            }
            else
            {
                g_Tracer.Trace(L"~mynpc~: InstanceThread() : ReadFile() failed, GLE=%d.\n", GetLastError());
                _tprintf(TEXT("InstanceThread ReadFile failed, GLE=%d.\n"), GetLastError());
            }
            break;
        }

        g_Tracer.Trace(L"~mynpc~: InstanceThread() : ReadFile() Succeeded.");
        
        // Process the incoming message.
        g_Tracer.Trace(L"~mynpc~: InstanceThread() :  Process the incoming message.");
        GetAnswerToRequest(pchRequest, pchReply, &cbReplyBytes);

        // Write the reply to the pipe. 
        fSuccess = WriteFile(
            hPipe,        // handle to pipe 
            pchReply,     // buffer to write from 
            cbReplyBytes, // number of bytes to write 
            &cbWritten,   // number of bytes written 
            NULL);        // not overlapped I/O 

        if (!fSuccess || cbReplyBytes != cbWritten)
        {
            g_Tracer.Trace(L"~mynpc~: InstanceThread() : WriteFile() failed, GLE=%d.\n", GetLastError());
            _tprintf(TEXT("InstanceThread WriteFile failed, GLE=%d.\n"), GetLastError());
            break;
        }
    }

    // Flush the pipe to allow the client to read the pipe's contents 
    // before disconnecting. Then disconnect the pipe, and close the 
    // handle to this pipe instance. 

    FlushFileBuffers(hPipe);
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);

    HeapFree(hHeap, 0, pchRequest);
    HeapFree(hHeap, 0, pchReply);

    g_Tracer.Trace(L"~mynpc~: InstanceThread() exiting.");
    printf("InstanceThread exiting.\n");
    return 1;
}

VOID GetAnswerToRequest(LPTSTR pchRequest,
    LPTSTR pchReply,
    LPDWORD pchBytes)
    // This routine is a simple function to print the client request to the console
    // and populate the reply buffer with a default data string. This is where you
    // would put the actual client request processing code that runs in the context
    // of an instance thread. Keep in mind the main thread will continue to wait for
    // and receive other client connections while the instance thread is working.
{
    _tprintf(TEXT("Client Request String:\"%s\"\n"), pchRequest);
    g_Tracer.Trace(L"~mynpc~: GetAnswerToRequest(): Client Request String:\"%s\"\n", pchRequest);


    // Check the outgoing message to make sure it's not too long for the buffer.
    if (FAILED(StringCchCopy(pchReply, BUFSIZE, TEXT("default answer from server"))))
    {
        *pchBytes = 0;
        pchReply[0] = 0;
        g_Tracer.Trace(L"~mynpc~: GetAnswerToRequest(): StringCchCopy() Failed, no outgoing message %d.", GetLastError());
        printf("StringCchCopy failed, no outgoing message.\n");
        return;
    }
    *pchBytes = (lstrlen(pchReply) + 1) * sizeof(TCHAR);
}
