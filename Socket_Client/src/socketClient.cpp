#include "../header/socketClient.h"
#include <cstring>



socketClient::socketClient() {
	ZeroMemory(clientmsg, sizeof(clientmsg));
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
		printMsg("successfully connect to server");
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
			if (clientmsg) {

			}
			std::string command;
			printMsg("avaliable command:  time / connInfo / serverName / msgTrans");
			std::cout << "please enter command:";
			std::cin >> command;
			if (command == "time") {
				resultRecv = requestTime();
			}
			else if (command == "connInfo") {
				resultRecv = requestConnInfo();
			}
			else if (command == "serverName") {
				resultRecv = requestServerName();
			}
			else if (command.find("msgTrans") >= 0) {
				resultRecv = requestMsgTrans();
			}
			else {
				printMsg("invalid command");
				resultRecv = 1;
			}


			if (resultRecv < 0)
			{
				printMsg("connection break");
				break;//跳出接收循环
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
			std::string tosend = "checkmsg";
			send(clientSocket, tosend.c_str(), sizeof(tosend.c_str()), 0);
			ZeroMemory(message, sizeof(message));
			int resultRecv = recv(clientSocket, message, sizeof(message), 0);
			
			if (resultRecv > 0)
			{
				if (strcmp(message, "no") != 0) {
					strcpy_s(clientmsg, message);
					printMsg(message);
				}
				ZeroMemory(message, sizeof(message));
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
		std::string tosend = "time";
		send(clientSocket, tosend.c_str(), sizeof(tosend.c_str()), 0);
		if (recieveAndPrint() == SOCKET_CONNECTION_BREAK)
			return -1;
	}
	else
	{
		printMsg("not connecting to server");
		return -1;
	}
}

int socketClient::requestConnInfo() {
	if (clientSocket != -1)
	{
		std::string tosend = "conninfo";
		send(clientSocket, tosend.c_str(), sizeof(tosend.c_str()), 0);
		if (recieveAndPrint() == SOCKET_CONNECTION_BREAK)
			return -1;
		return 1;
	}
	else
	{
		printMsg("not connecting to server");
		return -1;
	}
}

int socketClient::requestServerName() {
	if (clientSocket != -1)
	{
		char tosend[] = "servername";
		send(clientSocket, tosend, sizeof(tosend), 0);
		if (recieveAndPrint() == SOCKET_CONNECTION_BREAK)
			return -1;
		return 1;
	}
	else
	{
		printMsg("not connecting to server");
		return -1;
	}
}

int socketClient::requestMsgTrans() {
	if (clientSocket != -1)
	{
		char tosend[65535];
		int threadID=1;
		char totrans[500];
		strcpy_s(totrans, "test");
		sprintf_s(tosend, "msgtrans&%d&%s", threadID, totrans);
		printMsg(tosend);
		send(clientSocket, tosend, sizeof(tosend), 0);
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
		//这几种错误码，认为连接是正常的，继续接收
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