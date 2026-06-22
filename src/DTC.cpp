#include <iostream>
#include <string>
#include "DTC.h"
#include "Severity.h"

DTC::DTC(const std::string& code, const std::string& description, Severity severity)
    : code(code)
    , description(description)
    , severity(severity) {}

std::string DTC::severityToString() const {
    if (severity == Severity::Low) {
        return "Low";
    } else if (severity == Severity::Medium) {
        return "Medium";
    } else {
        return "High";
    }
}

std::string DTC::getCode() const {
    return code;
}

std::string DTC::getDescription() const {
    return description;
}

Severity DTC::getSeverity() const {
    return severity;
}

void DTC::display() const {
    std::cout << "\nCode: " << code;
    std::cout << "\nDescription: " << description;
    std::cout << "\nSeverity: " << severityToString() << '\n';
}