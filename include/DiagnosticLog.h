#pragma once
#include <vector>
#include <string>

class DiagnosticLog {
    std::vector<std::string> logs;

    public:
        void addLog(std::string& log);

        void displayLogs() const;

        bool logsExist() const;
};