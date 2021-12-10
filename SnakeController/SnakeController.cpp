
#include "SnakeController.hpp"

#define constW 'W'
#define constF 'F'
#define constS 'S'
#define constU 'U'
#define constD 'D'
#define constL 'L'
#define constR 'R'

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

Controller::Controller(IPort& p_displayPort, IPort& p_foodPort, IPort& p_scorePort, std::string const& p_config)
    : m_displayPort(p_displayPort),
      m_foodPort(p_foodPort),
      m_scorePort(p_scorePort),
      m_paused(false)
{
    std::istringstream istr(p_config);
    char w, f, s, d;

    int width, height, length;
    int foodX, foodY;
    istr >> w >> width >> height >> f >> foodX >> foodY >> s;

    if (w == constW and f == constF and s == constS) {
        m_mapDimension = std::make_pair(width, height);
        m_foodPosition = std::make_pair(foodX, foodY);

        istr >> d;
        switch (d) {
            case constU:
                m_currentDirection = Direction_UP;
                break;
            case constD:
                m_currentDirection = Direction_DOWN;
                break;
            case constL:
                m_currentDirection = Direction_LEFT;
                break;
            case constR:
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

bool Controller::isSegmentAtPosition(int x, int y) const
{
    return m_segments.end() !=  std::find_if(m_segments.cbegin(), m_segments.cend(),
        [x, y](auto const& segment){ return segment.x == x and segment.y == y; });
}

bool Controller::isPositionOutsideMap(int x, int y) const
{
    return x < 0 or y < 0 or x >= m_mapDimension.first or y >= m_mapDimension.second;
}

void Controller::sendPlaceNewFood(int x, int y)
{
    m_foodPosition = std::make_pair(x, y);

    m_displayPort.send(std::make_unique<EventT<DisplayInd>>(returnFoodCell(m_foodPosition)));
}

void Controller::sendClearOldFood()
{
    DisplayInd clearOldFood ;
    clearOldFood.x = m_foodPosition.first;
    clearOldFood.y = m_foodPosition.second;
    clearOldFood.value = Cell_FREE;

    m_displayPort.send(std::make_unique<EventT<DisplayInd>>(clearOldFood));
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

///Controller::
Segment Controller::calculateNewHead() const
{
    Segment const& currentHead = m_segments.front();

    Segment newHead;
    newHead.x = currentHead.x + (isHorizontal(m_currentDirection) ? isPositive(m_currentDirection) ? 1 : -1 : 0);
    newHead.y = currentHead.y + (isVertical(m_currentDirection) ? isPositive(m_currentDirection) ? 1 : -1 : 0);

    //newHead = returnNewHead(currentHead);
    return newHead;
}

void Controller::removeTailSegment()
{
    auto tail = m_segments.back();
    
    m_displayPort.send(std::make_unique<EventT<DisplayInd>>(returnFreeCell(tail)));

    m_segments.pop_back();
}

void Controller::addHeadSegment(Segment const& newHead)
{
    m_segments.push_front(newHead);

    m_displayPort.send(std::make_unique<EventT<DisplayInd>>(returnSnakeCell(newHead)));
}

void Controller::removeTailSegmentIfNotScored(Segment const& newHead)
{
    if (std::make_pair(newHead.x, newHead.y) == m_foodPosition) {
        m_scorePort.send(std::make_unique<EventT<ScoreInd>>());
        m_foodPort.send(std::make_unique<EventT<FoodReq>>());
    } else {
        removeTailSegment();
    }
}

void Controller::updateSegmentsIfSuccessfullMove(Segment const& newHead)
{
    if (isPositionOutsideMap(newHead.x, newHead.y) or isSegmentAtPosition(newHead.x, newHead.y)  ) {
        m_scorePort.send(std::make_unique<EventT<LooseInd>>());
    } else {
        addHeadSegment(newHead);
        removeTailSegmentIfNotScored(newHead);
    }
}

void Controller::handleTimeoutInd()
{
    updateSegmentsIfSuccessfullMove(calculateNewHead());
}

void Controller::handleDirectionInd(std::unique_ptr<Event> e)
{
    auto direction = payload<DirectionInd>(*e).direction;

    if (perpendicular(m_currentDirection, direction)) {
        m_currentDirection = direction;
    }
}

void Controller::updateFoodPosition(int x, int y, std::function<void()> clearPolicy)
{
        
    if (isSegmentAtPosition(x, y) or isPositionOutsideMap(x,y)) {
        m_foodPort.send(std::make_unique<EventT<FoodReq>>());
        return;
    }

    clearPolicy();
    sendPlaceNewFood(x, y);
}

void Controller::handleFoodInd(std::unique_ptr<Event> e)
{
    auto receivedFood = payload<FoodInd>(*e);

    updateFoodPosition(receivedFood.x, receivedFood.y, std::bind(&Controller::sendClearOldFood, this));
}

void Controller::handleFoodResp(std::unique_ptr<Event> e)
{
    auto requestedFood = payload<FoodResp>(*e);

    
    updateFoodPosition(requestedFood.x, requestedFood.y, []{});
}

void Controller::handlePauseInd(std::unique_ptr<Event> e)
{
    m_paused = not m_paused;
}


DisplayInd Controller::returnFreeCell (Segment seg){
    
    DisplayInd l_evt;

    l_evt.x = seg.x;
    l_evt.y = seg.y;
    l_evt.value = Cell_FREE;

    return l_evt;

}

DisplayInd Controller::returnFoodCell (std::pair<int,int> seg){
    
    DisplayInd cell;

    cell.x = seg.first;
    cell.y = seg.second;
    cell.value = Cell_FOOD;

    return cell;

}

DisplayInd Controller::returnSnakeCell (Segment seg){
    
    DisplayInd cell;

    cell.x = seg.x;
    cell.y = seg.y;
    cell.value = Cell_SNAKE;

    return cell;

}

Segment Controller::returnNewHead (Segment const currentHead){

    Segment newHead;
    newHead.x = currentHead.x + (isHorizontal(m_currentDirection) ? isPositive(m_currentDirection) ? 1 : -1 : 0);
    newHead.y = currentHead.y + (isVertical(m_currentDirection) ? isPositive(m_currentDirection) ? 1 : -1 : 0);

    return newHead;

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
