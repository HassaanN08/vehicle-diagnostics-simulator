#pragma once
#include <string>
#include <vector>
#include <cstdint>

class DiagnosticResponse {
    public:
        int originalServiceId = 0;
        int responseServiceId = 0;
        bool positive = false;
        bool valid = false;
        std::string responseName = "";
        std::string error = "";
        int negativeResponseCode = 0;
        std::string negativeResponseReason = "";
        std::vector <uint8_t> payload;
};