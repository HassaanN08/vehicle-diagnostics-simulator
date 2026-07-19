#pragma once
#include <vector>
#include <string>
#include <cstdint>

class DiagnosticDataStore {
    std::string vin = "1FTFW1EF0HE123456";

    public:
        bool dataIdentifierAccepted(const std::vector<uint8_t>& parameters) const;

        std::vector<uint8_t> returnBytes(const std::vector<uint8_t>& parameters) const;
};