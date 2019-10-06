# Socket服务器和客户端实验报告

## 要求

1. 运输层协议采用TCP或UDP
2. 客户端采用交互菜单形式，用户可以选择以下功能：
     - 连接：请求连接到指定地址和端口的服务端
     - 断开连接：断开与服务端的连接
     - 获取时间: 请求服务端给出当前时间
     - 获取名字：请求服务端给出其机器的名称
     - 活动连接列表：请求服务端给出当前连接的所有客户端信息（编号、IP地址、端口等）
     - 发消息：请求服务端把消息转发给对应编号的客户端，该客户端收到后显示在屏幕上
     - 退出：断开连接并退出客户端程序
3. 服务端接收到客户端请求后，根据客户端传过来的指令完成特定任务：
     - 向客户端传送服务端所在机器的当前时间
     - 向客户端传送服务端所在机器的名称
     - 向客户端传送当前连接的所有客户端信息
     - 将某客户端发送过来的内容转发给指定编号的其他客户端
     - 采用异步多线程编程模式，正确处理多个客户端同时连接，同时发送消息的情况
4. 根据上述功能要求，设计一个客户端和服务端之间的应用通信协议



## C++Socket技术背景

套接字是一个抽象层，应用程序可以通过它发送或接收数据，可对其进行像对文件一样的打开、读写和关闭等操作。套接字允许应用程序将I/O插入到网络中，并与网络中的其他应用程序进行通信。网络套接字是IP地址与端口的组合。

在这一次的实验中，我们使用的是c++中封装的socket方法，其中包含了所有需要使用的方法这些方法，在c++的socket.h中都有定义。服务器端socket的连接通常是按照这样的步骤：

1. 建立socket
2. 绑定ip和端口（bind()）
3. 监听（listen()）
4. 接受客户端的socket连接请求（accept()），这时候会创建一个专门的socket对应这个会话
5. 发送或者接受消息（recv()/send()）

对于客户端socket，方法也是类似的，只不过通常客户端不负责等待socket连接请求，而是主动发出连接请求（connect()）。

## 设计

### 服务器设计

由于服务器的功能要求需要支持同时连接多个客户端，而socket在与客户端连接的时候等待信息会阻塞，所以需要将服务器设计为多线程的。服务器的主线程不断地监听端口传来的请求，如果有新的客户端申请了连接，那么就创建一个线程专门与其通信。

线程的功能是按照得到的命令进行对应的操作。每个线程都负责一个socket连接，这个socket连接对应的客户端地址和端口以及socket和这个线程的内存地址会被存储以方便查找。

特别需要注意的是转发消息的功能。这个功能不会由服务器端主动完成，而是需要客户定时进行请求。这是因为如果服务器突然需要发送一个消息的话，是不能直接通过原来的socket连接的。这是由于客户端不会知道何时进行收取。因此如果服务器需要主动发送一个消息，就要客户端也进行监听，等待服务器进行连接。这样的设计会让服务器和客户端都变得更加复杂，所以不使用这样的方法。反之，客户端定时的发送请求服务器端检查是否有给自己的信息，而服务器将其返回。因此服务器端还需要准备一定的空间作为转发信息的缓存，在客户端请求后将其返回然后清空这一空间。

### 客户端的设计

客户端的任务是接受用户的命令，然后想服务器发送相应的命令，最后接受并处理返回值。由于上述原因，服务器还需要定时向服务器发送检查是否有信息的请求。因此客户端也需要一个后台进程，每隔固定时间就想服务器发送这一请求。



### 通信协议的设计

socket支持多种网络协议的连接，我们选择的是最常见的tcp/ip协议作为基础。

为了检查服务器的连接是否成功，在成功连接服务器之后，服务器第一时间发送确认连接成功的消息，客户端接受以确认连接成功。

由于服务器需要完成的功能数量不多，同时大多也不需要传递很多的参数，所以通信协议的设计就直接由发送命令的简写完成。具体如下：

| 功能     | 协议命令 |
| -------- | -------- |
| 请求时间 | time     |
| 请求服务器名称 | servername |
| 请求服务器连接信息 | conninfo |
| 请求转发信息 | msgtrans&%targetThreadID%&%message% |
| 请求检查是否有消息 | checkmsg |

