#include "cptrie.hpp"

namespace cptrie {

// used just to build
struct Node {
    std::unordered_map<char, std::unique_ptr<Node>> children;
    std::optional<uint64_t> value;
};

std::vector<char> build(cptrie::Node const * root);

} // namespace cptrie