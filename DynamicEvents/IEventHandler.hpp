#pragma once

#include <memory>

class Event;

class IEventHandler
{
public:
    virtual ~IEventHandler() = default;
    virtual void receive(std::unique_ptr<Event>) = 0;
    virtual void change() = 0;
    virtual void requested(std::unique_ptr<Event> e) = 0;
};
