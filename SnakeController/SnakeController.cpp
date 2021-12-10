#include "SnakeController.hpp"

#include <algorithm>
#include <sstream>

#include "EventT.hpp"
#include "IPort.hpp"

namespace Snake
{

Controller::Controller(IPort& p_displayPort, IPort& p_foodPort, IPort& p_scorePort, std::string const& p_config)
    : m_displayPort(p_displayPort),
      m_foodPort(p_foodPort),
      m_scorePort(p_scorePort),
      m_paused(false)
{
    std::istringstream istr(p_config);
    m_snakeWorld = std::make_unique<SnakeWorld>(istr);
    m_snakeBody  = std::make_unique<SnakeBody>(istr);
}


void Controller::sendPlaceNewFood(Coordinates position)
{
    m_snakeWorld->setFoodPosition(position);
    auto [x,y] = position;
    DisplayInd placeNewFood;
    placeNewFood.x = x;
    placeNewFood.y = y;
    placeNewFood.value = Cell_FOOD;

    m_displayPort.send(std::make_unique<EventT<DisplayInd>>(placeNewFood));

}

void Controller::sendClearOldFood()
{

    auto foodPosition = m_snakeWorld->getFoodPosition();
    auto [x,y] = foodPosition;
    DisplayInd clearOldFood;
    clearOldFood.x = x;
    clearOldFood.y = y;
    clearOldFood.value = Cell_FREE;

    m_displayPort.send(std::make_unique<EventT<DisplayInd>>(clearOldFood));
}

void Controller::removeTailSegment()
{
    auto tail = m_snakeBody->popTailSegment();
    auto [x,y] = tail.position;
    DisplayInd l_evt;
    l_evt.x = x;
    l_evt.y = y;
    l_evt.value = Cell_FREE;
    m_displayPort.send(std::make_unique<EventT<DisplayInd>>(l_evt));

}

void Controller::addHeadSegment(SnakeBody::Segment const& newHead)
{
    m_snakeBody->setNewHead(newHead);

    auto [x,y] = newHead.position;
    DisplayInd placeNewHead;
    placeNewHead.x = x;
    placeNewHead.y = y;
    placeNewHead.value = Cell_SNAKE;

    m_displayPort.send(std::make_unique<EventT<DisplayInd>>(placeNewHead));
}

void Controller::removeTailSegmentIfNotScored(SnakeBody::Segment const& newHead)
{
    if (newHead.position == m_snakeWorld->getFoodPosition()) {
        m_scorePort.send(std::make_unique<EventT<ScoreInd>>());
        m_foodPort.send(std::make_unique<EventT<FoodReq>>());
    } else {
        removeTailSegment();
    }
}

void Controller::updateSegmentsIfSuccessfullMove(SnakeBody::Segment const& newHead)
{
    if (    m_snakeBody->isSegmentAtPosition(newHead.position)
         or m_snakeWorld->isPositionOutsideMap(newHead.position) ) {
        m_scorePort.send(std::make_unique<EventT<LooseInd>>());
    } else {
        addHeadSegment(newHead);
        removeTailSegmentIfNotScored(newHead);
    }
}

void Controller::handleTimeoutInd()
{
    updateSegmentsIfSuccessfullMove(m_snakeBody->calculateNewHead());
}

void Controller::handleDirectionInd(std::unique_ptr<Event> e)
{
    auto direction = payload<DirectionInd>(*e).direction;
    m_snakeBody->changeDirection(direction);
}

void Controller::updateFoodPosition(Coordinates foodPosition, std::function<void()> clearPolicy)
{

    if (m_snakeWorld->isPositionOutsideMap(foodPosition) or m_snakeBody->isSegmentAtPosition(foodPosition)) {
        m_foodPort.send(std::make_unique<EventT<FoodReq>>());
        return;
    }

    clearPolicy();
    sendPlaceNewFood(foodPosition);
}

void Controller::handleFoodInd(std::unique_ptr<Event> e)
{
    auto receivedFood = payload<FoodInd>(*e);
    Coordinates foodPosition = {receivedFood.x, receivedFood.y};

    updateFoodPosition(foodPosition, std::bind(&Controller::sendClearOldFood, this));
}

void Controller::handleFoodResp(std::unique_ptr<Event> e)
{
    auto requestedFood = payload<FoodResp>(*e);
    Coordinates foodPosition = {requestedFood.x, requestedFood.y};

    updateFoodPosition(foodPosition, []{});
}

void Controller::handlePauseInd(std::unique_ptr<Event> e)
{
    m_paused = not m_paused;
}

void Controller::receive(std::unique_ptr<Event> e)
{
    switch (e->getMessageId()) {
        case TimeoutInd::MESSAGE_ID:
            if (!m_paused) {
                return handleTimeoutInd();
            }
            return;
        case DirectionInd::MESSAGE_ID:
            if (!m_paused) {
                return handleDirectionInd(std::move(e));
            }
            return;
        case FoodInd::MESSAGE_ID:
            return handleFoodInd(std::move(e));
        case FoodResp::MESSAGE_ID:
            return handleFoodResp(std::move(e));
        case PauseInd::MESSAGE_ID:
            return handlePauseInd(std::move(e));
        default:
            throw UnexpectedEventException();
    }
}

} // namespace Snake
