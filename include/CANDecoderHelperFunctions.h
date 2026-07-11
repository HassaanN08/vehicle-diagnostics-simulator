#pragma once
#include <string>
#include <vector>
#include "DecodedCANFrame.h"
#include <cstdint>

void decodeEngineFrame(DecodedCANFrame& decodedData, const std::vector<uint8_t>& dataBytes);

void decodeBrakeFrame(DecodedCANFrame& decodedData, const std::vector<uint8_t>& dataBytes);

void decodeBatteryFrame(DecodedCANFrame& decodedData, const std::vector<uint8_t>& dataBytes);