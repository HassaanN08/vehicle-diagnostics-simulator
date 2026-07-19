#include <vector>
#include <cstdint>
#include "DiagnosticDataStore.h"

bool DiagnosticDataStore::dataIdentifierAccepted(const int& dataIdentifier) const {
    switch (dataIdentifier) {
        case 0xF190:
            return true;
        default:
            return false;
    }
}

std::vector<uint8_t> DiagnosticDataStore::returnBytes(const int& dataIdentifier) const {
    std::vector<uint8_t> bytes;

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