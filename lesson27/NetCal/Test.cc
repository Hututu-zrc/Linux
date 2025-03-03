#include "Protocol.hpp"


int main()
{
    Request req(100,200,'+');
    std::string s;
    req.Serialize(s);
    std::cout<<s<<std::endl;

    req.Deserialize(s);
    req.Print();
    return 0;
}