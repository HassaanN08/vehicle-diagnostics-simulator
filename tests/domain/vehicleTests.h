#pragma once

#include <vector>
#include <cassert>

#include "domain/Vehicle.h"
#include "domain/ECU.h"

void vehicleTests() {
    Vehicle vehicle {"Mercedez Benz"};
    auto engine {vehicle.getEcu(0x7E0)};
    auto brake {vehicle.getEcu(0x7E1)};
    auto battery {vehicle.getEcu(0x7E2)};

    assert(engine && brake && battery);

    std::vector<ECU>& ecuList { vehicle.getEcuList() };

    assert(vehicle.getName() == "Mercedez Benz");
    assert(ecuList[0].getEcuName() == engine->getEcuName() && ecuList[0].getRequestCANId() == engine->getRequestCANId());
    assert(ecuList[1].getEcuName() == brake->getEcuName() && ecuList[1].getRequestCANId() == brake->getRequestCANId());
    assert(ecuList[2].getEcuName() == battery->getEcuName() && ecuList[2].getRequestCANId() == battery->getRequestCANId());
}