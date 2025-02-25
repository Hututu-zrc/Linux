#pragma once
#include <functional>
#include<iostream>
using task_t=std::function<void()>;


void Push()
{
    std::cout<<"this is a task"<<std::endl;
}