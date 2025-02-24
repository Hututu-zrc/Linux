#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include "Common.hpp"

const std::string DICT_PATH = "./";
const std::string DICT_FILENAME = "dict.txt";
class Dictionary
{
private:
    void LoadDictionary()
    {
        std::ifstream ifs(_path + _filename);
        if (!ifs.is_open())
        {
            std::cerr << "open file " << DICT_PATH + DICT_FILENAME << " failed" << std::endl;
            exit(1);
        }
        std::string line;
        std::string key;
        std::string value;
        size_t pos;
        while (getline(ifs, line))
        {
            pos = SplitString(line);
            if (pos == std::string::npos)
            {
                std::cerr << "dictionary file format error" << std::endl;
                exit(1);
            }
            key = line.substr(0, pos);
            value = line.substr(pos + 1);
            _dict[key] = value;
        }
        {
            _dict[key] = value;
        }
        ifs.close();
    }

public:
    Dictionary(const std::string &path=DICT_PATH, const std::string &filename=DICT_FILENAME)
    : _path(path), _filename(filename)
    {
        LoadDictionary();
    }
    std::string Translate(const std::string &word)
    {   
         auto it = _dict.find(word);
       if(it== _dict.end())
       {
           return "NONE";
       }
       return it->second;
    }
    ~Dictionary() {}

private:
    std::string _path;
    std::string _filename;

    std::unordered_map<std::string, std::string> _dict;
};
