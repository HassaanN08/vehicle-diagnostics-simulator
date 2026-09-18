#pragma once

#include <cstdint>
#include <array>
#include <vector>

#include "can/CANFrame.h"
#include "isotp/Receiver.h"

class NewIsoTp {
    Receiver m_receiver;

    public:
        NewIsoTp(const std::uint16_t diagnosticRequestCANId, const std::uint16_t diagnosticResponseCANId) 
            : m_receiver {diagnosticRequestCANId, diagnosticResponseCANId} {}

        
};