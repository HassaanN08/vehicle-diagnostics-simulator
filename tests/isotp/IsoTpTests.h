#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <cstdint>

#include "isotp/IsoTp.h"

void isoTpTests() {
    std::vector<std::uint8_t> validOneBytePayload {0x10};
    std::vector<std::uint8_t> validTwoBytePayload {0x10, 0x03};
    std::vector<std::uint8_t> validSevenBytePayload {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    std::vector<std::uint8_t> validOneByteEncodedPayload {0x01, 0x10};
    std::vector<std::uint8_t> validTwoByteEncodedPayload {0x02, 0x10, 0x03};
    std::vector<std::uint8_t> validSevenByteEncodedPayload {0x07, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07}; 
    std::vector<std::uint8_t> validEightByteEncodedPayload {0x07, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};    
    std::vector<std::uint8_t> validEmptyEncodedPayload {};
    std::vector<std::uint8_t> invalidOneByteEncodedPayload {0x11, 0x10};

    assert(IsoTp::encode(validOneBytePayload) == validOneByteEncodedPayload);
    assert(IsoTp::encode(validTwoBytePayload) == validTwoByteEncodedPayload);
    assert(IsoTp::encode(validSevenBytePayload) == validSevenByteEncodedPayload);
    assert(IsoTp::encode({}) == validEmptyEncodedPayload);
    assert(IsoTp::encode({0x07, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}) == validEmptyEncodedPayload);
    

    assert(IsoTp::decode(validOneByteEncodedPayload) == validOneBytePayload);
    assert(IsoTp::decode(validTwoByteEncodedPayload) == validTwoBytePayload);
    assert(IsoTp::decode(validSevenByteEncodedPayload) == validSevenBytePayload);
    assert(!IsoTp::decode({}).has_value());
    assert(!IsoTp::decode(invalidOneByteEncodedPayload).has_value());
    assert(IsoTp::decode(IsoTp::encode(validSevenBytePayload)) == validSevenBytePayload);
}