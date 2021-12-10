#pragma once
#include <list>
#include <memory>
#include <functional>
#include <sstream>
#include "SnakeInterface.hpp"

namespace Snake {
class SnakeWorld {

  public:

    SnakeWorld(std::istream &istr);

    void setFoodPosition(Coordinates position)
    {
        m_foodPosition = position;
    }

    Coordinates getFoodPosition() const
    {
        return m_foodPosition;
    }

    bool isPositionOutsideMap(Coordinates position) const;

  private:
        Coordinates m_mapDimension;
        Coordinates m_foodPosition;
};

} // namespace Snake
