#include "..\header\socketClient.h"
#include <iostream>




int main()
{

	std::string cmd;

	do {
		printMsg("connect?(y/n)");		
		std::cin >> cmd;
		if (cmd == "y") {
			socketClient* client = new socketClient();
			client->createSocket();
			client->createConnection(TEXT(MYSERVERIP), MYPROT);
			delete client;

		}

	} while (cmd == "y");


}

