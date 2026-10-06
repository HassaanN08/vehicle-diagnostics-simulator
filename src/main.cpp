#include <iostream>
#include <cstdint>

#include "domain/Vehicle.h"
#include "app/DiagnosticRuntime.h"
#include "can/CanSocket.h"
#include "can/CANFrame.h"

int main() {
    Vehicle vehicle { "Mercedez G-Wagon" };
    DiagnosticRuntime runtime { vehicle };

    auto socket { CanSocket::create("vcan0") };

    if (!socket.has_value())
        return 0;

    while(true) {
        auto frame { socket->receiveFrame() };
        if (!frame.has_value())
            return 0;

        DiagnosticRuntimeResult runtimeResult { runtime.receiveCANFrame(*frame) };

        if (runtimeResult == DiagnosticRuntimeResult::CoordinatorNotFound)
            continue;

        if (runtimeResult == DiagnosticRuntimeResult::ProcessingError)
            return 0;

        frame = runtime.getOutgoingFrame();

        while(frame.has_value()) {
            CanSocketSendFrameResult result { socket->sendFrame(*frame) };

            if (result != CanSocketSendFrameResult::FrameSent)
                return 0;

            frame = runtime.getOutgoingFrame();
        }
    }

    return 0;
}