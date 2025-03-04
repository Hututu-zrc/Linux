#pragma once

#include "Protocol.hpp"
#include <functional>
using cal_t = std::function<Response(const Request &req)>;
class Prase
{
public:
    Prase(cal_t cal)
        : _cal(cal)
    {
    }
    std::string PraseMessage(std::string &package)
    {

        // package如果包含多个报文的时候，就会出现问题
        // 升级为while循环
        std::string res; // 最终解析完了所有的package报文后的返回报文
        std::string ret; // 单词循环里面的封装好的报文

        while (true)
        {
            std::string context;
            if (!Decode(package, &context))
                break;
            // 2、将我们拿到的数据段放入进行反序列化
            Request req;
            if (req.Deserialize(context))
                break;
            // 3、交给我们的calculate计算层,并保存返回值
            Response resp;
            resp = _cal(req);

            // 4、重新进行序列化
            resp.Serialize(ret);

            // 5、封装包头
            Encode(ret);
            res += ret;
        }
        // 1、判断读取到的长度是否符合要求

        return res;
    }

    ~Prase() {}

private:
    cal_t _cal;
};