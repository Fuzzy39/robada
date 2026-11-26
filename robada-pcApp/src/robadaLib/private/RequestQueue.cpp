#include <private/RequestQueue.h>

using namespace robada;

RequestQueue::RequestQueue() : lock(), requests()
{
    std::thread th(&RequestQueue::run, this);
    th.detach();

}

void RequestQueue::run()
{
    while(true)
    {
        
        lock.lock();
        if(requests.size()!=0) 
        {
            Servicable& request = *(requests.front());
            lock.unlock();
            // service the request while unlocked so as to not block new entries being added to the queue.
            request.serviceRequest();

            lock.lock();
            requests.pop();
            lock.unlock();
            continue;
        }

        lock.unlock();
    }

  
}
