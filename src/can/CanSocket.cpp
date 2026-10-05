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

    return CanSocket { std::move(fdWrapper) };
}

std::optional<CANFrame> CanSocket::receiveFrame() {
    can_frame frame {};

    ssize_t readBytes { read(m_fd.getFd(), &frame, sizeof(frame)) };

    while (readBytes == -1 && errno == EINTR) {
        readBytes = read(m_fd.getFd(), &frame, sizeof(frame));
    }

    if (readBytes == -1) {
        perror("read");
        return std::nullopt;
    }

    if (readBytes != sizeof(frame)) {
        return std::nullopt;
    }

    if ((frame.can_id & CAN_ERR_FLAG) || (frame.can_id & CAN_RTR_FLAG) || (frame.can_id & CAN_EFF_FLAG)) {
        return std::nullopt;
    }

    if (frame.len > CAN_MAX_DLEN) {
        return std::nullopt;
    }

    std::uint16_t frameId { static_cast<std::uint16_t>(frame.can_id & CAN_SFF_MASK) };
    std::vector<std::uint8_t> payload;
    payload.reserve(frame.len);

    for (std::size_t i { 0 }; i < frame.len; ++i) {
        payload.push_back(frame.data[i]);
    }

    return CANFrame::createCANFrame(frameId, payload);
}

CanSocketSendFrameResult CanSocket::sendFrame(const CANFrame& vdsFrame) {
    can_frame linuxFrame {};
    linuxFrame.can_id = vdsFrame.getFrameId();
    std::vector<std::uint8_t> payload { vdsFrame.getFramePayload() };
    linuxFrame.len = payload.size();

    for (std::size_t i {}; i < linuxFrame.len; ++i) {
        linuxFrame.data[i] = payload[i];
    }

    ssize_t writtenBytes { write(m_fd.getFd(), &linuxFrame, sizeof(linuxFrame)) };

    while (writtenBytes == -1 && errno == EINTR) {
        writtenBytes = write(m_fd.getFd(), &linuxFrame, sizeof(linuxFrame));
    }

    if (writtenBytes == -1) {
        perror("write");
        return CanSocketSendFrameResult::Error;
    }

    if (writtenBytes != sizeof(linuxFrame)) {
        return CanSocketSendFrameResult::UnexpectedWrittenSize;
    }

    return CanSocketSendFrameResult::FrameSent;
}