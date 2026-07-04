#pragma once
#include "Vehicle.h"
#include <string>

bool getValidECUName(std::string& ecuName, Vehicle& vehicle);

void displayMenu();

void addFaultsToECU(Vehicle& vehicle);

void addECUViaMenu(Vehicle& vehicle);

void clearFaultsMenu(Vehicle& vehicle);

void setStatusMenu(Vehicle& vehicle);

void ECUFaultHistoryMenu(Vehicle& vehicle);

void setDiagnosticSessionMenu(Vehicle& vehicle);