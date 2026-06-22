#include <iostream>
#include <string>
#include <vector>
#include "Severity.h"
#include "DTC.h"
#include "ECU.h"
#include "Vehicle.h"
using namespace std;

int main() {
    Vehicle vehicle("Toyota Subaru");

    ECU Engine ("Engine");
    ECU Brake ("Brake");
    ECU Battery ("Battery");

    vehicle.addECU(Engine);
    vehicle.addECU(Brake);
    vehicle.addECU(Battery);

    DTC fault1 ("P0301", "Cylinder 1 Misfire Detected", Severity::High);
    DTC fault2 ("P0171", "System Too Lean", Severity::Medium);
    DTC fault3 ("C0035", "Left Front Wheel Speed Sensor Fault", Severity::High);
    DTC fault4 ("B1001", "Battery Voltage Low", Severity::Low);

    vehicle.addDTCToECU("Engine", fault1);
    vehicle.addDTCToECU("Engine", fault2);
    vehicle.addDTCToECU("Brake", fault3);
    vehicle.addDTCToECU("Battery", fault4);

    cout << '\n' << vehicle.getName() << '\n';

    cout << "\nDisplay All ECUs:\n";

    vehicle.displayAllECUs();

    vehicle.scanVehicle();

    vehicle.clearFaultsFromECU("Engine");

    vehicle.scanVehicle();

    vehicle.clearFaultsFromECU("Transmission");
   
    cout << '\n';
    return 0;
}