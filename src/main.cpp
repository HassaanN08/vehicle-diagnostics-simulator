#include <iostream>
#include <cstdint>

#include "domain/ECU.h"
#include "uds/UDSServer.h"
#include "app/DiagnosticCoordinator.h"
#include "can/CanSocket.h"

int main() {
    auto socket { CanSocket::create("vcan0") };

    if (!socket.has_value())
        return 0;

    auto frame { socket->receiveFrame() };

    if (!frame.has_value())
        return 0;

    std::cout << static_cast<int>(frame->getFrameId()) << '\n';
    
    for (std::uint8_t byte : frame->getFramePayload()) {
        std::cout << static_cast<int>(byte) << '\n';
    }

    frame = CANFrame::createCANFrame(0x321, { 0x11, 0x22, 0x33, 0x44 });

    if (!frame.has_value())
        return 0;

    CanSocketSendFrameResult result { socket->sendFrame(*frame) };

    if (result == CanSocketSendFrameResult::FrameSent)
        std::cout << "Frame sent!";

    return 0;
}