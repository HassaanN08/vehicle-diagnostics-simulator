#include <iostream>
#include <cassert>

#include "domain/ecuTests.h"
#include "uds/UDSServerTests.h"
#include "isotp/isoTpTests.h"
#include "can/canFrameTests.h"
#include "implementation/diagnosticCoordinatorTests.h"

int main() {
    ecuTests();
    udsServerTests();
    isoTpTests();
    canFrameTests();
    diagnosticCoordinatorTests();

    std::cout << "\nAll Tests  Passed!\n";
    return 0;
}