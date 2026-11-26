#pragma once
#include <memory>
#include <private/RequestQueue.h>

namespace robada
{
    // Singleton class that represents the library as a whole.
    class RobadaLib
    {
    private:
        static std::unique_ptr<RobadaLib> instance;
        RequestQueue requestQueue;

    private:
        RobadaLib();
    public:
        static RobadaLib& getInstance();

        // test function.
        std::future<void> sleep();

    };
}