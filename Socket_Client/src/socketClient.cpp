#include "../header/socketClient.h"
#include <cstring>

int timenum = 0;

socketClient::socketClient() {
	ZeroMemory(clientmsg, sizeof(clientmsg));
	ZeroMemory(requestmsg, sizeof(requestmsg));
	clientSocket = -1;
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
		printMsg("socket initialize failed");
	}
}
socketClient::~socketClient() {
	closeSocket();
}

bool socketClient::createSocket() {
	if (clientSocket == -1) {
		clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (clientSocket == SOCKET_ERROR) {
			printMsg("socket create failed");
			return false;
		}
	}
	printMsg("socket create success");
	return true;

}
void socketClient::closeSocket() {
	if (clientSocket != -1) {
		closesocket(clientSocket);
	}
	clientSocket = -1;
	WSACleanup();

}
bool socketClient::createConnection(PCWSTR ip, const unsigned short port) {
	int tmp = SOCKET_ERROR;
	if (clientSocket != -1) {
		sockaddr_in serveraddr;
		memset(&serveraddr, 0, sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_port = htons(port);
		InetPton(AF_INET, ip, &serveraddr.sin_addr.s_addr);

		tmp = connect(clientSocket, (sockaddr*)&serveraddr, sizeof(serveraddr));

		if (tmp == SOCKET_ERROR) {
			printMsg("cannot connect to server");
			return false;
		}
		if (recieveAndPrint() == SOCKET_CONNECTION_BREAK)
			return false;
		clientContact();
		return true;
	}
	return false;
}


void socketClient::clientContact() {
	if (clientSocket != -1)
	{
		listen = new Lthread();
		listen->isRunning = true;
		std::thread t(&socketClient::clientListenThread, this);
		t.detach();
		listen->t1 = &t;

		int resultRecv = -1;
		while (true)
		{
			std::string command;
			printMsg("avaliable command:  time / connInfo / serverName / msgTrans");
			std::cout << "please enter command:";
			std::cin >> command;
			ZeroMemory(requestmsg, sizeof(requestmsg));
			if (command == "time") {
				resultRecv = requestTime();
			}
			else if (command == "connInfo") {
				resultRecv = requestConnInfo();
			}
			else if (command == "serverName") {
				resultRecv = requestServerName();
			}
			else if (command == "msgTrans") {
				resultRecv = requestMsgTrans();
			}
			else if (command == "disconnect") {
				printMsg("disconnecting");
				break;
			}
			else if (command == "check") {
				printMsg(clientmsg);
			}
			else if (command == "changecheckstate") {
				listen->isRunning = (!listen->isRunning);
			}
			else {
				printMsg("invalid command");
				resultRecv = 1;
			}


			if (resultRecv < 0)
			{
				printMsg("connection break");
				break;
			}
			
		}
	}
	else
	{
		printMsg("not connecting to server");
	}
}

void socketClient::clientListenThread()
{
	while (listen->isRunning) {
		if (clientSocket != -1)
		{
			if (strcmp(requestmsg, "") != 0 ) {

				send(clientSocket, requestmsg, sizeof(requestmsg), 0);
				ZeroMemory(requestmsg, sizeof(requestmsg));

				if (recieveAndPrint() == SOCKET_CONNECTION_BREAK) {
					closeSocket();
				}
			}
			else {
				char tosend[] = "checkmsg";
				send(clientSocket, tosend, sizeof(tosend), 0);
				ZeroMemory(message, sizeof(message));
				int resultRecv = recv(clientSocket, message, sizeof(message), 0);

				if (resultRecv > 0)
				{
					//printMsg(message);	
					std::string messagestring = message;
					if (messagestring != "NO_MESSAGE") {
						strcpy_s(clientmsg, message);
						printMsg(messagestring);
					}
					ZeroMemory(message, sizeof(message));
				}
			}

			
		}
		else
		{
			printMsg("not connecting to server");
			listen->isRunning = false;
		}

		Sleep(CLIENT_LISTEN_PERIOD);
	}
}




int socketClient::requestTime() {
	if (clientSocket != -1)
	{
		strcpy_s(requestmsg, "time");
		return 1;
	}
	printMsg("not connecting to server");
	return -1;
}

int socketClient::requestConnInfo() {
	if (clientSocket != -1)
	{
		strcpy_s(requestmsg, "conninfo");
		return 1;
	}
	printMsg("not connecting to server");
	return -1;
}

int socketClient::requestServerName() {
	if (clientSocket != -1)
	{
		strcpy_s(requestmsg, "servername");
		return 1;
			
	}
	printMsg("not connecting to server");
	return -1;
}

int socketClient::requestMsgTrans() {
	if (clientSocket != -1)
	{
		printMsg("please input target thread ID&msg");
		int threadID = -1;
		std::cin >> threadID;
		printMsg("please input message");
		char totrans[65535];
		std::cin >> totrans;
		sprintf_s(requestmsg, "msgtrans&%d&%s", threadID, totrans);
		return 1;
	}
	else
	{
		printMsg("not connecting to server");
		return -1;
	}
}


std::string socketClient::recieveAndPrint() {
	ZeroMemory(message, sizeof(message));
	int resultRecv = recv(clientSocket, message, sizeof(message), 0);
	if (resultRecv > 0)
	{
		std::string stringmsg = message;
		printMsg("[server]# "+stringmsg);
		memset(message, '\0', sizeof(message));
		return stringmsg;
	}
	else
	{
		if ((resultRecv < 0) && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR))
		{
			return "blocked";
		}
		printMsg("connection break");
		return SOCKET_CONNECTION_BREAK;
	}
}










void printMsg(char* msg) {
	std::cout << msg << std::endl;
}

void printMsg(std::string msg) {
	std::cout << msg << std::endl;
}