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

        std::string getName() const;

        void addDTC(const DTC& dtc);

        void displayDTCs() const;

        bool hasFaults() const;

        void clearFaults();
};