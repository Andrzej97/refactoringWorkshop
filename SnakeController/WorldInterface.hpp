#pragma once

#include <cstdint>

namespace World
{

enum Cell
{
    Cell_FREE,
    Cell_FOOD,
    Cell_SNAKE
};

struct DisplayInd
{
    static constexpr std::uint32_t MESSAGE_ID = 0x30;

    int x;
    int y;
    Cell value;
};

struct FoodInd
{
    static constexpr std::uint32_t MESSAGE_ID = 0x40;

    int x;
    int y;
};

struct FoodReq
{
    static constexpr std::uint32_t MESSAGE_ID = 0x41;
};

struct FoodResp
{
    static constexpr std::uint32_t MESSAGE_ID = 0x42;

    int x;
    int y;
};

}   // namespace World