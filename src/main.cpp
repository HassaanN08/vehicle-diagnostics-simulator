#include <iostream>
#include "ECU.h"
#include "Vehicle.h"
#include "VehicleSession.h"
#include "ConsoleInput.h"
#include "MenuActions.h"
using namespace std;

int main() {

    cout << "\nVehicle Diagnostics Simulator\n";
    Vehicle vehicle("Toyota Subaru");

    ECU Engine ("Engine");
    ECU Brake ("Brake");
    ECU Battery ("Battery");

    vehicle.addECU(Engine, false);
    vehicle.addECU(Brake, false);
    vehicle.addECU(Battery, false);

    displayMenu();

    int option;

    cout << "Choose an option: ";
    cin >> option;

    validateInputIsInt(option);

    while (option != 15) {
        if (option < 1 || option > 15) {
            cout << "Please type in a number between 1 and 15: ";
            cin >> option;
            validateInputIsInt(option);
        }
        else if (option > 0 && option < 15) {
            switch (option) {
                case 1:
                    vehicle.displayAllECUs();
                    break;
                case 2:
                    vehicle.scanVehicle();
                    break;
                case 3:
                    if (vehicle.hasActiveFaults()) {
                        cout << "\nVehicle health: Faults Detected\n";
                    } else {
                        cout << "\nVehicle health: Healthy\n";
                    }
                    break;
                case 4:
                    addFaultsToECU(vehicle);
                    break;
                case 5:
                    clearFaultsMenu(vehicle);
                    break;
                case 6:
                    addECUViaMenu(vehicle);
                    break;
                case 7:
                    vehicle.displayLogs();
                    break;
                case 8:
                    setStatusMenu(vehicle);
                    break;
                case 9:
                    ECUFaultHistoryMenu(vehicle);
                    break;
                case 10:
                    if (vehicle.getDiagnosticSession() == VehicleSession::DefaultSession) {
                        cout << "\nVehicle is in Default Session\n";
                    } else {
                        cout << "\nVehicle is in Extended Session\n";
                    }
                    break;
                case 11:
                    setDiagnosticSessionMenu(vehicle);
                    break;
                case 12:
                    simulateSampleCANTraffic(vehicle);
                    break;
                case 13:
                    addCustomCANFrame(vehicle);
                    break;
                case 14:
                    displayCANBusTrafficMenu(vehicle);
                    break;
            }

            displayMenu();

            cout << "Choose an option: ";
            cin >> option;

            validateInputIsInt(option);
        }
    }

    return 0;
}