#pragma once

#include "IPort.hpp"

#include <gmock/gmock.h>

namespace Snake
{

class PortMock : public IPort
{
public:
    void send(std::unique_ptr<Event> p_evt) override { return send_rvr(*p_evt); }
    MOCK_METHOD1(send_rvr, void(Event const&));
};

} // namespace Snake
