#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include "Common.hpp"
const std::string Sep = "\r\n";        // 行与行之间的分隔符
const std::string LineSep = " ";       // 请求行里面的分隔符
const std::string HeaderLineSep = ":"; // 请求报头的每行里面的分隔符key:value式的
const std::string BlankLine = "\r\n";  // 空行
const std::string DefaultHomePath = "wwwroot/";
const std::string http_version = "HTTP/1.0";
const std::string page404 = "/404.html";
const std::string firstpage="index.html";
// B/S模式
class HttpRequest
{
    // 在反序列化的过程中，细化我们解析出来的字段
public:
    HttpRequest() {}

    // 反序列化就是按照分隔符将字符串打散
    void Deserialize(std::string &request_str)
    {

        if (PraseOneLine(request_str, &_req_line, Sep))
        {
            PraseRequestLine(_req_line, LineSep);
            ParseHeader(request_str);
            _body = request_str;
        }
        // 提取请求行里面的详细字段
    }

    // 打开用户指定的路径下面的信息
    // 用户指定的路径下，文件存储在我们的服务器上面
    // 需要拿到该uri下面的信息
    std::string GetContent()
    {
        //暂时的做法
        //读取图片信息的时候,可能发生'\0'截断了,导致前端显示不出来
        std::string content;
        std::ifstream in(_uri);
        if (in.is_open() == false)
            return std::string();
        std::string line;
        while (std::getline(in, line))
        {
            content += line;
        }
        in.close();

        return content;
    }
    void Print()
    {

        std::cout << "_method: " << _method << std::endl;
        std::cout << "_uri: " << _uri << std::endl;
        std::cout << "_version: " << _version << std::endl;

        for (auto &[key, value] : map)
        {
            std::cout << key << "#: " << value << std::endl;
        }

        std::cout << " _body: " << _body << std::endl;
    }

    std::string GetUrl()
    {
        return _uri;
    }
    void SetUrl(const std::string number)
    {
        _uri = number;
    }

    std::string Suffix()
    {
        //uri ->wwwroot/index.html wwwroot/image/q.jpg wwwroot/login.html
        auto pos=_uri.rfind(".");
        if(pos==std::string::npos)   return std::string(".html");
        else    return _uri.substr(pos);
    }





    ~HttpRequest() {}

private:
    void PraseRequestLine(std::string &_req_line, const std::string sep)
    {
        std::stringstream ss(_req_line);
        ss >> _method >> _uri >> _version;
        _uri = DefaultHomePath + _uri;
    }
    bool ParseHeaderKv(std::vector<std::string> &_req_header)
    {
        for (auto &header : _req_header)
        {
            // Key:value
            std::string key;
            std::string value;
            if (SplitString(header, HeaderLineSep, &key, &value))
            {
                map[key] = value;
            }
        }
        return true;
    }
    bool ParseHeader(std::string &request_str)
    {
        std::string line;
        while (true)
        {
            bool ret = PraseOneLine(request_str, &line, Sep);

            if (ret && !line.empty())
            {
                _req_header.emplace_back(line);
            }
            else if (ret && line.empty())
            {
                _blank_line = "\r\n";
                break;
            }
            else
            {
                return false;
            }
        }

        ParseHeaderKv(_req_header);

        return true;
    }

private:
    std::string _req_line;                // 请求行
    std::vector<std::string> _req_header; // 请求报头
    std::string _blank_line;              // 空行
    std::string _body;                    // 请求正文

    // 请求行里面的字段 : 方法 [空格] uri [空格] 版本 [换行符]
    std::string _method;                              // 请求行的方法字段
    std::string _uri;                                 // 请求行的uri路径字段  用户需要的
    std::string _version;                             // 请求行的http版本字段
    std::unordered_map<std::string, std::string> map; // 用来记录请求报头里面的属性
};

// 对于http，任何的请求，都必须要有应答，没有正文的可以
class HttpResponse
{
public:
    HttpResponse()
        : _version(http_version),
          _blank_line(BlankLine)
    {
    }
    void Build(HttpRequest &req)
    {
        std::string uri=req.GetUrl();
        if(uri.back()=='/') // wwwroot/ wwwroot/a/b/
        {
            uri+=firstpage;
            req.SetUrl(uri);
        }

        // 首先打开用户指定路径的文件，如果文件为空就判断
        _content = req.GetContent();
        if (_content.empty())
        {
            // 用户请求资源的不存在
            // 1、首先设置状态码为404
            _status_code = 404;
            // 2、将用户的uri改成我们的404.html页面的路径
            req.SetUrl(page404);
            // 4、返回404.html的内容
            _content = req.GetContent();
        }
        else
        {
            // 请求的资源是存在的

            _status_code = 200;
        }
        _status_desc = CodeToDesc(_status_code);
        if(!_content.empty())
        {
            SetHeader("Content-Length",std::to_string(_content.size()));
        }
        std::string mime_type=SuffixToDesc(req.Suffix());
        SetHeader("Cotent_Type",mime_type);

        for(auto&[k,v]:_header_kv)
        {
            _resp_header.emplace_back(k+HeaderLineSep+v);
        }
    }

    void Serialize(std::string *resp_str)
    {
        // 首先构建的状态行
        std::string _resp_line = _version + LineSep + std::to_string(_status_code) +LineSep + _status_desc + Sep;

        *resp_str = _resp_line;
        for (auto &e : _resp_header)
        {
            *resp_str += e + Sep;
        }
        *resp_str += _blank_line;
        _body = _content;
        *resp_str += _body;
    }
    ~HttpResponse() {}
    void SetHeader(const std::string &k,const std::string &v)
    {
        _header_kv[k]=v;
    }
private:
    std::string CodeToDesc(int code)
    {
        switch (code)
        {
        case 200:
            return ("OK");
        case 404:
            return ("Not Found");
        default:
            return std::string();
        }
    }

    std::string SuffixToDesc(const std::string &suffix)
    {
        if(suffix==".html")
            return "text/html";
        else if(suffix==".jpg")
            return "application/x-jpg";
        else
            return "text/html";
    }

private:
    // 构成下面的四要素，所需要的元素
    std::string _version;
    int _status_code;
    std::string _status_desc;
    std::string _content;
    std::unordered_map<std::string ,std::string > _header_kv;
    // 最终需要以下的四部分
    std::string _resp_line;                // 请求行
    std::vector<std::string> _resp_header; // 请求报头
    std::string _blank_line;               // 空行
    std::string _body;                     // 请求正文
};
