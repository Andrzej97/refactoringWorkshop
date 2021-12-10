#pragma once

#include <list>
#include <memory>
#include <functional>

#include "IEventHandler.hpp"
#include "SnakeInterface.hpp"
#include <stdexcept>

class Event;
class IPort;

namespace Snake
{
struct ConfigurationError : std::logic_error
{
    ConfigurationError();
};

struct UnexpectedEventException : std::runtime_error
{
    UnexpectedEventException();
};

class Controller : public IEventHandler
{
public:
    Controller(IPort& p_displayPort, IPort& p_foodPort, IPort& p_scorePort, std::string const& p_config);

    Controller(Controller const& p_rhs) = delete;
    Controller& operator=(Controller const& p_rhs) = delete;

    void receive(std::unique_ptr<Event> e) override;

private:
    IPort& m_displayPort;
    IPort& m_foodPort;
    IPort& m_scorePort;

    Ind m_mapDimension;
    Ind m_foodPosition;

    std::list<Ind> m_segments;
    Direction m_currentDirection;

    void handleTimeoutInd();
    void handleDirectionInd(std::unique_ptr<Event>);
    void handleFoodInd(std::unique_ptr<Event>);
    void handleFoodResp(std::unique_ptr<Event>);
    void handlePauseInd(std::unique_ptr<Event>);

    bool isSegmentAtPosition(Ind cord) const;
    Ind& calculateNewHead() const;
    void updateSegmentsIfSuccessfullMove(Ind const& newHead);
    void addHeadSegment(Ind const& newHead);
    void removeTailSegmentIfNotScored(Ind const& newHead);
    void removeTailSegment();

    bool isPositionOutsideMap(Ind cord) const;

    void updateFoodPosition(Ind& cord, std::function<void()> clearPolicy);
    void sendClearOldFood();
    void sendPlaceNewFood(Ind& cord);

    bool m_paused;
};

} // namespace Snake
