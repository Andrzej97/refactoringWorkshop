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
    
private:
    struct Segment : public Ind
    {
   
        int ttl;
    };

    void placeNewObj(Ind& obj, Cell val);
    void scorePort(Segment& newHead, bool& lost);
    void mapOutOfCheck(Segment& newHead);
    void foodColidateController(FoodInd receivedFood);
    void foodRequestController(FoodResp requestedFood);
    IPort& m_displayPort;
    IPort& m_foodPort;
    IPort& m_scorePort;

    //Ind m_foodPosition;
    //Ind m_mapDimension;

    std::pair<int, int> m_mapDimension;
    std::pair<int, int> m_foodPosition;

    Direction m_currentDirection;
    std::list<Segment> m_segments;
};

} // namespace Snake
