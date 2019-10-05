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
	InetPton(AF_INET, ip, &serverAddr.sin_addr.s_addr);
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
	char respond[] = "server acknowledged";
	send(clientSocket, respond, sizeof(respond), 0);
	Sthread *nt = new Sthread();
	nt->threadID = ++clientSocketCount;
	nt->isRunning = true;
	nt->client = clientSocket;
	std::thread t(&socketServer::socketRecieveThread, this, nt);
	t.detach();
	nt->t1 = &t;
	char clientip[INET_ADDRSTRLEN];
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	if (getpeername(clientSocket, (struct sockaddr*)&addr, &addrlen) != -1) {
		strcpy_s(nt->ip, inet_ntop(AF_INET, &(addr.sin_addr), clientip, sizeof(clientip)));
		nt->port = ntohs(addr.sin_port);
	}
	
	threadVect.push_back(nt);

}


void socketServer::socketRecieveThread(Sthread* cthread) {
	while (cthread->isRunning == true)
	{
		printMsg("waiting for message...");
		threadRefresh(cthread);
		std::string recv = recieveAndPrint(cthread);
		if (recv != "SOCKET_CONNECT_BREAK")
		{
			char str[50];
			sprintf_s(str, "%d thread send message", cthread->threadID);
			printMsg(str);
			printMsg(recv);

			// deal with incomming msg and then reply
			handleRecieve(recv, cthread);
			
		}
		else
		{
			char str[50];
			sprintf_s(str, "ID%d exit", cthread->threadID);
			printMsg(str);
			cthread->isRunning = false;
		}
	}
	return;
}


void socketServer::handleRecieve(std::string recieve, Sthread* nowthread) {
	// return time
	if (recieve == "time") {
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		char msg[100];
		snprintf(msg, 100, "%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, sys.wDayOfWeek);
		printMsg("sending: " + *(new std::string(msg)));
		send(nowthread->client, msg, sizeof(msg), 0);
		return;
	}
	// return name of server
	else if (recieve == "servername") {
		printMsg("in servername");
		TCHAR buf[MAX_COMPUTERNAME_LENGTH + 2];
		DWORD buf_size;
		buf_size = sizeof buf - 1;
		GetComputerName(buf, &buf_size);
		send(nowthread->client, buf, sizeof(buf), 0);
		return;
	}
	// return data of connections
	else if (recieve == "conninfo") {
		char msg[200];
		strcpy_s(msg, "");
		for (auto iter : threadVect) {
			if (iter->isRunning == false)
				continue;
			char tmp[100];
			snprintf(tmp, 100, "connetion %d: %s : %d", iter->threadID, iter->ip, iter->port);
			strcat_s(msg, tmp);
		}
		printMsg(msg);
		send(nowthread->client, msg, sizeof(msg), 0);
		return;
	}
	// transfer data to target client	
	else if (recieve.find("msgtrans") != std::string::npos) {
		int tmp = recieve.find("&");
		std::string idstring = recieve.substr(tmp + 1, recieve.find_last_of("&") - tmp - 1);
		int threadid = stringtoint(idstring);
		char toTrans[500];
		ZeroMemory(toTrans, sizeof(toTrans));
		strcpy_s(toTrans, recieve.c_str());
		char msg[100];
		ZeroMemory(msg, sizeof(msg));
		sprintf_s(msg, "roger, thread id=%d, message=%s", threadid, recieve.substr(recieve.find_last_of("&"), std::string::npos).c_str());
		printMsg(msg);
		if ((threadid - 1) >= 0 && ((threadid - 1) < threadVect.size())) {
			if (threadVect[threadid - 1]->isBuffUsing == false) {
				strcpy_s(threadVect[threadid - 1]->msgbuff, recieve.substr(recieve.find_last_of("&"), std::string::npos).c_str());
				threadVect[threadid - 1]->isBuffUsing = true;
				//send(nowthread->client, msg, sizeof(msg), 0);
			}
			else {
				printMsg("blocked");
			}
		}
		else {
			printMsg("thread id invalid");
		}
		

		
	}
	else if (recieve == "checkmsg") {
		char testmsg[65535];
		if (nowthread->isBuffUsing == true ) {
			strcpy_s(testmsg, nowthread->msgbuff);
			nowthread->isBuffUsing = false;
			memset(nowthread->msgbuff, '\0', sizeof(nowthread->msgbuff));
			std::cout << "try sending: " << testmsg << std::endl;
			send(threadVect[0]->client, testmsg, sizeof(testmsg), 0);
		}
		else {
			strcpy_s(testmsg, "NO_MESSAGE");
			printMsg("no msg");
			send(threadVect[0]->client, testmsg, sizeof(testmsg), 0);
		}

	}


}


std::string socketServer::recieveAndPrint(Sthread* nowthread) {
	char message[65535];
	ZeroMemory(message, sizeof(message));
	int resultRecv = recv(nowthread->client, message, sizeof(message), 0);
	if (resultRecv > 0)
	{
		std::string stringmsg = message;
		std::cout << "[client " << nowthread->threadID << "]# " << stringmsg << std::endl;
		memset(message, '\0', sizeof(message));
		return stringmsg;
	}
	else
	{
		//这几种错误码，认为连接是正常的，继续接收
		if ((resultRecv < 0) && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR))
		{
			return "";
		}
		printMsg("socket connection break.");
		return "SOCKET_CONNECT_BREAK";
	}
}

void socketServer::threadRefresh(Sthread* cthread) {

}




inline void printMsg(const char* msg) {
	std::cout << msg << std::endl;
}


inline void printMsg(std::string msg) {
	std::cout << msg << std::endl;
}

int stringtoint(std::string s) {
	int result = 0;
	for (int i = 0; i < s.length(); i++) {
		if (s[i] >= '0' && s[i] <= '9') {
			result *= 10;
			result += (s[i] - '0');
		}
	}
	return result;
}