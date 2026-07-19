#include <vector>
#include <cstdint>
#include "diagnostics/DiagnosticDataStore.h"

bool DiagnosticDataStore::dataIdentifierAccepted(const std::vector<uint8_t>& parameters) const {
    if (parameters.size() != 2) return false;

    int dataIdentifier = 0;

    for (const uint8_t parameter : parameters) {
        dataIdentifier = (dataIdentifier << 8) + parameter;
    }

    switch (dataIdentifier) {
        case 0xF190:
            return true;
        default:
            return false;
    }
}

std::vector<uint8_t> DiagnosticDataStore::returnBytes(const std::vector<uint8_t>& parameters) const {
    std::vector<uint8_t> bytes;
    if (parameters.size() > 2) return bytes;

    int dataIdentifier = 0;

    for (const uint8_t& parameter : parameters) {
        dataIdentifier = (dataIdentifier << 8) + parameter;
    }

    switch (dataIdentifier) {
        case 0xF190:
            bytes.reserve(17);

            for (const char& character : vin) {
                bytes.push_back(static_cast<uint8_t>(character));
            }

            break;
    }

    return bytes;
}