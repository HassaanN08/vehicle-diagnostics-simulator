#pragma once

#include <cstdint>
#include <array>
#include <vector>

#include "can/CANFrame.h"
#include "isotp/Receiver.h"
#include "isotp/Sender.h"

class NewIsoTp {
    Receiver m_receiver;
    Sender m_sender;

    public:
        NewIsoTp(const std::uint16_t diagnosticRequestCANId, const std::uint16_t diagnosticResponseCANId, const std::vector<std::uint8_t>& payload) 
            : m_receiver {diagnosticRequestCANId, diagnosticResponseCANId}
            , m_sender {diagnosticRequestCANId, diagnosticResponseCANId, payload} {}

        
};