#pragma once
#include <string>
#include <vector>
#include "DecodedCANFrame.h"
#include <cstdint>

void decodeEngineFrame(std::string& ecuName, DecodedCANFrame& decodedData, std::vector<uint8_t>& dataBytes);

void decodeBrakeFrame(std::string& ecuName, DecodedCANFrame& decodedData, std::vector<uint8_t>& dataBytes);

void decodeBatteryFrame(std::string& ecuName, DecodedCANFrame& decodedData, std::vector<uint8_t>& dataBytes);