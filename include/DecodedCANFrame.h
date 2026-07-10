#pragma once
#include <string>

class DecodedCANFrame {
    public:
        int id;
        std::string source;
        std::string signalName;
        std::string valueText;
        std::string unit;
        bool known;
        bool valid;
        std::string error;

        std::string toDisplayString() const;
};