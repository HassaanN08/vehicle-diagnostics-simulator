#include <iostream>
#include <string>
#include <vector>
#include "DTC.h"
#include "Severity.h"
using namespace std;

int main() {
    vector<DTC> codes;

    codes.push_back(DTC("P0301", "Cylinder 1 Misfire Detected", Severity::High));

    codes.push_back(DTC("P0171", "System Too Lean", Severity::Medium));
    
    codes.push_back(DTC("B1001", "Battery Voltage Low", Severity::Low));

    cout << "\nDiagnostic Trouble Codes:\n";

    for (const DTC& code : codes) {
        code.display();
    }

    
    return 0;
}