#pragma once
#include <private/RequestQueue.h>

using namespace robada;


// no parameters
template <typename returnType>
std::future<returnType> RequestQueue::submitRequest(std::function<returnType()> func)
{
    std::unique_lock<std::mutex> ul(lock);
    
    requests.push(std::make_unique<Request<returnType>>(func));
    RequestQueue::Servicable& servicable = *requests.back();
    RequestQueue::Request<returnType>& request = static_cast<RequestQueue::Request<returnType>&>(servicable);
    return request.getFuture();
    
}

// single parameter
template <typename returnType, typename paramType>
std::future<returnType> RequestQueue::submitRequest(
    std::function<returnType(paramType)> func, 
    paramType param
)
{
    std::unique_lock<std::mutex> ul(lock);
    requests.push(std::make_unique<ParamRequest<returnType, paramType>>(func, param));
    return static_cast<RequestQueue::ParamRequest<returnType, paramType>>( *(requests.back())).getFuture();
}

