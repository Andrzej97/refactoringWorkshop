#include "SnakeMap.hpp"

namespace Snake
{

SnakeMap::SnakeMap()
{

}
void SnakeMap::setDimension(std::pair<int,int> dimension)
{
    m_mapDimension = dimension;
}
void SnakeMap::setFoodPosition(Segment position)
{
    m_foodPosition = position;
}
bool SnakeMap::isPositionOutsideMap(Segment const& seg) const
{
    return seg.x < 0 or seg.y < 0 or seg.x >= m_mapDimension.first or seg.y >= m_mapDimension.second;
}
Segment SnakeMap::getFoodPosition()
{
    return m_foodPosition;
}

bool SnakeMap::eatFood(Segment newHead)
{
return (newHead.x == m_foodPosition.x and newHead.y == m_foodPosition.y);
}
}
