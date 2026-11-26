#pragma once
#include <private/RequestQueue.h>
#include <functional>
#include <iostream>

using namespace robada;

// RequestBase
template <typename returnType>
RequestQueue::RequestBase<returnType>::RequestBase(): promise() {}

template <typename returnType>
std::future<returnType> RequestQueue::RequestBase<returnType>::getFuture()
{
    return promise.get_future();
}

// Request
template <typename returnType>
RequestQueue::Request<returnType>::Request(std::function<returnType()> func)
: RequestQueue::RequestBase<returnType>(), function(func){
}

template <typename returnType>
void RequestQueue::Request<returnType>::serviceRequest()
{
    try
    {
        if constexpr (std::is_same_v<returnType, void>) // cursed nonsesne.
        {
            function();
            RequestBase<returnType>::promise.set_value();
        }
        else
        {
            RequestBase<returnType>::promise.set_value(function());
        }
    }
    catch(...)
    {
        RequestBase<returnType>::promise.set_exception(std::current_exception());
    }
    
}

template <typename returnType, typename paramType>
RequestQueue::ParamRequest<returnType, paramType>::ParamRequest(std::function<returnType()> func, paramType param)
: RequestQueue::RequestBase<returnType>(), function(func), parameter(param){
}

template <typename returnType, typename paramType>
void RequestQueue::ParamRequest<returnType, paramType>::serviceRequest()
{
    try
    {
        if constexpr (std::is_same_v<returnType, void>) // cursed nonsesne.
        {
            function(parameter);
            RequestBase<returnType>::promise.set_value();
        }
        else
        {
            RequestBase<returnType>::promise.set_value(function(parameter));
        }
    }
    catch(...)
    {
        RequestBase<returnType>::promise.set_exception(std::current_exception());
    }
    
}
