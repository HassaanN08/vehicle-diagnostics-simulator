#include <iostream>
#include <cassert>

#include "domain/ecuTests.h"
#include "uds/UDSServerTests.h"
#include "isotp/isoTpTests.h"
#include "can/canFrameTests.h"
#include "implementation/transportCoordinatorTests.h"

int main() {
    ecuTests();
    udsServerTests();
    isoTpTests();
    canFrameTests();
    transportCoordinatorTests();

    std::cout << "\nAll Tests  Passed!\n";
    return 0;
}