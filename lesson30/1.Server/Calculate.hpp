#pragma once

#include <iostream>
#include <string>
#include "Protocol.hpp"

class Calculator // 只负责计算操作
{
public:
    Calculator() {}
    // 从request里面拿到计算数据，计算出来结果
    // 然后返回到Response里面的位置
    Response Execute(const Request &req)
    {

        Response resp;
        switch (req.GetOper())
        {
        case '+':
            resp.SetResult(req.GetX() + req.GetY());
            break;
        case '-':
            resp.SetResult(req.GetX() - req.GetY());
            break;
        case '*':
            resp.SetResult(req.GetX() * req.GetY());
            break;
        case '/':
        {
            if (req.GetY() == 0)
                resp.SetCode(1); // 代表除数为0
            else
                resp.SetResult(req.GetX() / req.GetY());
        }
        break;
        case '%':
        {
            if (req.GetY() == 0)
                resp.SetCode(2); // 代表取余的时候除数为0
            else
                resp.SetResult(req.GetX() % req.GetY());
        }
        break;
        default:
            resp.SetCode(3);
        }
        return resp;
    }
    ~Calculator() {}

private:
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
        if (!req.Deserialize(context))
            break;
        std::cout << "#############" << std::endl;
        req.Print();
        std::cout << "#############" << std::endl;
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