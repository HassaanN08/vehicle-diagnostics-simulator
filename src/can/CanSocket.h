#pragma once

#include <utility>
#include <optional>
#include <string>

#include "FdOwner.h"
#include "CANFrame.h"

enum class CanSocketSendFrameResult {
    FrameSent,
    WouldBlock,
    UnexpectedWrittenSize,
    Error,
};

enum class CanSocketReceiveFrameResult {
    FrameReceived,
    NoFrameAvailable,
    Error,
};

class CanSocket {
    FdOwner m_fd;
    CanSocket(FdOwner owner) : m_fd { std::move(owner) } {}

    public:
        static std::optional<CanSocket> create(const std::string& ifName);
        CanSocketReceiveFrameResult receiveFrame(std::optional<CANFrame>&);
        CanSocketSendFrameResult sendFrame(const CANFrame&);
        int getFd() const { return m_fd.getFd(); }
};