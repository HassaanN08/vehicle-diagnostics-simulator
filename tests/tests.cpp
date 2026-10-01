#include <iostream>
#include <cassert>

#include "domain/ecuTests.h"
#include "uds/UDSServerTests.h"
#include "isotp/isoTpTests.h"
#include "can/canFrameTests.h"
#include "implementation/diagnosticCoordinatorTests.h"
#include "domain/vehicleTests.h"
#include "uds/readDataByIdentifierTests.h"
#include "uds/clearDiagnosticInformationTests.h"
#include "isotp/receiverTests.h"
#include "isotp/senderTests.h"
#include "implementation/diagnosticRuntimeTests.h"

int main() {
    ecuTests();
    udsServerTests();
    isoTpTests();
    canFrameTests();
    diagnosticCoordinatorTests();
    vehicleTests();
    readDataByIdentifierTests();
    clearDiagnosticInformationTests();
    receiverTests();
    senderTests();
    diagnosticRuntimeTests();

    std::cout << "\nAll Tests  Passed!\n";
    return 0;
}