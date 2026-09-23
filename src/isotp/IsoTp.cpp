#include "isotp/NewIsoTp.h"
#include "isotp/Receiver.h"
#include "can/CANFrame.h"

#include <vector>
#include <cstdint>
#include <optional>

IsoTpReceiveFrameResult NewIsoTp::receiveFrame(const CANFrame& frame) {

    std::vector<std::uint8_t> payload { frame.getFramePayload() };

    if ((payload[0] >> 4) < 0x03) {
        ReceiveFrameResult result { m_receiver.receiveFrame(frame) };

        switch(result) {
            case ReceiveFrameResult::CompletedPayload:
                m_reassembledPayload = m_receiver.m_reassembledPayload;
                m_receiver.resetCompleteState();
                return IsoTpReceiveFrameResult::CompletedPayloadIsReady;
            case ReceiveFrameResult::NeedToSendFC:
                return IsoTpReceiveFrameResult::OutgoingCanFrameReady;
            case ReceiveFrameResult::WaitingForMoreFrames:
                return IsoTpReceiveFrameResult::WaitingForNextCF;
            default:
                return IsoTpReceiveFrameResult::Error;
        }

    } else if (payload[0] >> 4 == 0x03) {
        FlowControlResult result { m_sender.receiveFC(frame) };

        switch(result) {
            case FlowControlResult::CTS:
                return IsoTpReceiveFrameResult::OutgoingCanFrameReady;
            case FlowControlResult::Wait:
                return IsoTpReceiveFrameResult::NothingYet;
            case FlowControlResult::Abort:
                return IsoTpReceiveFrameResult::Error;
            case FlowControlResult::InvalidFC:
                return IsoTpReceiveFrameResult::Error;
        }
    }
}

std::optional<CANFrame> NewIsoTp::getNextFrame() {
    if (m_sender.m_currentState == SenderState::ReadyToSendCF) return m_sender.getNextCF();
    if (m_receiver.m_currentState == ReceiverState::SenderPaused) return m_receiver.getFlowControlFrame();

    return std::nullopt;
}

std::optional<CANFrame> NewIsoTp::sendPayload(const std::vector<std::uint8_t>& payload) {
    std::size_t payloadLength { payload.size() };
    if (payloadLength > 0 && payloadLength < 8) {
        return m_sender.processSingleFrame(payload);
    } else if (payloadLength >= 8 && payloadLength <= 4095 ) {
        return m_sender.processFirstFrame(payload);
    } else return std::nullopt;
}

std::vector<std::uint8_t> NewIsoTp::getCompleteReassembledPayload() const {
    return m_reassembledPayload;
}