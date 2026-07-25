#pragma once
#include "RingBuffer.h"
#include <string>

class DiagnosticLog {
    RingBuffer<std::string, 50> logs;

    public:
        void addLog(const std::string& log);

        void displayLogs() const;

        bool logsExist() const;

        std::string getFirstLog() const;

        size_t getSize() const;
};