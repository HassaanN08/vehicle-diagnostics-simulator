#pragma once

#include <cstdint>
#include <string>
#include <string_view>

class DTC {
    std::string m_name {};
    std::uint32_t m_code {};

    public:
        DTC(const std::string_view name, const std::uint32_t code) : m_name { name }, m_code { code } {}

        std::string_view getName() const & { return m_name; }
        std::string_view getName() && = delete;

        std::uint32_t getCode() const { return m_code; }
};