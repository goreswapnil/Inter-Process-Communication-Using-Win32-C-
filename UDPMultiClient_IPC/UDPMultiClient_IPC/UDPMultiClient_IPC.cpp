#define _WINSOCK_DEPRECATED_NO_WARNINGS 

#include <Winsock2.h>
#include <Windows.h> 
#include <string>
#include <stdio.h>
#include"TracerUMC.h"
#pragma comment(lib, "ws2_32.lib")

Tracer g_Tracer;
#define MSGBUFSIZE 256


int main(int argc, char* argv[])
{
    g_Tracer.Trace(L"~myumc~: In Main()");
    char group[16] = "239.255.255.250";         //"226.1.1.1";//argv[1]; // e.g. 239.255.255.250 for SSDP
    int port = 1900;                            //4321;//(argv[2]); // 0 if error, which is an invalid port

    // !!! If test requires, make these configurable via args

    const int delay_secs = 1;
    const char* message = "Message from Client..!";


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
    
    if (socketS < 0)
    {
        printf("Can't create a socket! Quitting\n");
        g_Tracer.Trace(L"~myums~: socket() Failed, Can't create socket, Err # %d", socketS);
        return -1;
    }
    printf("Socket  Created.\n");
    g_Tracer.Trace(L"~myums~: Socket() : Socket  Created Succeeded.");

    // set up destination address

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(group);
    addr.sin_port = htons(port);

    // now just sendto() our destination!

    while (1) {
        char ch = 0;
        int nbytes = sendto(
            socketS,
            message,
            strlen(message),
            0,
            (struct sockaddr*) & addr,
            sizeof(addr));
        char msgbuf[MSGBUFSIZE];
        int addrlen = sizeof(addr);
        recvfrom(
            socketS,
            msgbuf,
            strlen(msgbuf),
            0,
            (struct sockaddr*) & addr,
            &addrlen);
    }

    //Sleep(delay_secs * 1000); // Windows Sleep is milliseconds

    WSACleanup();
    return 0;
}