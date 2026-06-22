#include <iostream>
#include <string>
#include <vector>
#include "Severity.h"
#include "DTC.h"
#include "ECU.h"
#include "Vehicle.h"
#include <limits>
using namespace std;

void clearLine() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

bool doesECUExist(string& ecuName, Vehicle& vehicle) {
    while (!vehicle.doesECUExist(ecuName)) {
        cout << ecuName << " ECU does not exist in this Vehicle. Please enter a valid ECU name, or enter -1 to go back to the menu: ";
        cin >> ecuName;

        if (ecuName == "-1") {
            return false;
        }
    }

    return true;
}

void displayMenu() {
    cout << '\n' << "\n1. Display all ECUs\n";
    cout << "\n2. Scan vehicle\n";
    cout << "\n3. Display vehicle health\n";
    cout << "\n4. Add fault to ECU\n";
    cout << "\n5. Clear faults from ECU\n";
    cout << "\n6. Exit\n" << '\n';
}

void addFaultsToECU(Vehicle& vehicle) {
    string ecuName;
    string faultCode;
    string faultDescription;
    Severity severity;
    int severityIndex;

    cout << "Which ECU to add the fault to?\n";
    getline(cin >> ws, ecuName);

    bool ECUCheck = doesECUExist(ecuName, vehicle);

    if (!ECUCheck) {
        return;
    }

    cout <<"Enter fault code: ";
    cin >> faultCode;

    cout << "Enter fault description: ";
    getline(cin >> ws, faultDescription);

    cout << "Choose severity:\n";
    cout << "1. Low\n" << "2. Medium\n" << "3. High\n\n";

    cin >> severityIndex;

    while (cin.fail()) {
        clearLine();
        cout << "\nPlease enter a number, not a character, or enter -1 to go back to the menu: ";
        cin >> severityIndex;
    }

    while (severityIndex < 1 || severityIndex > 3) {
        if (cin.fail()) {
            clearLine();
            cout << "\nPlease enter a number, not a character, or enter -1 to go back to the menu: ";
            cin >> severityIndex;
        } else {
            cout << "Please enter a number between 1 and 3, or enter -1 to go back to the menu: ";
            cin >> severityIndex;
        }

        if (severityIndex == -1) return;
    }

    switch(severityIndex) {
        case 1:
            severity = Severity::Low;
            break;
        case 2:
            severity = Severity::Medium;
            break;
        case 3:
            severity = Severity::High;
            break;
    }

    vehicle.addDTCToECU(ecuName, DTC(faultCode, faultDescription, severity));
}

void clearFaults(Vehicle& vehicle) {
    string ecuName;

    cout << "Enter ECU name: ";
    cin >> ecuName;

    bool ECUCheck = doesECUExist(ecuName, vehicle);

    if (!ECUCheck) {
        return;
    }

    vehicle.clearFaultsFromECU(ecuName);
}

int main() {

    cout << "\nVehicle Diagnostics Simulator\n";
    Vehicle vehicle("Toyota Subaru");

    ECU Engine ("Engine");
    ECU Brake ("Brake");
    ECU Battery ("Battery");

    vehicle.addECU(Engine);
    vehicle.addECU(Brake);
    vehicle.addECU(Battery);

    displayMenu();

    int option;

    cout << "Choose an option: ";
    cin >> option;

    while (cin.fail()) {
        clearLine();
        cout << "Please type in a number, not a character: ";
        cin >> option;
    }

    while (option != 6) {
        if (option < 1 || option > 6) {
            cout << "Please type in a number between 1 and 6: ";
            cin >> option;
            while (cin.fail()) {
                clearLine();
                cout << "Please type in a number, not a character: ";
                cin >> option;
            }
        }
        else if (option > 0 && option < 6) {
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
                    clearFaults(vehicle);
                    break;
            }

            displayMenu();

            cout << "Choose an option: ";
            cin >> option;

            while (cin.fail()) {
                clearLine();
                cout << "Please type in a number, not a character: ";
                cin >> option;
            }
        }
    }
    return 0;
}