#pragma once

#include <memory>
#include <functional>

#include "IEventHandler.hpp"
#include "WorldInterface.hpp"
#include <stdexcept>

class Event;
class IPort;

namespace World
{

struct ConfigurationError : std::logic_error
{
    ConfigurationError();
};

struct UnexpectedEventException : std::runtime_error
{
    UnexpectedEventException();
};

class WorldController : public IEventHandler
{
public:
    WorldController(IPort& p_displayPort, IPort& p_foodPort, IPort& p_scorePort, std::string const& p_config);

    WorldController(WorldController const& p_rhs) = delete;
    WorldController& operator=(WorldController const& p_rhs) = delete;

private:
    IPort& m_displayPort;
    IPort& m_foodPort;
    IPort& m_scorePort;

    std::pair<int, int> m_mapDimension;
    std::pair<int, int> m_foodPosition;

    void handleFoodInd(std::unique_ptr<Event>);
    void handleFoodResp(std::unique_ptr<Event>);

    bool isPositionOutsideMap(int x, int y) const;

    void updateFoodPosition(int x, int y, std::function<void()> clearPolicy);
    void sendClearOldFood();
    void sendPlaceNewFood(int x, int y);
};

}   // namespace World