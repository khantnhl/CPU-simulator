#ifndef ALU_H
#define ALU_H

#include <cstdint>
#include <assert.h>

class ALU {
private:
    bool zero_flag;

public:
    // Constructor
    ALU();
    // Execute
    int32_t execute(uint8_t alu_operation, int32_t operand1, int32_t operand2);
    // Check if zero
    bool is_zero() const;
};

#endif // ALU_H