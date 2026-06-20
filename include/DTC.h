#pragma once
#include <string>

enum class Severity : int;

class DTC {
    std::string code;
    std::string description;
    Severity severity;

    public:
        DTC(std::string code, std::string description, Severity severity);

        std::string getCode() const;

        std::string getDescription() const;

        Severity getSeverity() const;

        void display() const;
};