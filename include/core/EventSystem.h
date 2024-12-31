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
        // loop manually witih indices to prevent invalidated operator access
        for (int i = 0; i < subscribers.size(); i++) {
            auto& [_, subscriber] = subscribers[i];
            subscriber(message);
        }
    }
private:
    std::vector<std::pair<void*, std::function<void(MessageType)>>> subscribers;
};

// template variable:
template<class MessageType>
EventSystem<MessageType> event_system;

// Safe RAII subscription wrapper.
template<class MessageType>
class EventSubscription
{
public:
    EventSubscription(std::function<void(MessageType)> subscriber)
    {
        event_system<MessageType>.Subscribe(this, subscriber);
    }
    ~EventSubscription()
    {
        event_system<MessageType>.Unsubscribe(this);
    }
};
