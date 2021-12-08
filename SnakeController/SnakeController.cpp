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

Controller::Controller(IPort& p_displayPort, IPort& p_foodPort, IPort& p_scorePort, std::string const& p_config)
    : m_displayPort(p_displayPort),
      m_foodPort(p_foodPort),
      m_scorePort(p_scorePort)
{
    std::istringstream istr(p_config);
    char w, f, s, d;

    int width, height, length;
    int foodX, foodY;
    istr >> w >> width >> height >> f >> foodX >> foodY >> s;

    if (w == 'W' and f == 'F' and s == 'S') {
        m_mapDimension = std::make_pair(width, height);
        m_foodPosition = std::make_pair(foodX, foodY);

        istr >> d;
        switch (d) {
            case 'U':
                m_currentDirection = Direction_UP;
                break;
            case 'D':
                m_currentDirection = Direction_DOWN;
                break;
            case 'L':
                m_currentDirection = Direction_LEFT;
                break;
            case 'R':
                m_currentDirection = Direction_RIGHT;
                break;
            default:
                throw ConfigurationError();
        }
        istr >> length;

        while (length) {
            Segment seg;
            istr >> seg.x >> seg.y;
            seg.ttl = length--;

            m_segments.push_back(seg);
        }
    } else {
        throw ConfigurationError();
    }
}

int Controller::addToXCurrentHead()
{
    return ((m_currentDirection & 0b01) ? (m_currentDirection & 0b10) ? 1 : -1 : 0);
}

int Controller::addToYCurrentHead()
{
    return (not (m_currentDirection & 0b01) ? (m_currentDirection & 0b10) ? 1 : -1 : 0);
}

template<typename T, typename U>
int Controller::isSegmentANewHead(T segment, U newHead)
{
    return (segment.x == newHead.x and segment.y == newHead.y);
}

template <typename T>
int Controller::checkFoodPosition(T newHead)
{
    return (std::make_pair(newHead.x, newHead.y) == m_foodPosition);
}

template <typename T>
int Controller::isNewHeadOutOfRange(T newHead)
{
    return (newHead.x < 0 or newHead.y < 0 or
            newHead.x >= m_mapDimension.first or
            newHead.y >= m_mapDimension.second);
}

template <typename T>
int Controller::isCurrentDirectionLeft(T direction)
{
    return ((m_currentDirection & 0b01) == (direction & 0b01));
}

template <typename T, typename U>
int Controller::isFoodCollidingWithSnake(T segment, U requestedFood)
{
    return (segment.x == requestedFood.x and segment.y == requestedFood.y);
}

void Controller::clearOldFood()
{
    DisplayInd clearOldFood;
    clearOldFood.x = m_foodPosition.first;
    clearOldFood.y = m_foodPosition.second;
    clearOldFood.value = Cell_FREE;
    m_displayPort.send(std::make_unique<EventT<DisplayInd>>(clearOldFood));
}

template<typename T>
void Controller::placeNewFood(T receivedFood)
{
    DisplayInd placeNewFood;
    placeNewFood.x = receivedFood.x;
    placeNewFood.y = receivedFood.y;
    placeNewFood.value = Cell_FOOD;
    m_displayPort.send(std::make_unique<EventT<DisplayInd>>(placeNewFood));
}

template<typename T>
void Controller::placeNewHead(T newHead)
{
    DisplayInd placeNewHead;
    placeNewHead.x = newHead.x;
    placeNewHead.y = newHead.y;
    placeNewHead.value = Cell_SNAKE;

    m_displayPort.send(std::make_unique<EventT<DisplayInd>>(placeNewHead));
}

template<typename T>
void Controller::requestNewFoodIfCollided(bool requestedFoodCollidedWithSnake, T requestedFood)
{
    if (requestedFoodCollidedWithSnake) {
        m_foodPort.send(std::make_unique<EventT<FoodReq>>());
    } else {
        placeNewFood(requestedFood);
    }
}

template<typename T>
void Controller::ifNotLost(T newHead, bool* lost)
{
    if (checkFoodPosition(newHead)) {
        m_scorePort.send(std::make_unique<EventT<ScoreInd>>());
        m_foodPort.send(std::make_unique<EventT<FoodReq>>());
    } else if (isNewHeadOutOfRange(newHead)) {
        m_scorePort.send(std::make_unique<EventT<LooseInd>>());
        *lost = true;
    } else {
        for (auto &segment : m_segments) {
            if (not --segment.ttl) {
                DisplayInd l_evt;
                l_evt.x = segment.x;
                l_evt.y = segment.y;
                l_evt.value = Cell_FREE;

                m_displayPort.send(std::make_unique<EventT<DisplayInd>>(l_evt));
            }
        }
    }
}

void Controller::receive(std::unique_ptr<Event> e)
{
    try {
        auto const& timerEvent = *dynamic_cast<EventT<TimeoutInd> const&>(*e);

        Segment const& currentHead = m_segments.front();

        Segment newHead;
        newHead.x = currentHead.x + addToXCurrentHead();
        newHead.y = currentHead.y + addToYCurrentHead();
        newHead.ttl = currentHead.ttl;

        bool lost = false;

        for (auto segment : m_segments) {
            if (isSegmentANewHead(segment, newHead)) {
                m_scorePort.send(std::make_unique<EventT<LooseInd>>());
                lost = true;
                break;
            }
        }

        if (not lost) {
            ifNotLost(newHead, &lost);
        }

        if (not lost) {
            m_segments.push_front(newHead);
            placeNewHead(newHead);

            m_segments.erase(
                std::remove_if(
                    m_segments.begin(),
                    m_segments.end(),
                    [](auto const& segment){ return not (segment.ttl > 0); }),
                m_segments.end());
        }
    } catch (std::bad_cast&) {
        try {
            auto direction = dynamic_cast<EventT<DirectionInd> const&>(*e)->direction;

            if (not isCurrentDirectionLeft(direction)) {
                m_currentDirection = direction;
            }
        } catch (std::bad_cast&) {
            try {
                auto receivedFood = *dynamic_cast<EventT<FoodInd> const&>(*e);

                bool requestedFoodCollidedWithSnake = false;
                for (auto const& segment : m_segments) {
                    if (isFoodCollidingWithSnake(segment, receivedFood)) {
                        requestedFoodCollidedWithSnake = true;
                        break;
                    }
                }

                if (requestedFoodCollidedWithSnake) {
                    m_foodPort.send(std::make_unique<EventT<FoodReq>>());
                } else {
                    clearOldFood();
                    placeNewFood(receivedFood);
                }

                m_foodPosition = std::make_pair(receivedFood.x, receivedFood.y);

            } catch (std::bad_cast&) {
                try {
                    auto requestedFood = *dynamic_cast<EventT<FoodResp> const&>(*e);

                    bool requestedFoodCollidedWithSnake = false;
                    for (auto const& segment : m_segments) {
                        if (isFoodCollidingWithSnake(segment, requestedFood)) {
                            requestedFoodCollidedWithSnake = true;
                            break;
                        }
                    }

                    requestNewFoodIfCollided(requestedFoodCollidedWithSnake, requestedFood);

                    m_foodPosition = std::make_pair(requestedFood.x, requestedFood.y);
                } catch (std::bad_cast&) {
                    throw UnexpectedEventException();
                }
            }
        }
    }
}

} // namespace Snake
