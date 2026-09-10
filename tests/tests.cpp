#include <iostream>
#include <cassert>

#include "domain/ecuTests.h"
#include "uds/UDSServerTests.h"

int main() {
    ecuTests();
    udsServerTests();

    std::cout << "\nAll Tests  Passed!\n";
    return 0;
}