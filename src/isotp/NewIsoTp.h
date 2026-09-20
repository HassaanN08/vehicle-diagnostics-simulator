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
        NewIsoTp(const std::uint16_t TXCanId, const std::uint16_t RXCanId, const std::vector<std::uint8_t>& payload) 
            : m_receiver {TXCanId, RXCanId}
            , m_sender {TXCanId} {}

        
};