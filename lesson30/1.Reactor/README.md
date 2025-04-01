# 各文件功能介绍

## 1. `Epoller.hpp`

该文件专门用于包装`epoll`的接口，提供了对`epoll`操作的封装，使得`epoll`的使用更加方便和安全。具体功能如下：

- `Create()`：创建一个`epoll`实例，返回`epoll`的文件描述符。如果创建失败，会记录错误日志并终止程序。
- `Wait()`：等待`epoll`实例上的事件发生，返回就绪的文件描述符数量。如果发生错误，会记录错误日志并终止程序。
- `Ctl()`：对`epoll`实例进行控制操作，如添加、修改或删除文件描述符的监听事件。如果操作失败，会记录错误日志并终止程序。

```cpp
namespace EpollerModule
{
    class Epoller
    {
    public:
        int Create();
        int Wait(struct epoll_event rcev[], int recv_num, int timeout);
        int Ctl(int mode, int fd, uint32_t events);
        // ...
    };
}
```

## 2. `Reactor.hpp`

实现了反应堆模式，将`IO`事件的响应和处理进行分离。通过创建`Epoll`模型，监听多个`IO`源上的事件，如监听套接字和普通连接套接字。具体功能如下：

- `Init()`：初始化`epoll`实例。
- `InsertConnection()`：将新的连接插入到`unordered_map`中，并将其添加到`epoll`内核中进行监听。
- `DeleteConnection()`：从`epoll`内核和`unordered_map`中删除指定的连接。
- `ModifyConnection()`：修改指定连接的监听事件。
- `Loop()`：进入事件循环，不断调用`LoopOnce()`处理就绪的事件。
- `DisPatch()`：根据`epoll`返回的事件，调用相应连接的`Recv()`或`Send()`方法进行处理。



```cpp
class Reactor
{
private:
    std::unique_ptr<Epoller> _epoll;
    std::unordered_map<int, connection_t> _connections;
    // ...
public:
    void Init();
    void InsertConnection(connection_t conn);
    void DeleteConnection(int sockfd);
    void ModifyConnection(int sockfd, bool read_flag, bool write_flag);
    void Loop();
    void DisPatch(int n);
    // ...
};
```

## 3. `Connection.hpp`

对所有的文件描述符进行封装，作为基类，将原本的回调函数变成接口函数。具体功能如下：

- 提供了设置和获取文件描述符、事件、输入输出缓冲区等属性的方法。
- 定义了纯虚函数`Recv()`、`Send()`和`Excep()`，要求派生类实现具体的读写和异常处理逻辑。
- 提供了一个指针`_owner`，指向该连接所属的`Reactor`对象。

```cpp
class Connection
{
public:
    void SetEpollOwner(Reactor *epollserver_ptr);
    Reactor *GetEpollOwner();
    void SetEvents(uint32_t events);
    uint32_t GetEvents();
    void SetSockfd(int sockfd);
    int GetSockfd();
    virtual void Recv() = 0;
    virtual void Send() = 0;
    virtual void Excep() = 0;
    // ...
private:
    int _sockfd;
    std::string _inbuffer;
    std::string _outbuffer;
    Reactor *_owner;
    uint32_t _events;
};
```

## 4. `Listener.hpp`

专门负责获取新的连接，继承自`Connection`类。具体功能如下：

- 在构造函数中初始化监听套接字，并将其设置为非阻塞模式。
- 实现了`Recv()`方法，当监听套接字有新的连接到来时，循环接受新的连接，并将其封装成`IOService`对象，注册到`Reactor`中进行监听。
- 实现了`Send()`和`Excep()`方法，目前为空。



```cpp
class Listener : public Connection
{
public:
    Listener(uint16_t port = 8888);
    void Recv() override;
    void Send() override;
    void Excep() override;
    // ...
private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensock;
};
```

## 5. `IOService.hpp`

处理普通连接的`IO`操作，继承自`Connection`类。具体功能如下：

- 在构造函数中设置连接套接字为非阻塞模式，并设置监听的事件为读事件。
- 实现了`Recv()`方法，循环读取客户端发送的数据，直到缓冲区为空或发生错误。对读取的数据进行处理，调用`HandlerRequest()`函数进行计算，并将结果封装成响应数据，调用`Send()`方法发送给客户端。
- 实现了`Send()`方法，循环将输出缓冲区的数据发送给客户端，直到发送完毕或发生错误。如果发送缓冲区满了，会注册写事件，等待缓冲区有空间时继续发送。
- 实现了`Excep()`方法，处理连接异常，关闭连接并从`Reactor`中删除该连接。

