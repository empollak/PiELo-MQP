#include "instructions.h"

namespace PiELo {

    // Load a file into memory (parses it)
    // Returns 1 on success, 0 otherwise
    std::vector<uint8_t>* load(std::string file);

    // Returns -1 on error, 0 on normal exit, 1 if continuing
    int step();
}