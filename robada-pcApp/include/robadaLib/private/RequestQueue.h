#pragma once
#include <queue>
#include <mutex>
#include <any>
#include <functional>
#include <future>

// OLD COMMENT:
// this is a big singleton, you initialize it, it spawns a thread.
// It will be a command queue-type thing. It runs a command if available,
// reads from robada's console, if that existed, and polls the gamepad each iteration.

// Need to figure out a good way to do async. That'll be the other headers in this, the actual commands.
// Probably will have a state machine of sorts for robada control modes. That could be represented by classes, actually.
namespace robada 
{

    // This class represents a thread that runs functions on request. other threads can submit requests for functions to be run.
    class RequestQueue
    {
    private:
        class Servicable
        {
        public:
           virtual void serviceRequest() = 0;  
        };

        // request classes. A request represents a function call that will be made on the requestQueue's thread.
        template <typename returnType>
        class RequestBase : public Servicable
        {
        protected:
            std::promise<returnType> promise;
        public:
            RequestBase();
            std::future<returnType> getFuture();
        };
        
        // a request with no parameters
        template <typename returnType>
        class Request : public RequestBase<returnType>
        {
        private:
            const std::function<returnType()> function;
        public:
            Request(std::function<returnType()> func);
            virtual void serviceRequest() override;
            
        };

        // a request with a single parameter
        template <typename returnType, typename paramType>
        class ParamRequest : public RequestBase<returnType>
        {
        private:
            const paramType parameter;
            const std::function<returnType(paramType)> function;
        public:
            ParamRequest(std::function<returnType()> func, paramType param);
            virtual void serviceRequest() override;
        };


        // class members
    private:
        std::mutex lock; // protects the queue.
        std::queue<std::unique_ptr<Servicable>> requests; // not any, it's a RequestBase. But I can't figure out how to specify that.
    public:
        RequestQueue(); 

        // pump the queue. Never returns.
        void run();

        
        template <typename returnType>
        std::future<returnType> submitRequest(std::function<returnType()> func);

        template <typename returnType, typename paramType>
        std::future<returnType> submitRequest(std::function<returnType(paramType)> func, paramType param);

    };
}

#include <private/Request.ipp>
#include <private/RequestQueue.ipp>
