#include <iostream>
#include <cassert>

#include "domain/ecuTests.h"
#include "uds/UDSRequestParserTests.h"

int main() {
    ecuTests();
    udsRequestParserTests();

    std::cout << "\nAll Tests  Passed!\n";
    return 0;
}