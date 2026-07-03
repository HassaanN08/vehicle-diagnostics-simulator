#include "DiagnosticLog.h"
#include <iostream>
#include <vector>
#include <string>

void DiagnosticLog::addLog(std::string& log) {
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