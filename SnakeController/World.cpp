#include "World.hpp"
#include "Directions.hpp"


namespace Snake{
    WorldSpace::WorldSpace(){

    }

    bool WorldSpace::isPositionOutsideMap(int x, int y) const
    {
        return x < 0 || y < 0 || x >= m_mapDimension.first || y >= m_mapDimension.second;
    }

    void WorldSpace::updateFoodPos(int x, int y)
    {
        PlaceNewFood(x, y);
    }

    Snake::DisplayInd WorldSpace::ClearOldFood()
    {
        DisplayInd clearOldFood;
        clearOldFood.x = m_foodPosition.first;
        clearOldFood.y = m_foodPosition.second;
        clearOldFood.value = Cell_FREE;
        return clearOldFood;
    }

    Snake::DisplayInd WorldSpace::PlaceNewFood(int x, int y)
    {
        m_foodPosition = std::make_pair(x, y);

        DisplayInd placeNewFood;
        placeNewFood.x = x;
        placeNewFood.y = y;
        placeNewFood.value = Cell_FOOD;

        return placeNewFood;
    }
}