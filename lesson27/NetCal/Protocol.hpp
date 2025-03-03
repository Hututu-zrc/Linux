#pragma once

#include <jsoncpp/json/json.h>

#include <memory>
#include <sstream>
#include <iostream>
#include <string>

const std::string Sep = "\r\n";


//给序列化的message添加报头
//格式：报头+Sep+message+Sep
bool Encode(std::string & message)
{
    if(message.size()==0)   return false;
    std::string package=std::to_string(message.size())+Sep+message+Sep;
    message=package;
    return true;
}

//len \r\n Json \r\n len \r\n Json \r\n
//package是输入的参数，是报文
//context是输出型参数，是数据报
bool Decode(std::string &package,std::string *context)
{
    //首先判断长度
    auto pos=package.find(Sep);
    if(pos==std::string::npos)   return false;

    //拿到报头的数据
    std::string context_length_str=package.substr(0,pos);

    //报头的数据就是数据部分的长度
    int context_length=std::stoi(context_length_str);
    //记录整个报文的长度
    int full_length = context_length_str.size() + context_length + 2*Sep.size();
    //package长度不够所有的长度，直接返回false
    if(package.size()<full_length)
        return false;

    //拿出数据部分
    *context=package.substr(pos+Sep.size(),context_length);

    //将得到的第一个报文删除掉
    package.erase(0,full_length);


}


class Request
{
public:
    Request(int x, int y, char oper)
        : _x(x),
          _y(y),
          _oper(oper)
    {
    }
    bool Serialize( std::string &Outstring)
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
        Outstring=ss.str();
        return true;
    }
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
    int GetX() { return _x; };
    int GetY() { return _y; };
    char GetOper() { return _oper; };
    ~Request() {}

private:
    int _x;
    int _y;
    char _oper;
};

class Response
{
public:
    Response() {}
    bool Serialize(const std::string &Outstring)
    {
        Json::Value root;
        root["result"] = _result;
        root["code"] = _code;

        Json::StreamWriterBuilder wb;
        std::unique_ptr<Json::StreamWriter> w(wb.newStreamWriter());
        std::stringstream ss;
        w->write(root, &ss);
        // std::cout << ss.str() << std::endl;
        return true;
    }
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
    int GetResult() { return _result; }
    ~Response() {}

private:
    int _result;
    int _code; // 标识传输是否出错误
};