#include <iostream>
#include <string>
#include <vector>
#include "Severity.h"
#include "ECUStatus.h"
#include "DTC.h"
#include "ECU.h"
#include "Vehicle.h"
#include "VehicleResults.h"
#include "VehicleSession.h"
#include "ConsoleInput.h"
using namespace std;

bool getValidECUName(std::string& ecuName, Vehicle& vehicle) {
    while (!vehicle.doesECUExist(ecuName)) {
        std::cout << ecuName << " ECU does not exist in this Vehicle. Please enter a valid ECU name, or enter -1 to go back to the menu: ";
        getFullTextInput(ecuName);

        if (ecuName == "-1") {
            return false;
        }
    }

    return true;
}

void displayMenu() {
    cout << "\n=================\n    MAIN MENU    \n=================\n";
    cout << "\n1. Display all ECUs";
    cout << "\n2. Scan vehicle";
    cout << "\n3. Display vehicle health";
    cout << "\n4. Add fault to ECU";
    cout << "\n5. Clear faults from ECU";
    cout << "\n6. Add ECU";
    cout << "\n7. Display diagnostic log";
    cout << "\n8. Set ECU communication status";
    cout << "\n9. Display ECU fault history";
    cout << "\n10. Display Diagnostic Session";
    cout << "\n11. Set Diagnostic Session";
    cout << "\n12. Exit\n" << '\n';
}

