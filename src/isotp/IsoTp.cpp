#include "isotp/NewIsoTp.h"
#include "isotp/Receiver.h"
#include "can/CANFrame.h"

#include <vector>
#include <cstdint>
#include <optional>
#include <chrono>

IsoTpReceiveFrameResult NewIsoTp::receiveFrame(const CANFrame& frame) {

    std::vector<std::uint8_t> payload { frame.getFramePayload() };

    if (payload.empty() || frame.getFrameId() != m_RXCanId) return IsoTpReceiveFrameResult::Error;

    if ((payload[0] >> 4) < 0x03) {
        ReceivePayloadResult result { m_receiver.receivePayload(payload) };

        switch(result) {
            case ReceivePayloadResult::CompletedPayload:
                m_reassembledPayload = m_receiver.m_reassembledPayload;
                m_receiver.resetCompleteState();
                return IsoTpReceiveFrameResult::CompletedPayloadIsReady;
            case ReceivePayloadResult::NeedToSendFC:
                return IsoTpReceiveFrameResult::OutgoingCanFrameReady;
            case ReceivePayloadResult::WaitingForMoreFrames:
                return IsoTpReceiveFrameResult::WaitingForNextCF;
            default:
                return IsoTpReceiveFrameResult::Error;
        }

    } else if (payload[0] >> 4 == 0x03) {
        FlowControlResult result { m_sender.receiveFC(frame) };

        switch(result) {
            case FlowControlResult::CTS:
                if (m_sender.m_lastCFSent.has_value()) {
                    if (std::chrono::steady_clock::now() - *m_sender.m_lastCFSent < m_sender.m_STmin) 
                        return IsoTpReceiveFrameResult::CTS;
                    else if (std::chrono::steady_clock::now() - *m_sender.m_lastCFSent >= m_sender.m_STmin) 
                        return IsoTpReceiveFrameResult::OutgoingCanFrameReady;
                } else return IsoTpReceiveFrameResult::OutgoingCanFrameReady;
            case FlowControlResult::Wait:
                return IsoTpReceiveFrameResult::NothingYet;
            case FlowControlResult::Abort:
                return IsoTpReceiveFrameResult::Error;
            case FlowControlResult::InvalidFC:
                return IsoTpReceiveFrameResult::Error;
        }
    } 
    
    return IsoTpReceiveFrameResult::Error;
}

std::optional<CANFrame> NewIsoTp::getNextFrame() {
    if (m_sender.m_currentState == SenderState::ReadyToSendCF) {
        auto returnFrame { m_sender.getNextCF() };

        if (returnFrame.has_value()) return returnFrame;
    }
    
    if (m_receiver.m_currentState == ReceiverState::SenderPaused) return m_receiver.getFlowControlFrame();

    return std::nullopt;
}

IsoTpTimeoutResponse NewIsoTp::checkTimeout() {
    if ((m_receiver.checkTimeout() == CheckReceiverTimeoutResult::TimeoutExpired) && (m_sender.checkTimeout() == CheckSenderTimeoutResult::TimeoutExpired))
        return IsoTpTimeoutResponse::BothTimedOut;
    else if (m_receiver.checkTimeout() == CheckReceiverTimeoutResult::TimeoutExpired)
        return IsoTpTimeoutResponse::RxTimedOut;
    else if (m_sender.checkTimeout() == CheckSenderTimeoutResult::TimeoutExpired)
        return IsoTpTimeoutResponse::TxTimedOut;
    else return IsoTpTimeoutResponse::Active;
}

std::vector<std::uint8_t> NewIsoTp::getCompleteReassembledPayload() {
    std::vector<std::uint8_t> payload;
    payload.swap(m_reassembledPayload);
    
    return payload;
}