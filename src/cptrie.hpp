#ifndef CPTRIE_H
#define CPTRIE_H

#include <cstdint>
#include <optional>
#include <unordered_map>
#include <memory>
#include <vector>

namespace cptrie {

// used just to build
struct Node {
    std::unordered_map<char, std::unique_ptr<Node>> children;
    std::optional<uint64_t> value;
};

std::optional<uint64_t> get(char const * key, uint32_t offset, char const * dataBegin);

std::vector<char> build(cptrie::Node const * root);

} // namespace cptrie


#endif