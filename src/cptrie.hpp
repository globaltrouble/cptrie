#ifndef CPTRIE_H
#define CPTRIE_H

#include <cstdint>
#include <optional>
#include <unordered_map>
#include <memory>
#include <vector>

namespace cptrie {

/**
 * @brief Search for value by key in trie blob.
 * 
 * @param key C-style string ('\0' terminated) to search by
 * @param dataBegin pointer to first byte of binary blob of trie
 * @return std::nullopt in case target key was'n in trie, else its uin64_t value 
*/
std::optional<uint64_t> get(char const * key, char const * dataBegin);

} // namespace cptrie


#endif