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

    Point m_mapDimension;
    Point m_foodPosition;

    std::list<Point> m_segments;
    Direction m_currentDirection;

    void handleTimeoutInd();
    void handleDirectionInd(std::unique_ptr<Event>);
    void handleFoodInd(std::unique_ptr<Event>);
    void handleFoodResp(std::unique_ptr<Event>);
    void handlePauseInd();

    bool isSegmentAtPosition(Point cord) const;
    Point calculateNewHead() const;
    void updateSegmentsIfSuccessfullMove(Point const& newHead);
    void addHeadSegment(Point const& newHead);
    void removeTailSegmentIfNotScored(Point const& newHead);
    void removeTailSegment();

    bool isPositionOutsideMap(Point cord) const;

    void updateFoodPosition(Point& cord, std::function<void()> clearPolicy);
    void sendClearOldFood();
    void sendPlaceNewFood(Point& cord);

    bool m_paused;
};

} // namespace Snake
