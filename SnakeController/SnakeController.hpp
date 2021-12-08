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

  struct Segment
    {
        int x;
        int y;
        int ttl;
    };

class Controller : public IEventHandler
{
public:
    
  

    Controller(IPort& p_displayPort, IPort& p_foodPort, IPort& p_scorePort, std::string const& p_config);

    Controller(Controller const& p_rhs) = delete;
    Controller& operator=(Controller const& p_rhs) = delete;

    void receive(std::unique_ptr<Event> e) override;
    void placeNewFood (FoodResp requestedFood,IPort & m_displayPort);
    bool checkFoodRespCollision ( std::list<Segment> seg, FoodResp requestedFood);
    bool checkFoodIndCollision ( std::list<Segment> seg, FoodInd requestedFood);

    bool checkLostFlag ( std::list<Segment> seg, Segment & newHead,IPort & m_scorePort);

    void findFreeCell (std::list<Segment> seg,IPort & m_displayPort); 

    void checkDirection(Direction& currentDirection, Direction& newDirection);

  
    

private:
  

    IPort& m_displayPort;
    IPort& m_foodPort;
    IPort& m_scorePort;

    std::pair<int, int> m_mapDimension;
    std::pair<int, int> m_foodPosition;

    Direction m_currentDirection;
    std::list<Segment> m_segments;
};

} // namespace Snake
