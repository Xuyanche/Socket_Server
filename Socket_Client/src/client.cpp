#include "..\header\socketClient.h"
#include <iostream>

#define MYSERVERIP	"127.0.0.1"
#define MYPROT	16000


int main()
{
	socketClient* client = new socketClient();
	client->createSocket();
	client->createConnection(TEXT(MYSERVERIP), MYPROT);
	delete client;

}

