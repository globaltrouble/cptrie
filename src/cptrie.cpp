#include "cptrie.hpp"

#include <cstring>
#include <cassert>

namespace cptrie {

std::optional<uint64_t> get(char const * key, uint32_t offset, char const * dataBegin) {
    char const * ptr = dataBegin + offset;

    static_assert(sizeof(char) == sizeof(unsigned char));
    unsigned char childrenCount = *reinterpret_cast<unsigned char const *>(ptr);
    ptr += sizeof(childrenCount);
    
    static_assert(sizeof(bool) == sizeof(unsigned char));
    bool hasValue = *reinterpret_cast<bool const *>(ptr);
    ptr += sizeof(hasValue);

    char const * childKey = ptr;
    ptr += sizeof(*childKey) * childrenCount;

    char const * childOffset = ptr;
    ptr += sizeof(uint32_t) * childrenCount;

    char const * valueData = ptr;

    if (*key == '\0') {
        if (!hasValue) {
            return std::nullopt;
        }

        char localBuff[sizeof(uint64_t)];
        std::memcpy(localBuff, valueData, sizeof(uint64_t));

        // will fail on mismatch big/little endian
        return { *reinterpret_cast<uint64_t const *>(localBuff) };
    }

    for (int i = 0; i < childrenCount; i++) {
        if (*(childKey + i) == *key) {
            char localBuff[sizeof(uint32_t)];
            std::memcpy(localBuff, childOffset + i * sizeof(uint32_t), sizeof(uint32_t));
            uint32_t nextOffset = *reinterpret_cast<uint64_t const *>(localBuff);
            return get(key+1, nextOffset, dataBegin);
        }
    }

    return std::nullopt;
}

std::optional<uint64_t> get(char const * key, char const * dataBegin) {
    return get(key, 0, dataBegin);
}


} // namespace cptrie