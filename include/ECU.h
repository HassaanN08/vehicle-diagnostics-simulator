#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "DTC.h"

enum class Severity : int;
enum class ECUStatus : int;

class ECU {
    std::string name;
    std::vector<DTC> faults;
    ECUStatus status;

    public:
        ECU(const std::string name);

        std::string getName() const;

        void addDTC(const DTC& dtc);

        bool setECUStatus(const ECUStatus& state);

        ECUStatus getECUStatus() const;

        void displayDTCs() const;

        bool hasFaults() const;

        void clearFaults();
};