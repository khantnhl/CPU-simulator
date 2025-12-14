#include "ImmediateGenerator.h"
#include "Controller.h"

/**
 * Generates immediate value based on instruction 32 bits.
 * Different instruction types (I, S, B, U) have different immediate field layouts.
 * This function extracts the immediate bits and performs sign extension.
 */
int32_t ImmediateGenerator::generate(uint32_t instruction, uint8_t opcode) {
    uint32_t imm;
    int32_t immediate;
    
    switch (opcode) {
        case ITYPE: // I-type (arithmetic immediate)
            imm = instruction >> 20;
            // Sign extend from the 12th bit
            immediate = (int32_t)(imm << 20) >> 20;
            break;
            
        case LW: // I-type (load)
            imm = instruction >> 20;
            // Sign extend from the 12th bit
            immediate = (int32_t)(imm << 20) >> 20;
            break;
            
        case JALR: // (jump and link register)
            imm = instruction >> 20;
            // Sign extend from the 12th bit
            immediate = (int32_t)(imm << 20) >> 20;
            break;
            
        case SW: // S-type (store)
            // S-type: imm[11:5] in bits [31:25], imm[4:0] in bits [11:7]
            imm = ((instruction >> 7) & 0x1F) | ((instruction >> 25) << 5);
            // Sign extend from the 12th bit
            immediate = (int32_t)(imm << 20) >> 20;
            break;
            
        case BNE: // B-type (branch)
            // B-type: imm[12|10:5] in bits [31:25], imm[4:1|11] in bits [11:7]
            imm = ((instruction >> 8) & 0xF) << 1 |
                  ((instruction >> 25) & 0x3F) << 5 |
                  ((instruction >> 7) & 0x1) << 11 |
                  ((instruction >> 31) & 0x1) << 12;
            // Sign extend from the 13th bit
            immediate = (int32_t)(imm << 19) >> 19;
            break;
                  
        case LUI: // U-type (load upper immediate)
            // U-type: imm[31:12] in bits [31:12]
            immediate = (int32_t)(instruction & 0xFFFFF000);
            break;
            
        default: 
            immediate = 0;
            break;
    }
    
    return immediate;      
}

