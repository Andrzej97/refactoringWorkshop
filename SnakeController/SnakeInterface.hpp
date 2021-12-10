#pragma once

#include <cstdint>

namespace Snake
{

struct Segment
{
    int x;
    int y;
};

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

struct DirectionSet
{
    static const char up = 'U';
    static const char down = 'D';
    static const char left = 'L';
    static const char right = 'R';
};

struct ConfigSet
{
    static const char width = 'W';
    static const char food = 'F';
    static const char snake = 'S';
};

} // namespace Snake
