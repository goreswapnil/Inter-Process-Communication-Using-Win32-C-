#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#include <Winsock2.h> 
#include <Ws2tcpip.h>
#include <string.h>
#include <stdio.h>
#pragma comment(lib, "ws2_32.lib")
#include"TracerUMS.h"

Tracer g_Tracer;

#define MSGBUFSIZE 256

int main(int argc, char* argv[])
{
    g_Tracer.Trace(L"~myums~: In Main()");
    char group[16] = "239.255.255.250";      //"226.1.1.1";//argv[1]; // e.g. 239.255.255.250 for SSDP
    int port = 1900;                         // 4321;//(argv[2]); // 0 if error, which is an invalid port
    
    WSADATA wsaData;
    int wsOk = WSAStartup(0x0101, &wsaData);
    if (wsOk != 0)
    {
        printf("Can't Initialize winsock! Quitting\n");
        g_Tracer.Trace(L"~myums~: WSAStartup() Failed, Can't start Winsock, Err # %d", wsOk);
        return -1;
    }
    printf("Initialize winsock.\n");
    g_Tracer.Trace(L"~myums~: WSAStartup() : Initialize winsock Succeeded.");
   
    // create what looks like an ordinary UDP socket

    int socketS = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketS == INVALID_SOCKET)
    {
        printf("Can't create a socket! Quitting\n");
        g_Tracer.Trace(L"~myums~: socket() Failed, Can't create socket, Err # %d", socketS);
        return -1;
    }
    printf("Socket  Created.\n");
    g_Tracer.Trace(L"~myums~: Socket() : Socket  Created Succeeded.");

    // allow multiple sockets to use the same PORT number

    u_int yes = 1;
    if (setsockopt(
        socketS, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes)
    ) < 0
        ) {
        printf("setsockopt() failed with error: %d\n", WSAGetLastError());
        g_Tracer.Trace(L"~myums~: setsockopt() allow multi addrs failed with error: %d", WSAGetLastError());
        return -1;
    }
    g_Tracer.Trace(L"~myums~: setsockopt() Succeeded ");

    // set up destination address

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // differs from sender
    addr.sin_port = htons(port);

    // bind to receive address

    int iBindResult = bind(socketS, (struct sockaddr*) & addr, sizeof(addr));
    if (iBindResult < 0 ) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        g_Tracer.Trace(L"~myums~: bind() failed with error: %d", WSAGetLastError());
        //freeaddrinfo(result);
        closesocket(socketS);
        g_Tracer.Trace(L"~myums~: closesocket() Succeeded ");
        WSACleanup();
        g_Tracer.Trace(L"~myums~: WSACleanup() Succeeded ");
        return -1;
    }
    g_Tracer.Trace(L"~myums~: bind() Succeeded ");

    // use setsockopt() to request that the kernel join a multicast group

    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(group);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (
        setsockopt(
            socketS, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)
        ) < 0
        ) {
        printf("setsockopt() failed with error: %d\n", WSAGetLastError());
        g_Tracer.Trace(L"~myums~: setsockopt() failed with error: %d", WSAGetLastError());
        return -1;
    }
    g_Tracer.Trace(L"~myums~: setsockopt() Succeeded ");

    // now just enter a read-print loop

    while (1)
    {
        char msgbuf[MSGBUFSIZE];
        ZeroMemory(msgbuf, sizeof(msgbuf));
        int addrlen = sizeof(addr);
        if(recvfrom(socketS, msgbuf, strlen(msgbuf), 0, (struct sockaddr*) & addr, &addrlen))
        {
            g_Tracer.Trace(L"~myums~: recvfrom() Succeeded.");
            g_Tracer.Trace(L"~myums~: Received message from Client");
            printf("Received message from %s: %s\n", inet_ntoa(addr.sin_addr), msgbuf);
            sendto(socketS, msgbuf, strlen(msgbuf), 0, (struct sockaddr*) & addr,sizeof(addr));
        }
    }
    closesocket(socketS);
    g_Tracer.Trace(L"~myums~: closesocket() Close Connection Socket Succeeded ");
    g_Tracer.Trace(L"~myums~: Out Main()");
    return 0;  
}

