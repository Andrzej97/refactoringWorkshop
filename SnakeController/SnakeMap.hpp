#include <map>
#include "SnakeInterface.hpp"

namespace Snake
{
class SnakeMap
{
public:
    SnakeMap();

    void setDimension(std::pair<int,int> dimension);
    void setFoodPosition(Segment position);
    bool isPositionOutsideMap(Segment const& seg) const;
    Segment getFoodPosition();
    bool eatFood(Segment newHead);
private:
    std::pair<int, int> m_mapDimension;
    Segment m_foodPosition;
};
}
