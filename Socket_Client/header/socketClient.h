#pragma once
#include <iostream>
#include <string>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define _WINSOCK_DEPRECATED_NO_WARNINGS


class socketClient {

public:
	socketClient();
	~socketClient();

	bool createSocket();
	void closeSocket();
	bool createConnection(PCWSTR ip, const unsigned short port);

	void clientContact();



private:
	int requestTime();
	int requestConnInfo();
	int requestServerName();
	int requestMsgTrans();

	std::string recieveAndPrint();


	SOCKET clientSocket;
	char message[100];

};

void printMsg(char* msg);
void printMsg(std::string msg);
