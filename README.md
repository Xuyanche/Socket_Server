# Socket_Server

## 实验7：基于Socket接口实现自定义协议通信

### 实验目的 

学习如何设计网络应用协议，掌握Socket编程接口编写基本的网络应用软件

### 实验工具

Visual C++、gcc等C++集成开发环境

### 实验内容

- 掌握C语言形式的Socket编程接口用法，能够正确发送和接收网络数据包

- 开发一个客户端，实现人机交互界面和与服务器的通信

- 开发一个服务端，实现并发处理多个客户端的请求

- 程序界面不做要求，使用命令行或最简单的窗体即可

- 功能要求

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

- 根据上述功能要求，设计一个客户端和服务端之间的应用通信协议

- 本实验涉及到网络数据包发送部分不能使用任何的Socket封装类，只能使用最底层的C语言形式的Socket API

- 本实验可组成小组，服务端和客户端可由不同人来完成

## 实验结果提交：

- 实验报告：应用通信协议设计说明，程序功能说明，运行截图及文字说明
- 执行文件：可运行的.exe文件或Linux可执行文件，客户端和服务端各一个
- 源代码：客户端和服务端的代码分别打包成一个压缩文件

























为了防止环境不同导致的不能运行，只上传源代码。visual studio的相关文件（.sln,.vcxporj）会被忽略。

想要运行可以在自己的电脑上新建项目然后带入源代码编译运行。sln建议放在项目根目录下，和readme一层。



[参考](https://blog.csdn.net/JusticeAngle/article/details/87869652)