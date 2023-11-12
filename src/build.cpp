#include "build.hpp"

#include <cstring>
#include <cassert>

namespace cptrie {

void set(cptrie::Node const * node, std::vector<char> & data) {
    assert(node != nullptr);

    uint32_t offset = data.size();
    unsigned char childrenCount = node->children.size();
    bool hasValue = node->value.has_value();

    uint32_t size = sizeof(childrenCount) + sizeof(hasValue) + childrenCount * sizeof(char) + childrenCount * sizeof(uint32_t);
    if (hasValue) {
        size += sizeof(uint64_t);
    }

    if (offset + size >= data.capacity()) {
        data.reserve(data.capacity() * 2);
    }
    
    assert(offset + size < data.capacity());
    data.resize(offset + size);


    std::memcpy(data.data() + offset, &childrenCount, sizeof(childrenCount));
    offset += sizeof(childrenCount);

    std::memcpy(data.data() + offset, &hasValue, sizeof(hasValue));
    offset += sizeof(hasValue);

    int i = 0;
    for (auto const & p : node->children) {
        char * cPtr = data.data() + offset + i * sizeof(char);
        *cPtr = p.first;

        uint32_t offsetPtr = offset + sizeof(char) * childrenCount + i * sizeof(uint32_t);
        uint32_t nextOffset = data.size();
        std::memcpy(data.data() + offsetPtr, &nextOffset, sizeof(nextOffset));

        set(p.second.get(), data);

        i++;
    }

    offset += sizeof(char) * childrenCount + sizeof(uint32_t) * childrenCount;

    if (hasValue) {
        uint64_t val = node->value.value();
        std::memcpy(data.data() + offset, &val, sizeof(uint64_t));
    }
}

std::vector<char> build(cptrie::Node const * root) {
    std::vector<char> data;
    data.reserve(1024);

    set(root, data);

    return data;
}

} // namespace cptrie
