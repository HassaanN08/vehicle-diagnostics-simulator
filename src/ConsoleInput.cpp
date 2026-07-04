#include <iostream>
#include <limits>
#include <string>
#include "ConsoleInput.h"

void getFullTextInput(std::string& input) {
    getline(std::cin >> std::ws, input);
}

void clearLine() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void validateInputIsInt(int& input) {
    while (std::cin.fail()) {
        clearLine();
        std::cout << "Please type in a number, not a character: ";
        std::cin >> input;
    }

    return;
}