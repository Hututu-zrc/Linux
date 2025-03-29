#pragma once

#include <jsoncpp/json/json.h>

#include <memory>
#include <sstream>
#include <iostream>
#include <string>
#include "Calculate.hpp"
const std::string Sep = "\r\n";

// 给序列化的message添加报头
// 格式：报头+Sep+message+Sep
bool Encode(std::string &message)
{
    if (message.size() == 0)
        return false;
    std::string package = std::to_string(message.size()) + Sep + message + Sep;
    message = package;
    return true;
}

// len \r\n Json \r\n len \r\n Json \r\n
// package是输入的参数，是报文
// context是输出型参数，是数据报
bool Decode(std::string &package, std::string *context)
{
    // 首先判断长度
    auto pos = package.find(Sep);
    if (pos == std::string::npos)
        return false;

    // 拿到报头的数据
    std::string context_length_str = package.substr(0, pos);

    // 报头的数据就是数据部分的长度
    int context_length = std::stoi(context_length_str);
    // 记录整个报文的长度
    int full_length = context_length_str.size() + context_length + 2 * Sep.size();
    // package长度不够所有的长度，直接返回false
    if (package.size() < full_length)
        return false;

    // 拿出数据部分
    *context = package.substr(pos + Sep.size(), context_length);

    // 将得到的第一个报文删除掉
    package.erase(0, full_length);
    return true;
}

class Request
{
public:
    Request(int x = 0, int y = 0, char oper = 0)
        : _x(x),
          _y(y),
          _oper(oper)
    {
    }

    // 这里的请求序列化，主要是给客户端使用的
    bool Serialize(std::string &Outstring)
    {
        // Josn的本质是键值对记录数据的
        Json::Value root;
        root["x"] = _x;
        root["y"] = _y;
        root["oper"] = _oper;
        //: Json::StreamWriterBuilder 是一个工具类，
        // 用于配置和生成 JSON 数据流的写入器 (StreamWriter)
        Json::StreamWriterBuilder wb;

        // wb.newStreamWriter() 会返回一个堆上分配的 Json::StreamWriter 对象，
        // 用于将 JSON 数据写入流
        std::unique_ptr<Json::StreamWriter> w(wb.newStreamWriter());
        // 这里 ss 用作目标缓冲区，Json::StreamWriter 会将 JSON 数据写入其中
        std::stringstream ss;
        // w->write(root, &ss) 会将 root 中的 JSON 数据序列化为字符串，并写入到 std::stringstream 对象 ss 中
        // write函数就是写入序列化的方式
        w->write(root, &ss);

        // std::cout << ss.str() << std::endl;
        Outstring = ss.str();
        return true;
    }

    // 这里的请求反序列化，主要是给服务器使用的，服务器从网络拿到字节流后发序列化出来数据
    bool Deserialize(const std::string &Instring)
    {
        Json::Value root;
        Json::Reader reader;
        bool parsingSuccessful = reader.parse(Instring, root);
        if (!parsingSuccessful)
        {
            std::cout << "Failed to parse Json" << reader.getFormattedErrorMessages() << std::endl;
            return false;
        }
        _x = root["x"].asInt();
        _y = root["y"].asInt();
        _oper = root["oper"].asInt(); // 字符也是整形家族

        return true;
    }

    void Print()
    {
        std::cout << _x << std::endl;
        std::cout << _y << std::endl;
        std::cout << _oper << std::endl;
    }
    int GetX() const { return _x; };
    int GetY() const { return _y; };
    char GetOper() const { return _oper; };
    ~Request() {}

private:
    int _x;
    int _y;
    char _oper;
};

class Response
{
public:
    Response(int result = 0)
        : _result(result),
          _code(0)
    {
    }

    // 服务器计算完了以后，将数据序列化后传输至网络
    bool Serialize(std::string &Outstring)
    {
        Json::Value root;
        root["result"] = _result;
        root["code"] = _code;

        Json::StreamWriterBuilder wb;
        std::unique_ptr<Json::StreamWriter> w(wb.newStreamWriter());
        std::stringstream ss;
        w->write(root, &ss);
        Outstring = ss.str();
        // std::cout << ss.str() << std::endl;
        return true;
    }

    // 客户端收到以后，进行反序列化，打印出来
    bool Deserialize(const std::string &Instring)
    {
        Json::Value root;
        Json::Reader reader;
        bool parsingSuccessful = reader.parse(Instring, root);
        if (!parsingSuccessful)
        {
            std::cout << "Failed to parse Json" << reader.getFormattedErrorMessages() << std::endl;
            return false;
        }
        _result = root["result"].asInt();
        _code = root["code"].asInt();

        return true;
    }
    int GetResult() const { return _result; }
    int GetCode() const { return _code; }
    void SetResult(int result) { _result = result; }
    void SetCode(int code) { _code = code; };
    ~Response() {}

private:
    int _result;
    int _code; // 标识传输是否出错误
};

std::string HandlerRequest(std::string &inbuffer)
{
    std::string context; // Decode解析以后，拿到的完整报文的数据部分

    std::string result_str; // 最后接收到的所有报文经过处理后，返回的结果报文
    while (Decode(inbuffer, &context))
    {
        if (context.empty())
            return std::string();
        // 1、反序列化
        Request req;
        req.Deserialize(context);

        // 2、应用层处理
        Calculator cal;
        Response resp = cal.Execute(req);

        // 3、序列化
        std::string ret_mes;
        resp.Serialize(ret_mes);

        // 4、合成报文Encode
        Encode(ret_mes);
        // 5、返回
        result_str += ret_mes;
    }
    return result_str;
}