而服务器直接回复结果。特别的，如果是收到的是检查信息的请求，而没有消息可供回复，那么服务器将返回"NO_MESSAGE"。





## 实现

### C++Socket服务器的实现

#### 服务器类的结构

```c++
typedef struct serverThread {
	std::thread *t1 = nullptr; // thread addr
	bool isRunning = false; // if the thread is running
	int threadID = -1;
	SOCKET client = -1; // save client socket info
	char ip[50] = "undefined"; // save client ip
	int port = -1; // save client port
	bool isBuffUsing = false; // if the message buff is using
	char msgbuff[100]; // save the message recived from other thread
} Sthread;
class socketServer {
public:
	socketServer();
	~socketServer();
	bool createSocket(const char* ip, unsigned short port);// this function is create and bind
	bool serverListen(); // wait for connection
	SOCKET acceptConnetion();
	void addClientServer(SOCKET& clientSocket);
	void socketRecieveThread(Sthread *clientSthread);
	void closeSocketServer();
private:
	void handleRecieve(std::string recieve, Sthread* cthread);
	std::string recieveAndPrint(Sthread* cthread);
	SOCKET serverSocket; // socket server
	std::vector<Sthread*> threadVect; // saving thread info
	int clientSocketCount = 0; // number of client socket
};
```

#### 服务器端接受socket连接并创建新线程

```c++
SOCKET socketServer::acceptConnetion() {
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
			addClientServer(client);// add new client to thread	
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
	/* 
	code of getting and saving client ip & port
    */
	threadVect.push_back(nt); // add thread to threadVect
}
void socketServer::socketRecieveThread(Sthread* cthread) {
	while (cthread->isRunning == true)
	{
		printMsg("waiting for message...");
		threadRefresh(cthread);
		std::string recv = recieveAndPrint(cthread);
		if (recv != "SOCKET_CONNECT_BREAK"){
            /* code for server commandline output, use for debug */
			handleRecieve(recv, cthread); // deal with incomming msg and then reply
		}
		else{
			/*
			code dealing client exit
            */
		}
	}
	return;
}

```






### C++Socket客户端的实现

#### 客户端类的结构

```C++
typedef struct listenThread {
	std::thread *t1 = nullptr; // the pointer to thread
	bool isRunning = false; // indicate if the thread is running
} Lthread;

class socketClient {
public:
	socketClient();
	~socketClient();
	bool createSocket();
	void closeSocket();
	// create connection to server
	bool createConnection(PCWSTR ip, const unsigned short port);
    // create thread here, then process user io and server contact
	void clientContact();
	// ask for message perioudly
	void clientListenThread();

private:
    // function for requsts 
	int requestTime(); 
	int requestConnInfo();
	int requestServerName();
	int requestMsgTrans();
	std::string recieveAndPrint();// wait for response and print out recived message
	SOCKET clientSocket; // saving server socket
	char message[65535];
	// thread to check message
	Lthread* listen;
    char clientmsg[65535]; // message got from server saved here for easier access
};
```

#### 客户端发起socket连接

```c++
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
```

#### 建立定时询问线程

```c++
void socketClient::clientContact() {
	if (clientSocket != -1)
	{
		listen = new Lthread();
		listen->isRunning = true;
        // create thread with function clientListenThread()
		std::thread t(&socketClient::clientListenThread, this);
		t.detach();
		listen->t1 = &t;
		/* 
		functions of waiting user input and react accordingly
		*/
	}
	else
		printMsg("not connecting to server");
}
// this thread will check message peroidly as long as isRunning is true
void socketClient::clientListenThread(){
	while (listen->isRunning) {
		if (clientSocket != -1){
			char tosend[] = "checkmsg";
			send(clientSocket, tosend, sizeof(tosend), 0);
			ZeroMemory(message, sizeof(message));
			int resultRecv = recv(clientSocket, message, sizeof(message), 0);
			if (resultRecv > 0){
				std::string messagestring = message;
				if (messagestring != "NO_MESSAGE") {
					strcpy_s(clientmsg, message);
					printMsg(clientmsg);
				}
				ZeroMemory(message, sizeof(message));
			}
		}
		else{
			printMsg("not connecting to server");
			listen->isRunning = false;
		}
		Sleep(CLIENT_LISTEN_PERIOD); //CLIENT_LISTEN_PERIOD is the constant defined other where to contorl checking peroid
	}
}
```
