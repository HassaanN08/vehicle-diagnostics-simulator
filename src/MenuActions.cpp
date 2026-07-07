#include <iostream>
#include <string>
#include "Severity.h"
#include "ECUStatus.h"
#include "DTC.h"
#include "ECU.h"
#include "Vehicle.h"
#include "VehicleResults.h"
#include "VehicleSession.h"
#include "ConsoleInput.h"
#include "MenuActions.h"
#include "CANFrame.h"

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
    std::cout << "\n=================\n    MAIN MENU    \n=================\n";
    std::cout << "\n1. Display all ECUs";
    std::cout << "\n2. Scan vehicle";
    std::cout << "\n3. Display vehicle health";
    std::cout << "\n4. Add fault to ECU";
    std::cout << "\n5. Clear faults from ECU";
    std::cout << "\n6. Add ECU";
    std::cout << "\n7. Display diagnostic log";
    std::cout << "\n8. Set ECU communication status";
    std::cout << "\n9. Display ECU fault history";
    std::cout << "\n10. Display Diagnostic Session";
    std::cout << "\n11. Set Diagnostic Session";
    std::cout << "\n12. Simulate sample CAN traffic";
    std::cout << "\n13. Display CAN bus traffic";
    std::cout << "\n14. Exit\n" << '\n';
}

void addFaultsToECU(Vehicle& vehicle) {
    std::string ecuName;
    std::string faultCode;
    std::string faultDescription;
    Severity severity;
    int severityIndex;

    std::cout << "Which ECU to add the fault to?\n";
    getFullTextInput(ecuName);

    bool ECUCheck = getValidECUName(ecuName, vehicle);

    if (!ECUCheck) {
        return;
    }

    std::cout <<"Enter fault code: ";
    std::cin >> faultCode;

    std::cout << "Enter fault description: ";
    getFullTextInput(faultDescription);

    std::cout << "Choose severity:\n";
    std::cout << "1. Low\n" << "2. Medium\n" << "3. High\n\n";
    std::cin >> severityIndex;
    validateInputIsInt(severityIndex);

    while (severityIndex < 1 || severityIndex > 3) {
        if (std::cin.fail()) {
            clearLine();
            std::cout << "\nPlease enter a number, not a character, or enter -1 to go back to the menu: ";
            std::cin >> severityIndex;
        } else {
            std::cout << "\nPlease enter a number between 1 and 3, or enter -1 to go back to the menu: ";
            std::cin >> severityIndex;
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
    std::cout << "\nFault added to " << ecuName << " ECU" << '\n';
    return;
}

void addECUViaMenu(Vehicle& vehicle) {
    std::string ecuName;

    std::cout << "\nEnter ECU name to be added to " << vehicle.getName() << ": ";

    getFullTextInput(ecuName);

    bool added = vehicle.addECU(ECU(ecuName), true);

    while (!added) {
        std::cout << '\n' << ecuName << " ECU already exists in this Vehicle. Please enter a new ECU name, or enter -1 to go back to the menu: ";
        getFullTextInput(ecuName);

        if (ecuName == "-1") {
            return;
        }

        added = vehicle.addECU(ECU(ecuName), true);
    }

    if (added) {
        std::cout << "\nECU added successfully!\n";
    }
}

void clearFaultsMenu(Vehicle& vehicle) {
    std::string ecuName;
    std::cout << "\nEnter ECU Name: ";
    getFullTextInput(ecuName);

    ClearFaultResult clearFault = vehicle.clearFaultsFromECU(ecuName);

    while(clearFault != ClearFaultResult::FaultsCleared) {
        if (clearFault == ClearFaultResult::SessionNotAllowed) {
            std::cout << "\nCannot clear faults in Default Session. Switch to Extended Session first.\n";
            return;
        } else if (clearFault == ClearFaultResult::NoFaultsToClear) {
            std::cout << '\n' << ecuName << " ECU has no faults. Enter another ECU or enter -1 to go back to the menu: ";
            getFullTextInput(ecuName);
        } else if (clearFault == ClearFaultResult::ECUNotFound) {
            std::cout << '\n' << ecuName << " ECU does not exist in this vehicle. Enter another ECU or enter -1 to go back to the menu: ";
            getFullTextInput(ecuName);
        } else if (clearFault == ClearFaultResult::ECUOffline) {
            std::cout << '\n' << ecuName << " ECU is currently offline. Enter an Online ECU or enter -1 to go back to the menu: ";
            getFullTextInput(ecuName);
        }

        if (ecuName == "-1") {
            return;
        }

        clearFault = vehicle.clearFaultsFromECU(ecuName);
    }

    std::cout << "Faults cleared from " << ecuName << " ECU\n";
    return;
}

void setStatusMenu(Vehicle& vehicle) {
    std::string ecuName;
    int statusIndex;
    ECUStatus status;
    std::cout << "\nEnter ECU name: ";
    getFullTextInput(ecuName);

    bool ECUCheck = getValidECUName(ecuName, vehicle);

    if (!ECUCheck) return;
    
    std::cout << "Choose status:\n";
    std::cout << "1. Online\n" << "2. Offline\n\n";
    std::cin >> statusIndex;
    validateInputIsInt(statusIndex);

    while (statusIndex < 1 || statusIndex > 2) {
        if (std::cin.fail()) {
            clearLine();
            std::cout << "\nPlease enter a number, not a character, or enter -1 to go back to the menu: ";
            std::cin >> statusIndex;
        } else {
            std::cout << "\nPlease enter a number between 1 and 2, or enter -1 to go back to the menu: ";
            std::cin >> statusIndex;
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
        std::cout << '\n' << ecuName << " ECU was already " << ((status == ECUStatus::Online) ? "Online" : "Offline") << '\n';
        return;
    }

    std::cout << '\n' << ecuName << " ECU status changed!\n";

    return;
}

void ECUFaultHistoryMenu(Vehicle& vehicle) {
    std::string ecuName;

    std::cout << "Enter ECU name: ";
    getFullTextInput(ecuName);

    bool ECUCheck = getValidECUName(ecuName, vehicle);

    if (!ECUCheck) return;

    bool displayed = vehicle.displayECUFaultHistory(ecuName);

    return;
}

void setDiagnosticSessionMenu(Vehicle& vehicle) {
    int option;
    VehicleSession session;

    std::cout << "\n1. Default Session" << "\n2. Extended Session\n";
    std::cout << "\nPick a session: ";

    std::cin >> option;

    validateInputIsInt(option);
    while (option < 1 || option > 2) {
        std::cout << "\nPlease enter a number between 1 and 2. Or enter -1 to return to the menu: ";

        std::cin >> option;

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
        std::cout << "\nVehicle was already in " << ((session == VehicleSession::DefaultSession) ? "Default" : "Extended") << " Session" << '\n';
        return;
    }

    std::cout << "\nVehicle session changed!\n";

    return;
}

void simulateSampleCANTraffic(Vehicle& vehicle) {
    vehicle.transmitCANFrame(CANFrame(256, "Engine", {11, 184}));
    vehicle.transmitCANFrame(CANFrame(512, "Brake", {1}));
    vehicle.transmitCANFrame(CANFrame(768, "Battery", {12, 6}));

    std::cout << "\nSample CAN traffic transmitted.\n";

    return;
}

void displayCANBusTrafficMenu(Vehicle& vehicle) {
    if (vehicle.CANTrafficExists()) {
        vehicle.displayCANBusTraffic();
    } else {
        std::cout << "\nNo CAN Traffic recorded\n";
    }
}