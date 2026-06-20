#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "DTC.h"

enum class Severity : int;

class ECU {
    std::string name;
    std::vector<DTC> faults;

    public:
        ECU(std::string name);

        void addDTC(std::string code, std::string description, Severity severity);

        void displayName() const;

        void displayDTCs() const;

        bool hasFaults() const;

        void clearFaults();
};