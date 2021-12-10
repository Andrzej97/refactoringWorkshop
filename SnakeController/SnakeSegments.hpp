
#include "SnakeInterface.hpp"
#include <list>

namespace Snake
{

class SnakeSegments
{
public:
    bool isSegmentAtPosition(Segment const& newHead) const;
    Segment calculateNewHead(Direction m_currentDirection) const;
    void removeTailSegment();
    Segment getTailSegment();
    void addHeadSegment(Segment const& newHead);
    void addTailSegment(Segment const& newHead);


    SnakeSegments();
private:
    std::list<Segment> m_segments;
};

}
