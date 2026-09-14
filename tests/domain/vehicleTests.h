#pragma once

#include <vector>
#include <cassert>

#include "domain/Vehicle.h"
#include "domain/ECU.h"

void vehicleTests() {
    Vehicle vehicle {"Mercedez Benz"};
    const ECU* engine {vehicle.getEcu(0x7E0)};
    const ECU* brake {vehicle.getEcu(0x7E1)};
    const ECU* battery {vehicle.getEcu(0x7E2)};

    const ECU* nonExistantEcu {vehicle.getEcu(0x7E3)};

    assert(!nonExistantEcu);

    bool addEcu { vehicle.addECU(ECU ("Engine2", 0x7E0, 0x7E8)) };

    assert(!addEcu);

    addEcu = vehicle.addECU(ECU ("Gearbox", 0x7E4, 0x7EB));
    assert(addEcu);

    const ECU* gearbox {vehicle.getEcu(0x7E4)};

    assert(engine && brake && battery && gearbox);

    std::vector<ECU>& ecuList { vehicle.getEcuList() };

    assert(vehicle.getName() == "Mercedez Benz");
    assert(ecuList[0].getEcuName() == engine->getEcuName() && ecuList[0].getRequestCANId() == engine->getRequestCANId());
    assert(ecuList[1].getEcuName() == brake->getEcuName() && ecuList[1].getRequestCANId() == brake->getRequestCANId());
    assert(ecuList[2].getEcuName() == battery->getEcuName() && ecuList[2].getRequestCANId() == battery->getRequestCANId());
    assert(ecuList[3].getEcuName() == gearbox->getEcuName() && ecuList[3].getRequestCANId() == gearbox->getRequestCANId());
}