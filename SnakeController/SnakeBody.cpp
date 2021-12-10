#include <algorithm>
#include "SnakeBody.hpp"

namespace Snake
{

namespace
{
bool isHorizontal(Direction direction)
{
    return Direction_LEFT == direction or Direction_RIGHT == direction;
}

bool isVertical(Direction direction)
{
    return Direction_UP == direction or Direction_DOWN == direction;
}

bool isPositive(Direction direction)
{
    return (isVertical(direction) and Direction_DOWN == direction)
        or (isHorizontal(direction) and Direction_RIGHT == direction);
}

bool perpendicular(Direction dir1, Direction dir2)
{
    return isHorizontal(dir1) == isVertical(dir2);
}
} // namespace

SnakeBody::SnakeBody(std::istream &configIstr)
{
    char direction;
    configIstr >> direction;
    switch (direction) {
        case 'U':
            m_currentDirection = Direction_UP;
            break;
        case 'D':
            m_currentDirection = Direction_DOWN;
            break;
        case 'L':
            m_currentDirection = Direction_LEFT;
            break;
        case 'R':
            m_currentDirection = Direction_RIGHT;
            break;
        default:
            throw ConfigurationError();
    }

    int length;
    configIstr >> length;

    while (length--) {
        Segment seg;
        configIstr >> seg.position.first >> seg.position.second;
        m_segments.push_back(seg);
    }
}

bool SnakeBody::isSegmentAtPosition(Coordinates position) const
{
    return m_segments.end() !=  std::find_if(m_segments.cbegin(), m_segments.cend(),
        [position](auto const& segment){ return segment.position == position; });
}


SnakeBody::Segment SnakeBody::calculateNewHead() const
{
    auto [x,y] = (m_segments.front()).position;
    x += isHorizontal(m_currentDirection) ? isPositive(m_currentDirection) ? 1 : -1 : 0;
    y += isVertical(m_currentDirection) ? isPositive(m_currentDirection) ? 1 : -1 : 0;

    Segment newHead;
    newHead.position = {x,y};

    return newHead;
}

SnakeBody::Segment SnakeBody::popTailSegment()
{
    Segment tail;
    tail = m_segments.back();
    m_segments.pop_back();
    return tail;
}

void SnakeBody::setNewHead(Segment const& headSegment)
{
    m_segments.push_front(headSegment);
}


void SnakeBody::changeDirection(Direction direction)
{
    if (perpendicular(m_currentDirection, direction)) {
        m_currentDirection = direction;
    }
}


} // namespace Snake
