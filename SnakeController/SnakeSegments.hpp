#pragma once

#include <list>
#include <memory>
#include <functional>

#include "IEventHandler.hpp"
#include "SnakeInterface.hpp"
#include <stdexcept>

namespace Snake
{

struct Segment
{
    int x;
    int y;
};

} // namespace Snake
