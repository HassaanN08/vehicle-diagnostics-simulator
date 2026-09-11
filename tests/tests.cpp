#include <iostream>
#include <cassert>

#include "domain/ecuTests.h"
#include "uds/UDSServerTests.h"
#include "isotp/IsoTpTests.h"
#include "can/canFrameTests.h"

int main() {
    ecuTests();
    udsServerTests();
    isoTpTests();
    canFrameTests();

    std::cout << "\nAll Tests  Passed!\n";
    return 0;
}