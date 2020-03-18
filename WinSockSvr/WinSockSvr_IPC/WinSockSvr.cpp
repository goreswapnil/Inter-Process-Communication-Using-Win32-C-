#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include"TracerWS.h"

#pragma comment (lib, "ws2_32.lib")
using namespace std;

Tracer g_Tracer;

int main()
{
	g_Tracer.Trace(L"~mywss~: In Main()");
	// Initialze winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);			//MAKEWORD(2, 2) parameter of WSAStartup makes a request for version 2.2 of Winsock on the system, 
										//and sets the passed version as the highest version of Windows Sockets support that the caller can use.
	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		cerr << "Can't Initialize winsock! Quitting" << endl;
		g_Tracer.Trace(L"~mywss~: WSAStartup() Failed, Can't start Winsock, Err # %d", wsOk);
		return -1;
	}
	cout << "Initialize winsock." << endl;
	g_Tracer.Trace(L"~mywss~: WSAStartup() : Initialize winsock Succeeded.");

	// SOCK_STREAM uses for TCP and SOCK_DGRAM uses for UDP
	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);			
	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Quitting" << endl;
		g_Tracer.Trace(L"~mywss~: socket() Failed, Can't create socket, Err # %d", listening);
		return -1;
	}
	cout << "Socket  Created." << endl;
	g_Tracer.Trace(L"~mywss~: Socket() : Socket  Created Succeeded.");

	// Bind the ip address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inet_pton .... 
	
	// Setup the TCP listening socket
	int iBindResult = bind(listening, (sockaddr*)&hint, sizeof(hint));
	if (iBindResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		g_Tracer.Trace(L"~mywss~: bind() failed with error: %d", WSAGetLastError());
		//freeaddrinfo(result);
		closesocket(listening);
		g_Tracer.Trace(L"~mywss~: closesocket()  Close listening socket Succeeded ");
		WSACleanup();
		g_Tracer.Trace(L"~mywss~: WSACleanup() Succeeded ");
		return 1;
	}
	g_Tracer.Trace(L"~mywss~: bind() Setup the TCP listening socket Succeeded ");

	// Tell Winsock the socket is for listening 
	int iResult = listen(listening, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		g_Tracer.Trace(L"~mywss~: listen() failed with error: %d", WSAGetLastError());
		closesocket(listening);
		g_Tracer.Trace(L"~mywss~: closesocket()  Close listening socket Succeeded ");
		WSACleanup();
		g_Tracer.Trace(L"~mywss~: WSACleanup() Succeeded ");
		return 1;
	}
	g_Tracer.Trace(L"~mywss~: listen() listening socket Succeeded ");

	g_Tracer.Trace(L"~mywss~: Wait for a connection");
	printf("Wait for a connection");
	// Wait for a connection
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
	if (clientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		g_Tracer.Trace(L"~mywss~: accept() failed with error: %d", WSAGetLastError());
		closesocket(listening);
		g_Tracer.Trace(L"~mywss~: closesocket()  Close listening socket Succeeded ");
		WSACleanup();
		g_Tracer.Trace(L"~mywss~: WSACleanup() Succeeded ");
		return 1;
	}
	g_Tracer.Trace(L"~mywss~: accept() Succeeded ");


	char host[NI_MAXHOST];		// Client's remote name
	char service[NI_MAXSERV];	// Service (i.e. port) the client is connect on

	ZeroMemory(host, NI_MAXHOST); // same as memset(host, 0, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << host << " connected on port " << service << endl;
		g_Tracer.Trace(L"~mywss~: %d connected on port %d.", host, service);
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " << ntohs(client.sin_port) << endl;
		g_Tracer.Trace(L"~mywss~: connected on port. %ud", ntohs(client.sin_port));
	}

	// While loop: accept and echo message back to client
	char buf[4096];

	while (true)
	{
		ZeroMemory(buf, 4096);

		// Wait for client to send data
		g_Tracer.Trace(L"~mywss~: Wait for client to send data.");
		int bytesReceived = recv(clientSocket, buf, 4096, 0);

		if (bytesReceived == SOCKET_ERROR)
		{
			cerr << "Error in recv(). Quitting" << endl;
			g_Tracer.Trace(L"~mywss~: recv() Failed with error: %d", WSAGetLastError());
			break;
		}

		if (bytesReceived == 0)
		{
			cout << "Client disconnected " << endl;
			g_Tracer.Trace(L"~mywss~: Client disconnected");
			break;
		}

		g_Tracer.Trace(L"~mywss~: recv() Data Succeeded.");
		cout << string(buf, 0, bytesReceived) << endl;

		// Echo message back to client
		int sendResult = send(clientSocket, buf, bytesReceived + 1, 0);
		if (sendResult == SOCKET_ERROR) {
			g_Tracer.Trace(L"~mywsc~: Send Data to Client Failed with error: %d\n", WSAGetLastError());
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(clientSocket);
			WSACleanup();
			return -1;
		}
		g_Tracer.Trace(L"~mywss~: Send Echo message back to Client.");
	}

	// Close listening socket
	closesocket(listening);
	g_Tracer.Trace(L"~mywss~: closesocket()  Close listening socket Succeeded ");

	// Close the socket
	closesocket(clientSocket);
	g_Tracer.Trace(L"~mywss~: closesocket() Close Connection Socket Succeeded ");

	// Cleanup winsock
	WSACleanup();
	g_Tracer.Trace(L"~mywss~: WSACleanup() Succeeded ");

	//system("pause");
	g_Tracer.Trace(L"~mywsc~: Out Main()");

	return 0;
}