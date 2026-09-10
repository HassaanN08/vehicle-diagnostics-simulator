#pragma once

#include <vector>
#include <cstdint>
#include <optional>

namespace IsoTp {
    inline std::vector<std::uint8_t> encode (const std::vector<std::uint8_t>& payload) {
        if (payload.empty()) return {};

        std::size_t payloadLength = payload.size();

        std::vector<std::uint8_t> encodedPayload;

        if (payloadLength < 8) {
            encodedPayload.reserve(payloadLength);

            encodedPayload.push_back(static_cast<uint8_t>(payloadLength));

            encodedPayload.insert(encodedPayload.end(), payload.begin(), payload.end());
        }
        
        return encodedPayload;
    }

    inline std::optional<std::vector<std::uint8_t>> decode(const std::vector<std::uint8_t>& encodedPayload) {
        if (encodedPayload.empty()) return std::nullopt;

        std::size_t actualPayloadLength = encodedPayload[0];

        std::vector<std::uint8_t> decodedPayload;

        decodedPayload.reserve(actualPayloadLength);

        for (std::size_t i { 1 }; i < actualPayloadLength + 1; ++i) {
            decodedPayload.push_back(encodedPayload[i]);
        }

        return decodedPayload;
    }
};