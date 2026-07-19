#pragma once
#include <vector>
#include <string>
#include <cstdint>

class DiagnosticDataStore {
    std::string vin = "1FTFW1EF0HE123456";

    public:
        bool dataIdentifierAccepted(const int& dataIdentifier) const;

        std::vector<uint8_t> returnBytes(const int& dataIdentifier) const;
};