#include <iostream>
#include <string>
#include <vector>
#include "ECU.h"
#include "Severity.h"
using namespace std;

int main() {
    ECU Engine ("Engine");
    ECU Brake ("Brake");
    ECU Battery ("Battery");

    Engine.addDTC("P0301", "Cylinder 1 Misfire Detected", Severity::High);
    Engine.addDTC("P0171", "System Too Lean", Severity::Medium);

    Brake.addDTC("C0035", "Left Front Wheel Speed Sensor Fault", Severity::High);

    Battery.addDTC("B1001", "Battery Voltage Low", Severity::Low);

    cout << "\nElectronic Control Units:\n";

    Engine.displayName();
    Engine.displayDTCs();

    Brake.displayName();
    Brake.displayDTCs();

    Battery.displayName();
    Battery.displayDTCs();

    Engine.clearFaults();

    Engine.displayName();
    Engine.displayDTCs();

    cout << "\nDoes the Engine have faults?\n" << (Engine.hasFaults() ? "Yes" : "No") << '\n';
    cout << "\nDoes the Brake have faults?\n" << (Brake.hasFaults() ? "Yes" : "No") << '\n';
    cout << "\nDoes the Battery have faults?\n" << (Battery.hasFaults() ? "Yes" : "No") << '\n';
   
    cout << '\n';
    return 0;
}