#include "build.hpp"
#include "cptrie.hpp"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <optional>
#include <memory>
#include <cassert>
#include <functional>

void iterOverFile(char const * fname, std::function<void(std::string const & key, uint64_t value)> && callback) {
    std::ifstream inpt(fname);
    std::string buf;
    buf.reserve(8192);

    constexpr char delimeter = '|';
    constexpr int intBufSize = 128;
    char intBuf[intBufSize];

    int i = 1;
    for (; getline(inpt, buf); i++) {
        auto begin = buf.cbegin();
        auto end = std::find(buf.cbegin(), buf.cend(), delimeter);
        auto dist = std::distance(begin, end);
        if (end == buf.cend() || end == begin || dist >= intBufSize || *end != delimeter) {
            std::cerr << "Err, no keysize end, line number: " << (i+1) <<  ", line: `" << buf << "`\n";
            return std::exit(2);
        }

        std::copy(begin, end, intBuf);
        intBuf[dist] = '\0';
        
        uint32_t size = std::atoll(intBuf);
        if (size == 0) {
            std::cerr << "Can't read string size, line number: " << (i+1) << ", line: `" << buf << "`\n";
            return std::exit(3);
        }

        // skip delimeter
        begin = end + 1;
        end = begin + size;
        if (end == buf.cend() || end == begin || *end != delimeter) {
            std::cerr << "End: " << static_cast<int>(*end) << "\n";
            std::cerr << "Err, no delimeter after key, line number: " << (i+1) << ", line: `" << buf << "`\n";
            std::exit(4);
        }

        // skip delimeter;
        auto valueBegin = end+1;
        dist = std::distance(valueBegin, buf.cend());
        if (valueBegin == buf.cend() || dist > intBufSize) {
            std::cerr << "Err, no value for key, line number: " << (i+1) << ", line: `" << buf << "`\n";
            std::exit(5);
        }

        std::copy(valueBegin, buf.cend(), intBuf);
        intBuf[dist] = '\0';
        uint64_t value = std::atoll(intBuf);
        if (value == 0) {
            std::cerr << "Can't read value, line number: " << (i+1) << ", line: `" << buf << "`\n";
            std::exit(6);
        }

        callback({begin, end}, value);
    }
    std::cerr << "Read " << i << " lines\n";
}

int main(int argc, char const * const * argv) {
    if (argc != 3) {
        std::cerr << "Expected argument count = 3. usage `prog path/to-dictionary path/to/output-trie-blob`\n";
        return 1;
    }

    char const * inputFname = argv[1];

    std::unique_ptr<cptrie::Node> root = std::make_unique<cptrie::Node>();
    int n = 0;

    iterOverFile(inputFname, [&root, &n](std::string const & key, uint64_t value) {
        std::unordered_map<char, std::unique_ptr<cptrie::Node>>* nodes = &root->children;

        for (auto begin = key.cbegin(); begin < key.cend(); begin++) {
            char c = *begin;
            auto nIt = nodes->find(c);
            if (nIt == nodes->end()) {
                nIt = nodes->insert({c, std::make_unique<cptrie::Node>()}).first;
                n++;
            }
            if (begin+1 == key.cend()) {
                nIt->second->value.emplace(value);
            }
            nodes = &nIt->second->children;
        }
    });

    std::cerr << "Parsed " << n << " entries\n";
    auto trieData = cptrie::build(root.get());
    std::cerr << "Trie size: " << trieData.size() << "\n";
    std::cerr << "Testing trie...\n";

    iterOverFile(inputFname, [&root, &n, data = trieData.data()](std::string const & key, uint64_t value) {
        std::unordered_map<char, std::unique_ptr<cptrie::Node>>* nodes = &root->children;

        for (auto begin = key.cbegin(); begin < key.cend(); begin++) {
            char c = *begin;
            auto nIt = nodes->find(c);
            if (nIt == nodes->end()) {
                std::cerr << "Can't find key: `" << key << "`, val: " << value << "\n";
                std::exit(7);
            }

            if (begin+1 == key.cend()) {
                if (!nIt->second->value.has_value()) {
                    std::cerr << "TempValue for `" << key << "` was not found, expected: " << value << "\n";
                    std::exit(8);
                }
                uint64_t tempTrieValue = nIt->second->value.value();
                if (tempTrieValue != value) {
                    std::cerr << "TempTrieValue mismatch for `" << key << "`, expected: " << value << ", got: " << tempTrieValue << "\n";
                    std::exit(9);
                }

                auto finalTrieValue = cptrie::get(key.c_str(), data);
                if (!finalTrieValue.has_value()) {
                    std::cerr << "FinalValue for `" << key << "` was not found, expected: " << value << "\n";
                    std::exit(10);
                }

                if (finalTrieValue.value() != value) {
                    std::cerr << "FinalValue mismatch for `" << key << "`, expected: " << value << ", got: " << finalTrieValue.value() << "\n";
                    std::exit(11);
                }
            }
            nodes = &nIt->second->children;
        }
    });

    char const * outputFname = argv[2];
    std::cerr << "Tested trie, all is OK!\nFlushing final trie to " << outputFname << "\n";

    {
        std::ofstream blob(outputFname);
        blob.write(trieData.data(), trieData.size());

        std::cerr << "Blob size after write: " << blob.tellp() << std::endl;
    }

    {
        std::ifstream blob(outputFname);
        blob.seekg(0, std::ifstream::end);

        size_t size = blob.tellg();
        std::cerr << "Blob size before read: " << size << "\nTesting trie from disk...\n";

        std::vector<char> blobData(size);
        blob.seekg(0, std::ifstream::beg);
        blob.read(blobData.data(), size);

        iterOverFile(inputFname, [&root, &n, data = blobData.data()](std::string const & key, uint64_t value) {
            std::unordered_map<char, std::unique_ptr<cptrie::Node>>* nodes = &root->children;

            for (auto begin = key.cbegin(); begin < key.cend(); begin++) {
                char c = *begin;
                auto nIt = nodes->find(c);
                if (nIt == nodes->end()) {
                    std::cerr << "Can't find key: `" << key << "`, val: " << value << "\n";
                    std::exit(7);
                }

                if (begin+1 == key.cend()) {
                    if (!nIt->second->value.has_value()) {
                        std::cerr << "TempValue for `" << key << "` was not found, expected: " << value << "\n";
                        std::exit(8);
                    }
                    uint64_t tempTrieValue = nIt->second->value.value();
                    if (tempTrieValue != value) {
                        std::cerr << "TempTrieValue mismatch for `" << key << "`, expected: " << value << ", got: " << tempTrieValue << "\n";
                        std::exit(9);
                    }

                    auto finalTrieValue = cptrie::get(key.c_str(), data);
                    if (!finalTrieValue.has_value()) {
                        std::cerr << "FinalValue for `" << key << "` was not found, expected: " << value << "\n";
                        std::exit(10);
                    }

                    if (finalTrieValue.value() != value) {
                        std::cerr << "FinalValue mismatch for `" << key << "`, expected: " << value << ", got: " << finalTrieValue.value() << "\n";
                        std::exit(11);
                    }
                }
                nodes = &nIt->second->children;
            }
        });
        
        std::cerr << "All is OK, trie at: `" << outputFname << "` can be used!\n";
    }
    
    return 0;
}