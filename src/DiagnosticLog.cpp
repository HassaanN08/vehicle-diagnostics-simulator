#include "DiagnosticLog.h"
#include <iostream>
#include <string>

void DiagnosticLog::addLog(const std::string& log) {
    if (logs.size() == 50) {
        logs.pop_front();
    }

    logs.push_back(log);
}

void DiagnosticLog::displayLogs() const {
    if (logs.empty()) {
        std::cout << "\nNo diagnostic events recorded.\n";
        return;
    }

    int i = 0;

    for(const std::string& log : logs) {
        i++;
        std::cout << i << ". " << log << '\n';
    }
}

bool DiagnosticLog::logsExist() const {
    return !logs.empty();
}

std::string DiagnosticLog::getFirstLog() const{
    if (!logs.empty()) {
        return logs[0];
    }

    return "No Logs Found";
}

size_t DiagnosticLog::getSize() const {
    return logs.size();
}