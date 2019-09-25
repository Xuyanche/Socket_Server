#pragma once
#include <winsock.h>
#include <iostream>
#include <vector>
#include <thread>
#include <time.h>
#include <string>
#include <windows.h>


#pragma comment(lib, "ws2_32.lib")


typedef struct serverThread {
	std::thread *t1 = nullptr;
	bool isRunning = false;
	int threadID = -1;
	SOCKET client = -1;
	char ip[50] = "undefined";
	int port = -1;
} Sthread;


class socketServer {
public:
	socketServer();
	~socketServer();

	//create->bind->listen->accept->addclient->recive/send->close

	bool createSocket(const char* ip, unsigned short port);
	bool serverListen();
	SOCKET acceptConnetion();
	void addClientServer(SOCKET& clientSocket);
	void socketRecieveThread(Sthread *clientSthread);
	void closeSocketServer();



	

private:

	std::string handleRecieve(char* recieve);



	SOCKET serverSocket;
	std::vector<Sthread*> threadVect;
	int clientSocketCount = 0;

};



void printMsg(const char* msg);
void printMsg(std::string msg);