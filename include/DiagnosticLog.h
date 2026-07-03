#pragma once
#include <deque>
#include <string>

class DiagnosticLog {
    std::deque<std::string> logs;

    public:
        void addLog(const std::string& log);

        void displayLogs() const;

        bool logsExist() const;

        std::string getFirstLog() const;

        size_t getSize() const;
};