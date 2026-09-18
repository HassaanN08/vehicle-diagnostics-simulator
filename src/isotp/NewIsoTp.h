#pragma once

#include <cstdint>
#include <array>
#include <vector>

#include "can/CANFrame.h"
#include "isotp/Receiver.h"
#include "domain/ECU.h"

enum class ReceiverState {
    Idle,
    Reassembling,
};

enum class ReceiveFrameResult {
    WaitingForMoreFrames,
    CompletedPayload,
    NeedToSendFC,
    TransportError,
};

class NewIsoTp {
    Receiver m_receiver;

    public:
        NewIsoTp(const std::uint16_t diagnosticRequestCANId, const std::uint16_t diagnosticResponseCANId) 
            : m_receiver {diagnosticRequestCANId, diagnosticResponseCANId} {}

        
};