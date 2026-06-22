#pragma once
#include <string>

enum class Severity : int;

class DTC {
    std::string code;
    std::string description;
    Severity severity;
    std::string severityToString() const;

    public:
        DTC(const std::string& code, const std::string& description, Severity severity);

        std::string getCode() const;

        std::string getDescription() const;

        Severity getSeverity() const;

        void display() const;
};