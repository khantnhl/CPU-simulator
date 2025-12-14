#include "Controller.h"

/**
 * Generates control signals based *only on the opcode.
 * This function is direct implementation of the control logic table.
 */
ControlSignals Controller::generate_signals(uint8_t opcode) {
    
    ControlSignals signals;

    // Initialize all signals to false and 0
    signals.RegWrite = false;
    signals.MemRead = false;
    signals.MemWrite = false;
    signals.Branch = false;
    signals.ALUSrc = false;
    signals.MemtoReg = false;
    signals.ALUOp = 0;
    signals.LUISel = false;
    signals.JALRSel = false;
    signals.PCSrc = 0;
    
    // Mapping according to the controls table
    switch (opcode) {
        case RTYPE: 
			signals.RegWrite = true; 
			signals.ALUOp = 0b10; 
			signals.PCSrc = 0; // PC+4
			break; // R-type
        case ITYPE: 
			signals.RegWrite = true; 
			signals.ALUSrc = true; 
			signals.PCSrc = 0; // PC+4
			break; // I-type
        case LUI: 
			signals.RegWrite = true; 
			signals.ALUSrc = true; 
			signals.LUISel = true; // LUI selector - Use immediate instead of ALU result
			signals.PCSrc = 0; // PC+4
			break; // LUI
        case LW: 
			signals.RegWrite = true; 
			signals.MemRead = true; 
			signals.ALUSrc = true; 
			signals.MemtoReg = true; 
			signals.PCSrc = 0; // PC+4
			break; // Load
        case SW: 
			signals.MemWrite = true; 
			signals.ALUSrc = true; 
            signals.MemtoReg = true;
			signals.PCSrc = 0; // PC+4
			break; // Store
        case BNE: 
			signals.Branch = true; 
			signals.ALUOp = 0b01; 
            signals.MemtoReg = true;
			signals.PCSrc = 1; // PC + immediate (if branch taken)
			break; // BNE
        case JALR: 
			signals.RegWrite = true; 
			signals.ALUSrc = true; 
			signals.JALRSel = true; // Write PC+4 to register
			signals.PCSrc = 2; 
			break; // JALR
        default:
            break;
    }

    return signals;
}

/**
 * the ALU Controller.
 * Takes the 2-bit ALUOp, funct3, funct7 and opcode to generate the 4-bit ALU operation signal.
 */
uint8_t Controller::aluController(uint8_t opcode, uint8_t funct3, uint8_t funct7, uint8_t alu_op) {
    if (alu_op == 0b00) 
        return 0b0010; // ADD (for Load/Store/ADDI)
    if (alu_op == 0b01) 
        return 0b0110; // SUB (for BNE)
    
    // For R-type instructions (ALUOp = 10), decode funct3/funct7
    if (alu_op == 0b10) {
        if (funct3 == 0b000 && funct7 == 0b0000000) 
            return 0b0010; // ADD
        if (funct3 == 0b000 && funct7 == 0b0100000) 
            return 0b0110; // SUB
        if (funct3 == 0b101 && funct7 == 0b0100000) 
            return 0b0111; // SRA
        if (funct3 == 0b111 && funct7 == 0b0000000) 
            return 0b0000; // AND
    }

    // For I-type ALU instructions (opcode 0010011), decode funct3
    if(opcode == 0b0010011) {
        if (funct3 == 0b110) 
            return 0b0001; // ORI
        if (funct3 == 0b011) 
            return 0b0011; // SLTIU
        if (funct3 == 0b000) 
            return 0b0010; // ADDI
    }

    return 0b0010; // Default : ADD
}
