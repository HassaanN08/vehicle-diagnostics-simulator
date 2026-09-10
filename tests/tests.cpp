#include <iostream>
#include <cassert>

#include "domain/ecuTests.h"
#include "uds/UDSServerTests.h"
#include "isotp/IsoTpTests.h"

int main() {
    ecuTests();
    udsServerTests();
    isoTpTests();

    std::cout << "\nAll Tests  Passed!\n";
    return 0;
}