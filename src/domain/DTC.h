#pragma once

#include <cstdint>
#include <optional>

class DTC {
    std::uint32_t m_diagnosticId {};
    std::uint8_t m_udsStatus {};

    DTC(const std::uint32_t diagnosticId) : m_diagnosticId { diagnosticId } {}

    public:
        static inline std::optional<DTC> createDTC(std::uint32_t diagnosticId) {
            if (diagnosticId > 0xFFFFFF) return std::nullopt;
            
            return DTC {diagnosticId};
        }

        std::uint32_t getDiagnosticId() const { return m_diagnosticId; }
        std::uint8_t getStatus() const { return m_udsStatus; }

        void setStatus(std::uint8_t udsStatus) { m_udsStatus = udsStatus; }
};