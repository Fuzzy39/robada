#include <public/RobadaLib.h>
#include <chrono>
#include <thread>
#include <iostream>

using namespace robada;

std::unique_ptr<RobadaLib> RobadaLib::instance(nullptr);

RobadaLib::RobadaLib(): requestQueue() 
{
}

RobadaLib& RobadaLib::getInstance()
{
    if(instance)
    {
        return *instance;
    }

    // can't use make unique here because the constructor is private.
    instance = std::unique_ptr<RobadaLib>(new RobadaLib());
    return *instance;
}

std::future<void> RobadaLib::sleep()
{
    return requestQueue.submitRequest<void>(
        []()->void {
            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
            
        });
}