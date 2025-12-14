#include "ALU.h"

// Constructor
ALU::ALU() : zero_flag(false) 
{}

/**
Performs an ALU operation based on the control signal.
It also sets the internal zero flag, which is used by branch instructions.
*/
int32_t ALU::execute(uint8_t alu_operation, int32_t operand1, int32_t operand2) {
    int32_t result = 0;
    switch (alu_operation) {
        case 0b0000: 
            result = operand1 & operand2; 
            break;  // AND
        case 0b0001: 
            result = operand1 | operand2; 
            break;  // OR
        case 0b0010: 
            result = operand1 + operand2; 
            break;  // ADD
        case 0b0011: 
            result = ((uint32_t)operand1 < (uint32_t)operand2) ? 1 : 0; 
            break; // SLTU
        case 0b0110: 
            result = operand1 - operand2; 
            break;  // SUB
        case 0b0111: 
            result = operand1 >> (operand2 & 0x1F); 
            break; // SRA
        default: 
            assert(false); // Should not happen
    }
    
    // The zero flag for BNE is based on subtraction, not the main result.
    zero_flag = ((operand1 - operand2) == 0);

    return result;
}

/**
Returns the zero flag state
 */
bool ALU::is_zero() const {
    return zero_flag;
}