```cpp
class IOService : public Connection
{
public:
    IOService(int sockfd = -1);
    void Recv() override;
    void Send() override;
    void Excep() override;
    void RegisterOnMeassage(func_t onmessage);
    // ...
private:
    int _sockfd;
    func_t _onmessage;
};
```

## 6. `Calculator.hpp`

只负责计算操作，提供了一个`Calculator`类和一个`HandlerRequest()`函数。具体功能如下：

- `Calculator`类的`Execute()`方法根据请求对象`Request`中的操作符进行计算，并返回计算结果的响应对象`Response`。
- `HandlerRequest()`函数从输入缓冲区中解析出完整的请求报文，进行反序列化，调用`Calculator`类的`Execute()`方法进行计算，将计算结果序列化并封装成响应报文，返回给调用者。

```cpp
class Calculator
{
public:
    Response Execute(const Request &req);
    // ...
};

std::string HandlerRequest(std::string &inbuffer);
```

## 7. `Protocol.hpp`

定义了网络通信的协议，包括请求和响应的序列化和反序列化，以及报文的编码和解码。具体功能如下：

- `Encode()`：给序列化的消息添加报头，格式为`报头+Sep+message+Sep`。
- `Decode()`：从报文中解析出完整的消息，去除报头。
- `Request`类：表示请求对象，提供了序列化和反序列化方法。
- `Response`类：表示响应对象，提供了序列化和反序列化方法。

```cpp
bool Encode(std::string &message);
bool Decode(std::string &package, std::string *context);

class Request
{
public:
    bool Serialize(std::string &Outstring);
    bool Deserialize(const std::string &Instring);
    // ...
};

class Response
{
public:
    bool Serialize(std::string &Outstring);
    bool Deserialize(const std::string &Instring);
    // ...
};
```

# 文件协作流程

1. **初始化阶段**
   - `Reactor`类在构造函数中创建`Epoller`对象，并在`Init()`方法中调用`Epoller`的`Create()`方法创建`epoll`实例。
   - `Listener`类在构造函数中初始化监听套接字，并将其设置为非阻塞模式。
   - `Reactor`调用`InsertConnection()`方法将`Listener`对象插入到`unordered_map`中，并将其添加到`epoll`内核中进行监听。
2. **事件循环阶段**
   - `Reactor`的`Loop()`方法进入事件循环，不断调用`LoopOnce()`方法。
   - `LoopOnce()`方法调用`Epoller`的`Wait()`方法等待事件发生。
   - 当有事件发生时，`Reactor`的`DisPatch()`方法根据事件类型调用相应连接的`Recv()`或`Send()`方法。
3. **新连接处理阶段**
   - 当监听套接字有新的连接到来时，`Listener`的`Recv()`方法被调用，循环接受新的连接。
   - 对于每个新的连接，`Listener`将其封装成`IOService`对象，并调用`Reactor`的`InsertConnection()`方法将其插入到`unordered_map`中，并将其添加到`epoll`内核中进行监听。
4. **数据处理阶段**
   - 当普通连接有数据可读时，`IOService`的`Recv()`方法被调用，循环读取客户端发送的数据。
   - `Recv()`方法调用`HandlerRequest()`函数对读取的数据进行处理，该函数调用`Protocol.hpp`中的`Decode()`方法解析出完整的请求报文，进行反序列化。
   - `HandlerRequest()`函数调用`Calculator`类的`Execute()`方法进行计算，将计算结果序列化并封装成响应报文，调用`Protocol.hpp`中的`Encode()`方法添加报头。
   - `Recv()`方法将响应报文设置到输出缓冲区，并调用`Send()`方法发送给客户端。
5. **数据发送阶段**
   - `IOService`的`Send()`方法循环将输出缓冲区的数据发送给客户端。
   - 如果发送缓冲区满了，`Send()`方法会调用`Reactor`的`ModifyConnection()`方法注册写事件，等待缓冲区有空间时继续发送。
6. **异常处理阶段**
   - 当发生异常时，`IOService`的`Excep()`方法被调用，处理连接异常，关闭连接并调用`Reactor`的`DeleteConnection()`方法从`unordered_map`和`epoll`内核中删除该连接。

通过以上协作流程，这些文件共同实现了一个基于`epoll`的网络服务器，能够处理多个客户端的连接和请求，并进行计算和响应。