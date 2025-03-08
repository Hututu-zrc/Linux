#pragma once
#include  "Log.hpp"
#include "Protocol.hpp"
#include <functional>
using cal_t = std::function<Response(const Request &req)>;
using namespace LogModule;
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
            // 1、判断读取到的长度是否符合要求
            if (!Decode(package, &context))
                break;
            LOG(LogLevel::DEBUG)<<"context: "<<context;

            //1.1、判断返回的context是否为空
            if(context.empty()) break;

            // 2、将我们拿到的数据段放入进行反序列化
            Request req;
            if (!req.Deserialize(context))
                break;
                
            // 3、交给我们的calculate计算层,并保存返回值
            Response resp;
            resp = _cal(req);

            // 4、重新进行序列化
            resp.Serialize(ret);
            LOG(LogLevel::DEBUG)<<"serialize: "<<ret;
            
            //LOG(LogLevel::DEBUG)<<"result: "<<resp.GetResult();
            
            // 5、封装包头
            Encode(ret);
            //LOG(LogLevel::DEBUG)<<"ret: "<<ret;

            res += ret;
            //LOG(LogLevel::DEBUG)<<"res: "<<res;

        }
        //LOG(LogLevel::DEBUG)<<"PraseMessage";
        LOG(LogLevel::DEBUG)<<"res: "<<res;

        return res;
    }

    ~Prase() {}

private:
    cal_t _cal;
};