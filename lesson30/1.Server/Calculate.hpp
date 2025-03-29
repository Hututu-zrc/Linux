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