#pragma once

#include <cstdint>
#include <vector>

#include "domain/ECU.h"

class UDSResponse {
        std::uint8_t m_positiveServiceID { };

        std::uint8_t m_notSupportedNRC { 0x11 };      //Requested Subfunction isn't supported
        std::uint8_t m_incorrectLengthNRC { 0x13 };   //Incorrect Payload Length

        public:
            std::uint8_t getPositiveSID() const { return m_positiveServiceID; }

            std::uint8_t getNotSupportedNRC() const { return m_notSupportedNRC; }

            std::uint8_t getIncorrectLengthNRC() const { return m_incorrectLengthNRC; }

            void setPositiveSID(std::uint8_t originalSID);

            bool setDiagnosticSessionControl(ECU& ecu, std::uint8_t requestByte);
};