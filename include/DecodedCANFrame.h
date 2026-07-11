#pragma once
#include <string>

class DecodedCANFrame {
    public:
        int id = 0;
        std::string source = "Unknown";
        std::string signalName = "Unknown";
        std::string valueText = "";
        std::string unit = "";
        bool known = false;
        bool valid = false;
        std::string error = "";

        std::string toDisplayString() const;
};