#include "SnakeController.hpp"
#include "WorldController.hpp"

#include "EventT.hpp"

#include <gtest/gtest.h>

#include "Mocks/PortMock.hpp"
#include "Mocks/EventMatchers.hpp"

using namespace ::testing;

namespace Snake
{

constexpr int STAGE_WIDTH = 100;
constexpr int STAGE_HEIGHT = 100;

struct SnakeBugTest : Test
{
    EventT<TimeoutInd> te;

    StrictMock<PortMock> displayPortMock;
    StrictMock<PortMock> foodPortMock;
    StrictMock<PortMock> scorePortMock;

    void configureSUT(std::string p_config)
    {
        sut = std::make_unique<SnakeController>(displayPortMock, foodPortMock, scorePortMock, p_config);
    }

    std::unique_ptr<SnakeController> sut = nullptr;
};

typedef std::pair<int, int> TestValues;

struct SnakeTestWrongFoodLocation : SnakeBugTest, public WithParamInterface<TestValues>
{
    void SetUp() override
    {
        std::stringstream ss;
        ss << "W " << STAGE_WIDTH << " " << STAGE_HEIGHT << " F 50 50 S R 1 20 20";
        configureSUT(ss.str());
    }
};

TEST_P(SnakeTestWrongFoodLocation, test_foodIndWrongCoordinates)
{
    World::FoodInd l_foodInd;
    l_foodInd.x = GetParam().first;
    l_foodInd.y = GetParam().second;
    EXPECT_CALL(foodPortMock, send_rvr(AnyFoodReq()));
    sut->receive(std::make_unique<EventT<World::FoodInd>>(l_foodInd));
}

TEST_P(SnakeTestWrongFoodLocation, test_foodRespWrongCoordinates)
{
    World::FoodResp l_foodResp;
    l_foodResp.x = GetParam().first;
    l_foodResp.y = GetParam().second;
    EXPECT_CALL(foodPortMock, send_rvr(AnyFoodReq()));
    sut->receive(std::make_unique<EventT<World::FoodResp>>(l_foodResp));
}

 // UTs to allign from extract-subclass 2 onward. To "turn them on" uncomment lines below.

INSTANTIATE_TEST_CASE_P(WrongDimensionsInFoodPlacementTest, SnakeTestWrongFoodLocation,
                        Values(TestValues(-1, 10),
                               TestValues(STAGE_WIDTH, 10),
                               TestValues(10, -1),
                               TestValues(10, STAGE_HEIGHT)));
} // namespace Snake
