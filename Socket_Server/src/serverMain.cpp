#include "../header/socketServer.h"
using namespace std;

#define MYSERVERIP	"127.0.0.1"
#define MYPROT	16000

int main() {

	socketServer* server = new socketServer();
	//create->bind->listen->accept->recive/send->close
	server->createSocket(MYSERVERIP, MYPROT);
	server->serverListen();
	server->acceptConnetion();




	system("pause");
	return 0;
}