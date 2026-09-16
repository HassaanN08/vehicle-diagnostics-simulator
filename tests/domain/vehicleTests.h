#pragma once

#include <vector>
#include <cassert>

#include "domain/Vehicle.h"
#include "domain/ECU.h"

void vehicleTests() {
    Vehicle vehicle {"Mercedez Benz"};
    const ECU* engine {vehicle.findEcuByRequestCanId(0x7E0)};
    const ECU* brake {vehicle.findEcuByRequestCanId(0x7E1)};
    const ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};

    const ECU* nonExistantEcu {vehicle.findEcuByRequestCanId(0x7E3)};

    assert(!nonExistantEcu);

    AddEcuResult addEcu { vehicle.addECU(ECU ("Engine2", 0x7E0, 0x7E8, 0x0F)) };

    assert(addEcu == AddEcuResult::ecuAlreadyExists);

    addEcu = vehicle.addECU(ECU ("Gearbox", 0x7E4, 0x7EB, 0x1F));
    assert(addEcu == AddEcuResult::success);

    const ECU* gearbox {vehicle.findEcuByRequestCanId(0x7E4)};

    assert(engine && brake && battery && gearbox);

    const std::vector<ECU>& ecuList { vehicle.getEcuList() };

    assert(vehicle.getName() == "Mercedez Benz");
    assert(ecuList[0].getEcuName() == engine->getEcuName() && ecuList[0].getRequestCANId() == engine->getRequestCANId());
    assert(ecuList[1].getEcuName() == brake->getEcuName() && ecuList[1].getRequestCANId() == brake->getRequestCANId());
    assert(ecuList[2].getEcuName() == battery->getEcuName() && ecuList[2].getRequestCANId() == battery->getRequestCANId());
    assert(ecuList[3].getEcuName() == gearbox->getEcuName() && ecuList[3].getRequestCANId() == gearbox->getRequestCANId());
}