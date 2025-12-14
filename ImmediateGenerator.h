#ifndef IMMGEN_H
#define IMMGEN_H

#include <cstdint>

// Immediate Generator - extracts and sign-extends immediate values
// from different instruction formats (I, S, B, U, J types)
class ImmediateGenerator {
public:
    // Generates immediate value based on instruction and opcode
    int32_t generate(uint32_t instruction, uint8_t opcode);
};

#endif // IMMGEN_H

