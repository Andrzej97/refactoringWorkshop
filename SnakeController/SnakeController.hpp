#pragma once

#include <list>
#include <memory>
#include <functional>

#include "IEventHandler.hpp"
#include "SnakeInterface.hpp"
#include "SnakeWorld.hpp"
#include "SnakeBody.hpp"


class Event;
class IPort;

namespace Snake
{

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
    bool m_paused;

    std::unique_ptr<SnakeWorld> m_snakeWorld;
    std::unique_ptr<SnakeBody>  m_snakeBody;


    void handleTimeoutInd();
    void handleDirectionInd(std::unique_ptr<Event>);
    void handleFoodInd(std::unique_ptr<Event>);
    void handleFoodResp(std::unique_ptr<Event>);
    void handlePauseInd(std::unique_ptr<Event>);

    void updateSegmentsIfSuccessfullMove(SnakeBody::Segment const& newHead);
    void addHeadSegment(SnakeBody::Segment const& newHead);
    void removeTailSegmentIfNotScored(SnakeBody::Segment const& newHead);
    void removeTailSegment();

    void updateFoodPosition(Coordinates position, std::function<void()> clearPolicy);
    void sendClearOldFood();
    void sendPlaceNewFood(Coordinates position);

};

} // namespace Snake
