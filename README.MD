# CPTRIE

Compile time trie build, key is a string, value is uint64_t.
Used to build blob with trie data. Pre build trie blob significantly reduce library load time (no allocations, no parsing).

### usage:

#### Build library
```
mkdir build && cd build && cmake --DCMAKE_BUILD_TYPE=Relase .. && cmake --build .
```

#### Build binary blob
```
# build-cptrie will be available in build directory
./build-cptrie full/path/to/input-mapping.txt full/path/to/output/binary-blob
```

this step require file `full/path/to/input-mapping.txt` to have next format:
```
keyTextSize|keyText|keyValue
```

* keyTextSize must be a valid integer
* keyText any char* string
* strlen(keyText) == keyTextSize is true
* keyValue is a valid uin64_t value
* each key-value pair must be placed on a new line
* all collumns (keyTextSize, keyText, keyValue) must be separated with `|` separator

Input file example:
```
13|abcdefgFJlfda|57287014
18|gjlkjf;aDDfaaeeeaa|1598539636
1|!|42
8|00392157|539636
```

Output blob will be saved at `full/path/to/output/binary-blob`

#### Use CPTRIE library in your project

The only function to use is `cptrie::get`
```
    char const * fname = "full/path/to/output/binary-blob";
    std::ifstream blob(fname);
    blob.seekg(0, std::ifstream::end);

    size_t size = blob.tellg();
    std::cerr << "Blob size: " << size << std::endl;

    std::vector<char> blobData(size);
    blob.seekg(0, std::ifstream::beg);
    blob.read(blobData.data(), size);
    auto result = cptrie::get("yourKey", blobData.data());
    if (!result.has_value()) {
        std::cout << "No value for found" << std::endl;
    } else {
        std::cout << "Found value: " << result.value() << std::endl;
    }
```

#### Optinal binary bloab can be loaded to binary at compile time
* or [directly linked with your binary at compile time](https://tratt.net/laurie/blog/2022/whats_the_most_portable_way_to_include_binary_blobs_in_an_executable.html)
* or use `std::embed` from C++23