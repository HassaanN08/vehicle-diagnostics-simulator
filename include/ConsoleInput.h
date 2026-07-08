#pragma once
#include <string>

void getFullTextInput(std::string& input);

void clearLine();

void validateInputIsInt(int& input);

bool validateInputIsIntAndWithinRange(int& input, const int& lowerRange, const int& upperRange);