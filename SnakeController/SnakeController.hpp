#pragma once

#include <list>
#include <memory>

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

    int addToXCurrentHead();
    int addToYCurrentHead();

    template<typename T, typename U>
    int isSegmentANewHead(T segment, U newHead);

    template <typename T>
    int checkFoodPosition(T newHead);

    template <typename T>
    int isNewHeadOutOfRange(T newHead);

    template <typename T>
    int isCurrentDirectionLeft(T direction);

    template <typename T, typename U>
    int isFoodCollidingWithSnake(T segment, U requestedFood);

    void clearOldFood();

    template<typename T>
    void placeNewFood(T receivedFood);

    template<typename T>
    void placeNewHead(T newHead);

    template<typename T>
    void requestNewFoodIfCollided(bool requestedFoodCollidedWithSnake, T requestedFood);

    template<typename T>
    void ifNotLost(T newHead, bool* lost);

private:
    struct Segment
    {
        int x;
        int y;
        int ttl;
    };

    IPort& m_displayPort;
    IPort& m_foodPort;
    IPort& m_scorePort;

    std::pair<int, int> m_mapDimension;
    std::pair<int, int> m_foodPosition;

    Direction m_currentDirection;
    std::list<Segment> m_segments;
};

} // namespace Snake
