#pragma once

#include <cstdint>

class DTC {
    std::uint32_t m_diagnosticId {};
    std::uint8_t m_udsStatus {};

    public:
        DTC(const std::uint32_t diagnosticId) : m_diagnosticId { diagnosticId } {}

        std::uint32_t getDiagnosticId() const { return m_diagnosticId; }
        std::uint8_t getStatus() const { return m_udsStatus; }

        void setStatus(std::uint8_t udsStatus) { m_udsStatus = udsStatus; }
};