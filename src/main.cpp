#include <iostream>
#include <cstdint>

#include "domain/ECU.h"
#include "uds/UDSServer.h"
#include "app/DiagnosticCoordinator.h"
#include "can/CanSocket.h"

int main() {
    auto socket { CanSocket::create("vcan0") };

    auto frame { socket->receiveFrame() };

    std::cout << static_cast<int>(frame->getFrameId()) << '\n';
    
    for (std::uint8_t byte : frame->getFramePayload()) {
        std::cout << static_cast<int>(byte) << '\n';
    }

    return 0;
}