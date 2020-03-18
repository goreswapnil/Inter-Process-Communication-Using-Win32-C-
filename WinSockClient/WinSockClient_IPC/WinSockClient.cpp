#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include"tracerWS.h"
#pragma comment(lib, "ws2_32.lib")

using namespace std;

Tracer g_Tracer;

int main()
{
	g_Tracer.Trace(L"~mywsc~: In Main()");

	string ipAddress = "127.0.0.1";			// IP Address of the server
	int port = 54000;						// Listening port # on the server

	// Initialize WinSock
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
	{
		cerr << "Can't start Winsock, Err #" << wsResult << endl;
		g_Tracer.Trace(L"~mywsc~: WSAStartup() Failed, Can't start Winsock, Err # %d",wsResult);
		return -1;
	}
	cout << "Initialize winsock." << endl;
	g_Tracer.Trace(L"~mywsc~: WSAStartup() : Initialize winsock Succeeded.");

	// Create socket
	// SOCK_STREAM use for TCP and SOCK_DGRAM for UDP
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cerr << "Can't create socket, Err #" << WSAGetLastError() << endl;
		g_Tracer.Trace(L"~mywsc~: socket() Failed, Can't create socket, Err # %d", sock);
		WSACleanup();
		return -1;
	}
	cout << "Socket  Created." << endl;
	g_Tracer.Trace(L"~mywsc~: Socket() : Socket Created Succeeded.");

	// Fill in a hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	// Connect to server
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		cerr << "Can't connect to server, Err #" << WSAGetLastError() << endl;
		g_Tracer.Trace(L"~mywsc~: connect() Failed, Can't connect to server, Err # %d", connResult);
		closesocket(sock);
		WSACleanup();
		g_Tracer.Trace(L"~mywsc~: WSACleanup() Succeeded ");
		return -1;
	}
	g_Tracer.Trace(L"~mywsc~: connect to server Succeeded.");

	// Do-while loop to send and receive data
	char buf[4096];
	string userInput;

	do
	{
		// Prompt the user for some text
		cout << "> ";
		getline(cin, userInput);

		if (userInput.size() > 0)		// Make sure the user has typed in something
		{
			// Send the text
			int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
			if (sendResult == SOCKET_ERROR) {
				g_Tracer.Trace(L"~mywsc~: Send Data to Server Failed with error: %d\n", WSAGetLastError());
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(sock);
				WSACleanup();
				return -1;
			}
			
			if (sendResult != SOCKET_ERROR)
			{
				g_Tracer.Trace(L"~mywsc~: Send Data to Server.");
				// Wait for response
				ZeroMemory(buf, 4096);
				int bytesReceived = recv(sock, buf, 4096, 0);
				if (bytesReceived > 0)
				{
					// Echo response to console
					g_Tracer.Trace(L"~mywsc~: Receive Data from Server.");
					cout << "SERVER> " << string(buf, 0, bytesReceived) << endl;
				}
			}
		}

	} while (userInput.size() > 0);

	// shutdown the connection since no more data will be sent
	int iResult = shutdown(sock, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	g_Tracer.Trace(L"~mywsc~: shutdown() Succeeded ");


	// Gracefully close down everything
	closesocket(sock);
	g_Tracer.Trace(L"~mywsc~: closesocket() Succeeded ");
	WSACleanup();
	g_Tracer.Trace(L"~mywsc~: WSACleanup() Succeeded ");

	g_Tracer.Trace(L"~mywsc~: Out Main()");

	return 0;
}
