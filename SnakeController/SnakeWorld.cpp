#include "SnakeWorld.hpp"
#include "SnakeInterface.hpp"

namespace Snake {

SnakeWorld::SnakeWorld(std::istream &istr)
{
    char w, f, s;

    int width, height;
    int foodX, foodY;
    istr >> w >> width >> height >> f >> foodX >> foodY >> s;

    if (w != 'W' or f != 'F' or s != 'S') {
        throw ConfigurationError();
    }
    m_mapDimension = std::make_pair(width, height);
    m_foodPosition = std::make_pair(foodX, foodY);
}


bool SnakeWorld::isPositionOutsideMap(Coordinates position) const
{
    auto [x, y] = position;
    return x < 0 or y < 0 or x >= m_mapDimension.first or y >= m_mapDimension.second;
}

} // namespace Snake
