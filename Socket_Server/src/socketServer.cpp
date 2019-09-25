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
	printMsg("listening");

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
	struct sockaddr_in sa;
	int len = sizeof(sa);
	if (!getpeername(serverSocket, (struct sockaddr *)&sa, &len))
	{
		printf("对方IP：%s ", inet_ntoa(sa.sin_addr));
		strcpy_s(nt->ip, inet_ntoa(sa.sin_addr));
		printf("对方PORT：%d ", ntohs(sa.sin_port));
		nt->port = ntohs(sa.sin_port);
	}
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
			std::string msg = handleRecieve(buff);
			printMsg(msg);
			send(cthread->client, msg.c_str(), sizeof(msg.c_str()), 0);
			
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


std::string socketServer::handleRecieve(char* recieve) {

	// return time
	if (strcmp(recieve, "time")) {
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		char msg[100];
		snprintf(msg, 100, "%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, sys.wDayOfWeek);
		printMsg(msg);
		std::string stringmsg = msg;
		return stringmsg;
	}
	// return name of server
	if (strcmp(recieve, "servername")) {
		return "lenovo S5";
	}
	// return data of connections
	if (strcmp(recieve, "connection")) {
		std::string msg = "";
		for (auto iter : threadVect) {
			char tmp[100];
			snprintf(tmp, 100, "connetion %d: %s : %d\n", iter->threadID, iter->ip, iter->port);
			msg.append(tmp);
		}
		return msg;
	}
	// transfer data to target client
	else if (1) {

	}


}





void printMsg(const char* msg) {
	std::cout << msg << std::endl;
}


void printMsg(std::string msg) {
	std::cout << msg << std::endl;
}