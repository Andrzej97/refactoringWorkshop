#pragma once

#include <cstdint>

namespace Snake
{

enum Direction
{
    Direction_UP    = 0b00,
    Direction_DOWN  = 0b10,
    Direction_LEFT  = 0b01,
    Direction_RIGHT = 0b11
};


struct DirectionInd
{
    static constexpr std::uint32_t MESSAGE_ID = 0x10;

    Direction direction;
};


struct TimeoutInd
{
    static constexpr std::uint32_t MESSAGE_ID = 0x20;
};

enum Cell
{
    Cell_FREE,
    Cell_FOOD,
    Cell_SNAKE
};

class Point{
public:
    Point() : x(0), y(0) {}
    Point(int _x, int _y): x(_x), y(_y) {}
    //Point(std::pair<int, int>& cord) : x(cord.first), y(cord.second) {}
    
    int x;
    int y;

    void setPoint(int& _x, int& _y){ x=_x; y=_y; }
    virtual void setPoint(Point p){ x=p.x; y=p.y; }
};

inline bool operator==(const Point& s, const Point& s2){ return s.x == s2.x and s.y == s2.y; }
inline bool operator>=(const Point& s, const Point& s2){ return s.x >= s2.x or s.y >= s2.y; }


class DisplayInd : public Point
{   
public:
    DisplayInd() : Point() {}
    DisplayInd(int x, int y) : Point(x,y) {}
    //DisplayInd(std::pair<int, int>& cord) : Point(cord) {}
    void setPoint(Point p) { x=p.x; y=p.y; }
    static constexpr std::uint32_t MESSAGE_ID = 0x30;

    Cell value;
};

class FoodInd : public Point
{
public:
    FoodInd() : Point() {}
    FoodInd(int x, int y) : Point(x,y) {}
    //FoodInd(std::pair<int, int>& cord) : Point(cord) {}

    void setPoint(Point p){ x=p.x; y=p.y; }
    static constexpr std::uint32_t MESSAGE_ID = 0x40;
};

struct FoodReq
{
    static constexpr std::uint32_t MESSAGE_ID = 0x41;
};

class FoodResp : public Point
{
public:
    FoodResp() : Point() {}
    FoodResp(int x, int y) : Point(x,y) {}
    //FoodResp(std::pair<int, int>& cord) : Point(cord) {}

    void setPoint(Point p){ x=p.x; y=p.y; }
    static constexpr std::uint32_t MESSAGE_ID = 0x42;
};

struct ScoreInd
{
    static constexpr std::uint32_t MESSAGE_ID = 0x70;
};

struct LooseInd
{
    static constexpr std::uint32_t MESSAGE_ID = 0x71;
};

struct PauseInd
{
    static constexpr std::uint32_t MESSAGE_ID = 0x91;
};

} // namespace Snake
