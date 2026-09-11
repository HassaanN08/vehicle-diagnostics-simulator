#include <iostream>
#include <cassert>

#include "domain/ecuTests.h"
#include "uds/UDSServerTests.h"
#include "isotp/isoTpTests.h"
#include "can/canFrameTests.h"
#include "can/canRouterTests.h"
#include "implementation/diagnosticCoordinatorTests.h"

int main() {
    ecuTests();
    udsServerTests();
    isoTpTests();
    canFrameTests();
    diagnosticCoordinatorTests();
    canRouterTests();

    std::cout << "\nAll Tests  Passed!\n";
    return 0;
}