#pragma once

#include <list>
#include <sstream>
#include <SnakeInterface.hpp>

namespace Snake
{

class SnakeBody {
public:
    struct Segment
    {
        Coordinates position;
    };

    SnakeBody(std::istream &configIstr);

    bool isSegmentAtPosition(Coordinates position) const;

    Segment calculateNewHead() const;
    Segment popTailSegment();
    void setNewHead(Segment const& headSegment);
    void changeDirection(Direction direction);

private:
    std::list<Segment> m_segments;
    Direction m_currentDirection;

};

} // namespace Snake
