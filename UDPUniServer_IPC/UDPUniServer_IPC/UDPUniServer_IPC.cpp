#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <stdio.h>
#include"TracerUUS.h"

Tracer g_Tracer;

#pragma comment(lib, "ws2_32.lib")

void InitWinsock()
{
	g_Tracer.Trace(L"~myuus~: In InitWinsock()");
    WSADATA wsaData;
    int wsOk = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsOk != 0)
    {
        printf("Can't Initialize winsock! Quitting\n");
        g_Tracer.Trace(L"~myuus~: WSAStartup() Failed, Can't start Winsock, Err # %d", wsOk);
        return;
    }
    printf("Initialize winsock.\n");
    g_Tracer.Trace(L"~myuus~: WSAStartup() : Initialize winsock Succeeded.");
	g_Tracer.Trace(L"~myuus~: Out InitWinsock()");
}

int main(int argc, wchar_t* argv[])
{
    g_Tracer.Trace(L"~myuus~: In Main()");
    SOCKET socketS;

    InitWinsock();
    struct sockaddr_in local;
    struct sockaddr_in from;
    int fromlen = sizeof(from);
    local.sin_family = AF_INET;
    local.sin_port = htons(1234);
    local.sin_addr.s_addr = INADDR_ANY;

    // SOCK_STREAM use for TCP and SOCK_DGRAM for UDP
    socketS = socket(AF_INET, SOCK_DGRAM, 0); 
    if (socketS == INVALID_SOCKET)
    {
        printf("Can't create a socket! Quitting\n");
        g_Tracer.Trace(L"~myuus~: socket() Failed, Can't create socket, Err # %d", socketS);
        return -1;
    }
    printf("Socket  Created.\n");
    g_Tracer.Trace(L"~myuus~: Socket() : Socket  Created Succeeded.");
        
    int iBindResult = bind(socketS, (sockaddr*)&local, sizeof(local));
    if (iBindResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        g_Tracer.Trace(L"~myuus~: bind() failed with error: %d", WSAGetLastError());
        //freeaddrinfo(result);
        closesocket(socketS);
        g_Tracer.Trace(L"~myuus~: closesocket() Succeeded ");
        WSACleanup();
        g_Tracer.Trace(L"~myuus~: WSACleanup() Succeeded ");
        return 1;
    }
    g_Tracer.Trace(L"~myuus~: bind() Succeeded ");

    while (1)
    {
        char buffer[100];
        ZeroMemory(buffer, sizeof(buffer));
        printf("Waiting...\n");
        if (recvfrom(socketS, buffer, sizeof(buffer), 0, (sockaddr*)&from, &fromlen) != SOCKET_ERROR)
        {
            g_Tracer.Trace(L"~myuus~: recvfrom() Succeeded.");
            g_Tracer.Trace(L"~myuus~: Received message from Client");
            printf("Received message from %s: %s\n", inet_ntoa(from.sin_addr), buffer);
            sendto(socketS, buffer, sizeof(buffer), 0, (sockaddr*)&from, fromlen);
            g_Tracer.Trace(L"~myuus~: Send Echo message back to Client.");
        }
        //Sleep(500);
    }
    closesocket(socketS);
    g_Tracer.Trace(L"~myuus~: closesocket() Close Connection Socket Succeeded ");
    g_Tracer.Trace(L"~myuus~: Out Main()");
    return 0;
}
