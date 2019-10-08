#pragma once
#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include <thread>
#include "../header/constant.h"

#pragma comment(lib, "ws2_32.lib")

#define _WINSOCK_DEPRECATED_NO_WARNINGS


typedef struct listenThread {
	std::thread *t1 = nullptr;
	bool isRunning = false;
} Lthread;



class socketClient {

public:
	socketClient();
	~socketClient();

	
	bool createSocket();
	void closeSocket();
	// create connection to server
	bool createConnection(PCWSTR ip, const unsigned short port);
	void clientContact();

	//listen to server
	void clientListenThread();


private:
	int requestTime();
	int requestConnInfo();
	int requestServerName();
	int requestMsgTrans();
	std::string recieveAndPrint();




	SOCKET clientSocket;
	char requestmsg[65535];
	char message[65535];
	char clientmsg[65535];

	// listen thread;
	Lthread* listen;

};

void printMsg(char* msg);
void printMsg(std::string msg);
