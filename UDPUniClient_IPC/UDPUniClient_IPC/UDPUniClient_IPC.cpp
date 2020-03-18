#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <iostream>
#include<string>
#include"TracerUUC.h"
using namespace std;
#pragma comment(lib, "ws2_32.lib")

Tracer g_Tracer;

void InitWinsock()
{
    WSADATA wsaData;
    int wsOk = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsOk != 0)
    {
        printf("Can't Initialize winsock! Quitting\n");
        g_Tracer.Trace(L"~myuuc~: WSAStartup() Failed, Can't start Winsock, Err # %d", wsOk);
        return;
    }
    printf("Initialize winsock.\n");
    g_Tracer.Trace(L"~myuuc~: WSAStartup() : Initialize winsock Succeeded.");
}

int main(int argc, char* argv[])
{
    g_Tracer.Trace(L"~myuuc~: In Main()");
    SOCKET socketC;

    InitWinsock();
    struct sockaddr_in serverInfo;
    int len = sizeof(serverInfo);
    serverInfo.sin_family = AF_INET;
    serverInfo.sin_port = htons(1234);
    serverInfo.sin_addr.s_addr = inet_addr("127.0.0.1");

    // SOCK_STREAM use for TCP and SOCK_DGRAM for UDP
    socketC = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketC == INVALID_SOCKET)
    {
        printf("Can't create a socket! Quitting\n");
        g_Tracer.Trace(L"~myuuc~: socket() Failed, Can't create socket, Err # %d", socketC);
        return -1;
    }
    printf("Socket  Created.\n");
    g_Tracer.Trace(L"~myuuc~: Socket() : Socket  Created Succeeded.");

    while (1)
    {
        char buffer[50],buffer1[50];
        ZeroMemory(buffer, sizeof(buffer));
       
        printf("Please input your message: ");
        cin >> buffer;
        if (strcmp(buffer, "exit") == 0)
            break;
        if (sendto(socketC, buffer, sizeof(buffer), 0, (sockaddr*)&serverInfo, len) != SOCKET_ERROR)
        {
            g_Tracer.Trace(L"~myuuc~: sendto() Succeeded.");
            g_Tracer.Trace(L"~myuuc~: Send Datagram to server");
            if (recvfrom(socketC, buffer1, sizeof(buffer1), 0, (sockaddr*)&serverInfo, &len))
            {
                g_Tracer.Trace(L"~myuuc~: recvfrom() Succeeded.");
                g_Tracer.Trace(L"~myuuc~: Receive response from server");
                printf("Receive response from server: %s\n", buffer1);
            }
        }
    }
    closesocket(socketC);
    g_Tracer.Trace(L"~myuuc~: closesocket() Close Connection Socket Succeeded ");
    g_Tracer.Trace(L"~myuuc~: Out Main()");
    return 0;
}