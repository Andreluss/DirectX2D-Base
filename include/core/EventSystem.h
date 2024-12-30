#pragma once
#include "common.h"
#include <functional>

template<class MessageType>
class EventSystem
{
public:
    void Subscribe(void* this_ptr, std::function<void(MessageType)> subscriber)
    {
        subscribers.push_back({ this_ptr, subscriber });
    }
    void Unsubscribe(void* this_ptr) 
    {
        std::erase_if(subscribers, [this_ptr](auto& subscriber) {
            return subscriber.first == this_ptr;
        });
    }
    void Publish(MessageType message)
    {
        for (auto& [_, subscriber] : subscribers)
        {
            subscriber(message);
        }
    }
private:
    std::vector<std::pair<void*, std::function<void(MessageType)>>> subscribers;
};

// template variable:
template<class MessageType>
EventSystem<MessageType> event_system;
