#pragma once
#include "SnakeInterface.hpp"

namespace Snake{
    bool isHorizontal(Direction direction);
    bool isVertical(Direction direction);
    bool isPositive(Direction direction);
    bool perpendicular(Direction dir1, Direction dir2);
}