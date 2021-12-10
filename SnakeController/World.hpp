#pragma once

#include <list>
#include <functional>
#include "SnakeInterface.hpp"

namespace Snake{
    class WorldSpace{
        public:
            WorldSpace();
            bool isPositionOutsideMap(int x, int y) const;
            void updateFoodPos(int x, int y);
            Snake::DisplayInd ClearOldFood();
            Snake::DisplayInd PlaceNewFood(int x, int y);
            std::pair<int, int> m_mapDimension;
            std::pair<int, int> m_foodPosition;
    };
}