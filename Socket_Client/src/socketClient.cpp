#include "../header/socketClient.h"
#include <cstring>



socketClient::socketClient() {
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
		int resultRecv = -1;
		while (true)
		{
			std::string command;
			printMsg("please enter command");
			std::cin >> command;
			if (command == "time") {
				resultRecv = requestTime(clientSocket);
			}
			else if (command == "connInfo") {
				resultRecv = requestConnInfo(clientSocket);
			}
			else if (command == "serverName") {

			}
			else if (command == "msgTrans") {

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




int socketClient::requestTime() {
	if (clientSocket != -1)
	{
		std::string tosend = "time";
		send(clientSocket, tosend.c_str(), sizeof(tosend.c_str()), 0);
		if (recieveAndPrint() == "SOCKET_CONNECT_BREAK")
			return -1;
		if (recieveAndPrint() == "SOCKET_CONNECT_BREAK")
			return -1;
		return 1;
	}
	else
	{
		printMsg("not connecting to server");
		return -1;
	}
}

int socketClient::requestConnInfo() {

}

int socketClient::requestServerName() {

}

int socketClient::requestMsgTrans() {

}


std::string socketClient::recieveAndPrint() {
	int resultRecv = recv(clientSocket, message, sizeof(message), 0);
	if (resultRecv > 0)
	{
		printMsg(*(new std::string(message)));
		memset(message, '\0', sizeof(message));
		return *(new std::string(message));
	}
	else
	{
		//这几种错误码，认为连接是正常的，继续接收
		if ((resultRecv < 0) && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR))
		{
			return "";
		}
		printMsg("与服务器连接中断！");
		return "SOCKET_CONNECT_BREAK";
	}
}

























void ouptutMsg(char* msg) {
	std::cout << msg << std::endl;
}

void printMsg(std::string msg) {
	std::cout << msg << std::endl;
}