void addFaultsToECU(Vehicle& vehicle) {
    string ecuName;
    string faultCode;
    string faultDescription;
    Severity severity;
    int severityIndex;

    cout << "Which ECU to add the fault to?\n";
    getFullTextInput(ecuName);

    bool ECUCheck = getValidECUName(ecuName, vehicle);

    if (!ECUCheck) {
        return;
    }

    cout <<"Enter fault code: ";
    cin >> faultCode;

    cout << "Enter fault description: ";
    getFullTextInput(faultDescription);

    cout << "Choose severity:\n";
    cout << "1. Low\n" << "2. Medium\n" << "3. High\n\n";
    cin >> severityIndex;
    validateInputIsInt(severityIndex);

    while (severityIndex < 1 || severityIndex > 3) {
        if (cin.fail()) {
            clearLine();
            cout << "\nPlease enter a number, not a character, or enter -1 to go back to the menu: ";
            cin >> severityIndex;
        } else {
            cout << "\nPlease enter a number between 1 and 3, or enter -1 to go back to the menu: ";
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
    cout << "\nFault added to " << ecuName << " ECU" << '\n';
    return;
}

void addECUViaMenu(Vehicle& vehicle) {
    string ecuName;

    cout << "\nEnter ECU name to be added to " << vehicle.getName() << ": ";

    getFullTextInput(ecuName);

    bool added = vehicle.addECU(ECU(ecuName), true);

    while (!added) {
        cout << '\n' << ecuName << " ECU already exists in this Vehicle. Please enter a new ECU name, or enter -1 to go back to the menu: ";
        getFullTextInput(ecuName);

        if (ecuName == "-1") {
            return;
        }

        added = vehicle.addECU(ECU(ecuName), true);
    }

    if (added) {
        cout << "\nECU added successfully!\n";
    }
}

void clearFaultsMenu(Vehicle& vehicle) {
    string ecuName;
    cout << "\nEnter ECU Name: ";
    getFullTextInput(ecuName);

    ClearFaultResult clearFault = vehicle.clearFaultsFromECU(ecuName);

    while(clearFault != ClearFaultResult::FaultsCleared) {
        if (clearFault == ClearFaultResult::SessionNotAllowed) {
            cout << "\nCannot clear faults in Default Session. Switch to Extended Session first.\n";
            return;
        } else if (clearFault == ClearFaultResult::NoFaultsToClear) {
            cout << '\n' << ecuName << " ECU has no faults. Enter another ECU or enter -1 to go back to the menu: ";
            getFullTextInput(ecuName);
        } else if (clearFault == ClearFaultResult::ECUNotFound) {
            cout << '\n' << ecuName << " ECU does not exist in this vehicle. Enter another ECU or enter -1 to go back to the menu: ";
            getFullTextInput(ecuName);
        } else if (clearFault == ClearFaultResult::ECUOffline) {
            cout << '\n' << ecuName << " ECU is currently offline. Enter an Online ECU or enter -1 to go back to the menu: ";
            getFullTextInput(ecuName);
        }

        if (ecuName == "-1") {
            return;
        }

        clearFault = vehicle.clearFaultsFromECU(ecuName);
    }

    cout << "Faults cleared from " << ecuName << " ECU\n";
    return;
}

void setStatusMenu(Vehicle& vehicle) {
    string ecuName;
    int statusIndex;
    ECUStatus status;
    cout << "\nEnter ECU name: ";
    getFullTextInput(ecuName);

    bool ECUCheck = getValidECUName(ecuName, vehicle);

    if (!ECUCheck) return;
    
    cout << "Choose status:\n";
    cout << "1. Online\n" << "2. Offline\n\n";
    cin >> statusIndex;
    validateInputIsInt(statusIndex);

    while (statusIndex < 1 || statusIndex > 2) {
        if (cin.fail()) {
            clearLine();
            cout << "\nPlease enter a number, not a character, or enter -1 to go back to the menu: ";
            cin >> statusIndex;
        } else {
            cout << "\nPlease enter a number between 1 and 2, or enter -1 to go back to the menu: ";
            cin >> statusIndex;
        }

        if (statusIndex == -1) return;
    }

    switch(statusIndex) {
        case 1:
            status = ECUStatus::Online;
            break;
        case 2:
            status = ECUStatus::Offline;
            break;
    }

    ECUStatusResult set = vehicle.setECUStatusByName(ecuName, status);

    if (set == ECUStatusResult::AlreadyInRequestedState) {
        cout << '\n' << ecuName << " ECU was already " << ((status == ECUStatus::Online) ? "Online" : "Offline") << '\n';
        return;
    }

    cout << '\n' << ecuName << " ECU status changed!\n";

    return;
}

void ECUFaultHistoryMenu(Vehicle& vehicle) {
    string ecuName;

    cout << "Enter ECU name: ";
    getFullTextInput(ecuName);

    bool ECUCheck = getValidECUName(ecuName, vehicle);

    if (!ECUCheck) return;

    bool displayed = vehicle.displayECUFaultHistory(ecuName);

    return;
}

void setDiagnosticSessionMenu(Vehicle& vehicle) {
    int option;
    VehicleSession session;

    cout << "\n1. Default Session" << "\n2. Extended Session\n";
    cout << "\nPick a session: ";

    cin >> option;

    validateInputIsInt(option);
    while (option < 1 || option > 2) {
        cout << "\nPlease enter a number between 1 and 2. Or enter -1 to return to the menu: ";

        cin >> option;

        validateInputIsInt(option);

        if (option == -1) {
            return;
        }
    }

    switch(option) {
        case 1:
            session = VehicleSession::DefaultSession;
            break;
        case 2:
            session = VehicleSession::ExtendedSession;
            break;
    }

    DiagnosticSessionResult set = vehicle.setDiagnosticSession(session);

    if (set == DiagnosticSessionResult::AlreadyInRequestedState) {
        cout << "\nVehicle was already in" << ((session == VehicleSession::DefaultSession) ? "Default" : "Extended") << " Session" << '\n';
        return;
    }

    cout << "\nVehicle session changed!\n";

    return;
}

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

    while (option != 12) {
        if (option < 1 || option > 12) {
            cout << "Please type in a number between 1 and 12: ";
            cin >> option;
            validateInputIsInt(option);
        }
        else if (option > 0 && option < 12) {
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
            }

            displayMenu();

            cout << "Choose an option: ";
            cin >> option;

            validateInputIsInt(option);
        }
    }
    return 0;
}