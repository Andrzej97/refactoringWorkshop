#include "SnakeController.hpp"

#include <algorithm>
#include <sstream>

#include "EventT.hpp"
#include "IPort.hpp"

namespace Snake
{
ConfigurationError::ConfigurationError()
    : std::logic_error("Bad configuration of Snake::Controller.")
{}

UnexpectedEventException::UnexpectedEventException()
    : std::runtime_error("Unexpected event received!")
{}

SnakeController::SnakeController(IPort& p_displayPort, IPort& p_foodPort, IPort& p_scorePort, std::string const& p_config)
    : m_displayPort(p_displayPort),
      m_foodPort(p_foodPort),
      m_scorePort(p_scorePort)
{
    std::istringstream istr(p_config);
    char w, f, s, d;

    int width, height, length;
    int foodX, foodY;
    istr >> w >> width >> height >> f >> foodX >> foodY >> s;

    if (w == ConfigSet::width and f == ConfigSet::food and s == ConfigSet::snake) {
        m_mapDimension = std::make_pair(width, height);
        m_foodPosition = std::make_pair(foodX, foodY);

        istr >> d;
        switch (d) {
            case DirectionSet::up:
                m_currentDirection = Direction_UP;
                break;
            case DirectionSet::down:
                m_currentDirection = Direction_DOWN;
                break;
            case DirectionSet::left:
                m_currentDirection = Direction_LEFT;
                break;
            case DirectionSet::right:
                m_currentDirection = Direction_RIGHT;
                break;
            default:
                throw ConfigurationError();
        }
        istr >> length;

        while (length--) {
            Segment seg;
            istr >> seg.x >> seg.y;
            m_segments.push_back(seg);
        }
    } else {
        throw ConfigurationError();
    }
}

bool SnakeController::isSegmentAtPosition(int x, int y) const
{
    return m_segments.end() !=  std::find_if(m_segments.cbegin(), m_segments.cend(),
        [x, y](auto const& segment){ return segment.x == x and segment.y == y; });
}

bool SnakeController::isPositionOutsideMap(int x, int y) const
{
    return x < 0 or y < 0 or x >= m_mapDimension.first or y >= m_mapDimension.second;
}

void SnakeController::sendPlaceNewFood(int x, int y)
{
    m_foodPosition = std::make_pair(x, y);

    World::DisplayInd placeNewFood;
    placeNewFood.x = x;
    placeNewFood.y = y;
    placeNewFood.value = World::Cell_FOOD;

    m_displayPort.send(std::make_unique<EventT<World::DisplayInd>>(placeNewFood));
}

void SnakeController::sendClearOldFood()
{
    World::DisplayInd clearOldFood;
    clearOldFood.x = m_foodPosition.first;
    clearOldFood.y = m_foodPosition.second;
    clearOldFood.value = World::Cell_FREE;

    m_displayPort.send(std::make_unique<EventT<World::DisplayInd>>(clearOldFood));
}

namespace
{
bool isHorizontal(Direction direction)
{
    return Direction_LEFT == direction or Direction_RIGHT == direction;
}

bool isVertical(Direction direction)
{
    return Direction_UP == direction or Direction_DOWN == direction;
}

bool isPositive(Direction direction)
{
    return (isVertical(direction) and Direction_DOWN == direction)
        or (isHorizontal(direction) and Direction_RIGHT == direction);
}

bool perpendicular(Direction dir1, Direction dir2)
{
    return isHorizontal(dir1) == isVertical(dir2);
}
} // namespace

Segment SnakeController::calculateNewHead() const
{
    Segment const& currentHead = m_segments.front();

    Segment newHead;
    newHead.x = currentHead.x + (isHorizontal(m_currentDirection) ? isPositive(m_currentDirection) ? 1 : -1 : 0);
    newHead.y = currentHead.y + (isVertical(m_currentDirection) ? isPositive(m_currentDirection) ? 1 : -1 : 0);

    return newHead;
}

void SnakeController::removeTailSegment()
{
    auto tail = m_segments.back();

    World::DisplayInd l_evt;
    l_evt.x = tail.x;
    l_evt.y = tail.y;
    l_evt.value = World::Cell_FREE;
    m_displayPort.send(std::make_unique<EventT<World::DisplayInd>>(l_evt));

    m_segments.pop_back();
}

void SnakeController::addHeadSegment(Segment const& newHead)
{
    m_segments.push_front(newHead);

    World::DisplayInd placeNewHead;
    placeNewHead.x = newHead.x;
    placeNewHead.y = newHead.y;
    placeNewHead.value = World::Cell_SNAKE;

    m_displayPort.send(std::make_unique<EventT<World::DisplayInd>>(placeNewHead));
}

void SnakeController::removeTailSegmentIfNotScored(Segment const& newHead)
{
    if (std::make_pair(newHead.x, newHead.y) == m_foodPosition) {
        m_scorePort.send(std::make_unique<EventT<ScoreInd>>());
        m_foodPort.send(std::make_unique<EventT<World::FoodReq>>());
    } else {
        removeTailSegment();
    }
}

void SnakeController::updateSegmentsIfSuccessfullMove(Segment const& newHead)
{
    if (isSegmentAtPosition(newHead.x, newHead.y) or isPositionOutsideMap(newHead.x, newHead.y)) {
        m_scorePort.send(std::make_unique<EventT<LooseInd>>());
    } else {
        addHeadSegment(newHead);
        removeTailSegmentIfNotScored(newHead);
    }
}

void SnakeController::handleTimeoutInd()
{
    updateSegmentsIfSuccessfullMove(calculateNewHead());
}

void SnakeController::handleDirectionInd(std::unique_ptr<Event> e)
{
    auto direction = payload<DirectionInd>(*e).direction;

    if (perpendicular(m_currentDirection, direction)) {
        m_currentDirection = direction;
    }
}

void SnakeController::updateFoodPosition(int x, int y, std::function<void()> clearPolicy)
{
    if (isSegmentAtPosition(x, y) or isPositionOutsideMap(x, y)) {
        m_foodPort.send(std::make_unique<EventT<World::FoodReq>>());
        return;
    }

    clearPolicy();
    sendPlaceNewFood(x, y);
}

void SnakeController::handleFoodInd(std::unique_ptr<Event> e)
{
    auto receivedFood = payload<World::FoodInd>(*e);

    updateFoodPosition(receivedFood.x, receivedFood.y, std::bind(&SnakeController::sendClearOldFood, this));
}

void SnakeController::handleFoodResp(std::unique_ptr<Event> e)
{
    auto requestedFood = payload<World::FoodResp>(*e);

    updateFoodPosition(requestedFood.x, requestedFood.y, []{});
}

void SnakeController::handlePauseInd()
{
    m_paused = not m_paused;
}

void SnakeController::receive(std::unique_ptr<Event> e)
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
        case World::FoodInd::MESSAGE_ID:
            return handleFoodInd(std::move(e));
        case World::FoodResp::MESSAGE_ID:
            return handleFoodResp(std::move(e));
        case PauseInd::MESSAGE_ID:
            return handlePauseInd();
        default:
            throw UnexpectedEventException();
    }
}

} // namespace Snake
