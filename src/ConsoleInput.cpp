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

bool validateInputIsIntAndWithinRange(int& input, const int& lowerRange, const int& upperRange) {
    while (std::cin.fail() || input > upperRange || input < lowerRange) {
        if (std::cin.fail()) {
            clearLine();
            std::cout << "Please type in a number, not a character: ";
        } else if(input > upperRange || input < lowerRange) {
            clearLine();
            std::cout << "Please type in a number between " << lowerRange << " and " << upperRange << ", or enter -1 to cancel: ";
        }
        std::cin >> input;

        if (input == -1) {
            return false;
        }
    }

    return true;
}