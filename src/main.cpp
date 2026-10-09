#include <optional>
#include <poll.h>

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

    pollfd descriptor {};
    descriptor.fd = socket->getFd();
    descriptor.events = POLLIN;
    
    int timeout { 100 };

    std::optional<CANFrame> frame { std::nullopt };
    std::optional<CANFrame> sendFrame { std::nullopt };

    while(true) {
        descriptor.revents = 0;

        int pollResult { poll(&descriptor, 1, timeout) };
        if (pollResult == -1) {
            if (errno != EINTR) {
                perror ("poll");
                return 0;
            }

            continue;
        }
        
        if (descriptor.revents & POLLIN){
            CanSocketReceiveFrameResult result { socket->receiveFrame(frame) };

            if (result == CanSocketReceiveFrameResult::Error) {
                return 0;
            } else if (result == CanSocketReceiveFrameResult::FrameReceived) {
                if (!frame.has_value())
                    return 0;
                DiagnosticRuntimeResult runtimeResult { runtime.receiveCANFrame(*frame) };

                if (runtimeResult == DiagnosticRuntimeResult::ProcessingError)
                    return 0;
            }
        }

        runtime.checkIsoTpTimeout();

        if (sendFrame.has_value()) {
            CanSocketSendFrameResult result { socket->sendFrame(*sendFrame) };

            if (result == CanSocketSendFrameResult::Error ||
                result == CanSocketSendFrameResult::UnexpectedWrittenSize) {
                return 0;
            }
            else if (result == CanSocketSendFrameResult::WouldBlock)
                continue;
            else if (result == CanSocketSendFrameResult::FrameSent) {
                runtime.confirmOutgoingFrameSent();
                sendFrame = std::nullopt;
            }
        }

        frame = runtime.getOutgoingFrame();

        while(frame.has_value()) {
            CanSocketSendFrameResult result { socket->sendFrame(*frame) };

            if (result == CanSocketSendFrameResult::Error ||
                result == CanSocketSendFrameResult::UnexpectedWrittenSize) {
                return 0;
            } else if (result == CanSocketSendFrameResult::WouldBlock){
                sendFrame = frame;
                break;
            } else if (result == CanSocketSendFrameResult::FrameSent){
                runtime.confirmOutgoingFrameSent();
                frame = runtime.getOutgoingFrame();
            }
        }
    }

    return 0;
}