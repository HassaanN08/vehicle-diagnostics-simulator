#pragma once

#include <utility>
#include <optional>
#include <string>

#include "FdOwner.h"
#include "CANFrame.h"

enum class CanSocketSendFrameResult {
    FrameSent,
    Error,
    UnexpectedWrittenSize,
};

class CanSocket {
    FdOwner m_fd;
    CanSocket(FdOwner owner) : m_fd { std::move(owner) } {}

    public:
        static std::optional<CanSocket> create(const std::string& ifName);
        std::optional<CANFrame> receiveFrame();
        CanSocketSendFrameResult sendFrame(const CANFrame&);
};