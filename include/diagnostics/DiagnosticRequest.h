#pragma once
#include <vector>
#include <string>
#include <cstdint>

class DiagnosticRequest {
    public:
        int sourceCANId = 0;
        int serviceId = 0;
        std::string serviceName = "";
        std::vector<uint8_t> parameters;
        bool recognized = false;
        bool valid = false;
        std::string error = "";
};