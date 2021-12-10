#include "SnakeSegments.hpp"

#include <algorithm>

namespace Snake
{

SnakeSegments::SnakeSegments()
{

}

bool SnakeSegments::isSegmentAtPosition(Segment const& seg) const
{
    return m_segments.end() !=  std::find_if(m_segments.cbegin(), m_segments.cend(),
        [seg](auto const& segment){ return segment.x == seg.x and segment.y == seg.y; });
}

Segment SnakeSegments::calculateNewHead(Direction m_currentDirection) const
{
    Segment const& currentHead = m_segments.front();

    Segment newHead;
    newHead.x = currentHead.x + (isHorizontal(m_currentDirection) ? isPositive(m_currentDirection) ? 1 : -1 : 0);
    newHead.y = currentHead.y + (isVertical(m_currentDirection) ? isPositive(m_currentDirection) ? 1 : -1 : 0);

    return newHead;
}

Segment SnakeSegments::getTailSegment()
{
    return m_segments.back();
}

void SnakeSegments::removeTailSegment()
{
    m_segments.pop_back();
}

void SnakeSegments::addHeadSegment(Segment const& newHead)
{
    m_segments.push_front(newHead);
}
void SnakeSegments::addTailSegment(Segment const& newHead)
{
    m_segments.push_back(newHead);
}

}
