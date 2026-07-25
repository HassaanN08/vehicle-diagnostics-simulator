#include "DiagnosticLog.h"
#include "RingBuffer.h"
#include <iostream>
#include <string>

void DiagnosticLog::addLog(const std::string& log) {
    logs.push(log);
}

void DiagnosticLog::displayLogs() const {
    if (logs.empty()) {
        std::cout << "\nNo diagnostic events recorded.\n";
        return;
    }

    for(size_t i = 0; i < logs.size(); i++) {
        std::cout << i + 1 << ". " << logs.at(i) << '\n';
    }
}

bool DiagnosticLog::logsExist() const {
    return !logs.empty();
}

std::string DiagnosticLog::getFirstLog() const{
    if (!logs.empty()) {
        return logs.at(0);
    }

    return "No Logs Found";
}

size_t DiagnosticLog::getSize() const {
    return logs.size();
}