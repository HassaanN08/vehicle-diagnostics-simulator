#include "can/CANFrame.h"

#include <vector>
#include <cstdint>

CANFrame::CANFrame(const int frameId, const std::vector<std::uint8_t>& payload) {
    m_payloadLength = payload.size();

    if (frameId >= 0x000 && frameId <= 0x7FF && m_payloadLength <= 8) {
        m_isValid = true;
        m_frameId = static_cast<uint16_t>(frameId);
        m_framePayload = payload;
    }
}