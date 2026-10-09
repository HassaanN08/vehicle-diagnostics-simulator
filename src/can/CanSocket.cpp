#include <unistd.h>
#include <fcntl.h>
#include <utility>
#include <optional>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <cerrno>
#include <string>
#include <cstdint>
#include <span>

#include "can/FdOwner.h"
#include "can/CANFrame.h"
#include "can/CanSocket.h"

std::optional<CanSocket> CanSocket::create(const std::string& ifName) {
    int fd { socket(AF_CAN, SOCK_RAW, CAN_RAW) };

    if (fd == -1) {
        perror("socket");
        return std::nullopt;
    }

    FdOwner fdWrapper { fd };

    unsigned int ifIndex { if_nametoindex(ifName.c_str()) };

    if (ifIndex == 0) {
        perror("interface index");
        return std::nullopt;
    }

    sockaddr_can address {};
    address.can_family = AF_CAN;
    address.can_ifindex = ifIndex;

    int bindResponse { bind(
        fdWrapper.getFd(), 
        reinterpret_cast<sockaddr*>(&address), 
        sizeof(address)
    ) };

    if (bindResponse == -1) {
        perror("bind");
        return std::nullopt;
    }

    int flag { fcntl(fd, F_GETFL) };
    if (flag == -1) {
        perror("get flag");
        return std::nullopt;
    }

    int setFlag { fcntl(fd, F_SETFL, flag | O_NONBLOCK) };
    if (setFlag == -1) {
        perror("set flag");
        return std::nullopt;
    }

    return CanSocket { std::move(fdWrapper) };
}

CanSocketReceiveFrameResult CanSocket::receiveFrame(std::optional<CANFrame>& vdsFrame) {
    can_frame frame {};
    vdsFrame = std::nullopt;

    ssize_t readBytes { read(m_fd.getFd(), &frame, sizeof(frame)) };

    while (readBytes == -1 && errno == EINTR) {
        readBytes = read(m_fd.getFd(), &frame, sizeof(frame));
    }

    if (readBytes == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return CanSocketReceiveFrameResult::NoFrameAvailable;
        } else {
            perror("read");
            return CanSocketReceiveFrameResult::Error;
        }
    }

    if (readBytes != sizeof(frame)) {
        return CanSocketReceiveFrameResult::Error;
    }

    if ((frame.can_id & CAN_ERR_FLAG) || (frame.can_id & CAN_RTR_FLAG) || (frame.can_id & CAN_EFF_FLAG)) {
        return CanSocketReceiveFrameResult::Error;
    }

    if (frame.len > CAN_MAX_DLEN) {
        return CanSocketReceiveFrameResult::Error;
    }

    std::uint16_t frameId { static_cast<std::uint16_t>(frame.can_id & CAN_SFF_MASK) };
    std::span<std::uint8_t> payload { frame.data, frame.len };

    vdsFrame = CANFrame::createCANFrame(frameId, payload);
    if (!vdsFrame.has_value()) {
        return CanSocketReceiveFrameResult::Error;
    }
    return CanSocketReceiveFrameResult::FrameReceived;
}

CanSocketSendFrameResult CanSocket::sendFrame(const CANFrame& vdsFrame) {
    can_frame linuxFrame {};
    linuxFrame.can_id = vdsFrame.getFrameId();
    std::span<const std::uint8_t> payload { vdsFrame.getFramePayload() };
    linuxFrame.len = payload.size();

    for (std::size_t i {}; i < linuxFrame.len; ++i) {
        linuxFrame.data[i] = payload[i];
    }

    ssize_t writtenBytes { write(m_fd.getFd(), &linuxFrame, sizeof(linuxFrame)) };

    while (writtenBytes == -1 && errno == EINTR) {
        writtenBytes = write(m_fd.getFd(), &linuxFrame, sizeof(linuxFrame));
    }

    if (writtenBytes == -1) {
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            return CanSocketSendFrameResult::WouldBlock;
        } else {
            perror("write");
            return CanSocketSendFrameResult::Error;
        }
    }

    if (writtenBytes != sizeof(linuxFrame)) {
        return CanSocketSendFrameResult::UnexpectedWrittenSize;
    }

    return CanSocketSendFrameResult::FrameSent;
}