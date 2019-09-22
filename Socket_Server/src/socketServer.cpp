#include "../header/socketServer.h"


socketServer::socketServer(){
	serverSocket = -1;
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2,1), &wsadata) != 0) {
		printMsg("socket version error");
	}
}


socketServer::~socketServer() {
	closeSocketServer();
}

void socketServer::closeSocketServer() {
	for (auto iter : threadVect) {
		iter->isRunning = false;
	}

	if (serverSocket != -1) {
		closesocket(serverSocket);
	}
	serverSocket = -1;
	WSACleanup();
}


bool socketServer::createSocket(const char* ip, unsigned short port) {
	// create socket
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET) {
		printMsg("socket create failed");
		WSACleanup();
		return false;
	}

	// bind socket	
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.S_un.S_addr = inet_addr(ip);
	int tmp = bind(serverSocket, (SOCKADDR *)&serverAddr, sizeof(SOCKADDR_IN));
	if (tmp == SOCKET_ERROR) {
		printMsg("socket bind error");
		closeSocketServer();
		return false;
	}
	printMsg("bind success");
	return true;
}


bool socketServer::serverListen() {
	if (serverSocket == -1) {
		printMsg("server socket not initialized");
		return false;
	}
	int tmp = listen(serverSocket, 2);
	if (tmp == SOCKET_ERROR) {
		printMsg("socket server listen failed");
		closeSocketServer();
		return false;
	}
	std::cout << "start listen" << std::endl;

	return true;
}


SOCKET socketServer::acceptConnetion() {
	printMsg("in acceptConnection");

	SOCKET client = 0;
	sockaddr_in nClientSocket;
	int nSizeClient = sizeof(nClientSocket);
	while (serverSocket != -1) {
		client = accept(serverSocket, (sockaddr*)&nClientSocket, &nSizeClient);
		if (client == SOCKET_ERROR) {
			printMsg("connect error");
			return 0;
		}
		else {
			// add new client to thread	
			addClientServer(client);
			printMsg("connect success");
		}
		
		Sleep(25);

	}
	return 0;
}


void socketServer::addClientServer(SOCKET& clientSocket) {
	Sthread *nt = new Sthread();
	nt->threadID = ++clientSocketCount;
	nt->isRunning = true;
	nt->client = clientSocket;
	std::thread t(&socketServer::socketRecieveThread, this, nt);
	t.detach();
	nt->t1 = &t;
	threadVect.push_back(nt);

}


void socketServer::socketRecieveThread(Sthread* cthread) {
	while (cthread->isRunning == true)
	{
		
		char buff[65535];
		int nRecv = recv(cthread->client, buff, 65535, 0);
		if (nRecv > 0)
		{
			char str[50];
			sprintf_s(str, "%dthread send message", cthread->threadID);
			printMsg(str);
			printMsg(buff);
			char mess[] = "server:收到了你的消息。";
			send(cthread->client, mess, sizeof(mess), 0);

			// deal with incomming msg and then reply
			handleRecieve(buff, cthread->client);

		}
		else
		{
			char str[50];
			sprintf_s(str, "ID%d is exit", cthread->threadID);
			printMsg(str);
			cthread->isRunning = false;
		}
	}
	return;
}


void socketServer::handleRecieve(char* recieve, SOCKET& from) {
	// add function to return time

	// add function to return name of server

	// add function to return data of connections
}





void printMsg(const char* msg) {
	std::cout << msg << std::endl;
}