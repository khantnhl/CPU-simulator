#include "CPU.h"


// ALU Source MUX
// 1 -> immediate
// 0 -> rs2
int32_t alu_source_mux(int32_t rs2_val, int32_t immediate, bool alu_src_signal) {
    if(alu_src_signal)
	{
		return immediate;
	} else{
		return rs2_val;
	}
}

// Writeback MUX
// 1 -> mem_data
// 0 -> alu_result
int32_t writeback_mux(int32_t alu_result, int32_t mem_data, bool mem_to_reg_signal) {
    if(mem_to_reg_signal)
	{
		return mem_data;
	} else {
		return alu_result;
	}
}

// PC MUX 1: Selects between PC+4 and PC+immediate (for branches)
// 1 -> pc_plus_imm
// 0 -> pc_plus_4
int32_t pc_branch_mux(int32_t pc_plus_4, int32_t pc_plus_imm, bool branch_taken) {
    if(branch_taken)
	{
		return pc_plus_imm;
	} else {
		return pc_plus_4;
	}
}

// PC MUX 2: Selects between branch target and JALR target
// 1 -> jalr_target
// 0 -> branch_target
int32_t pc_jalr_mux(int32_t branch_target, int32_t jalr_target, bool jalr_sel) {
    if(jalr_sel)
	{
		return jalr_target;
	} else {
		return branch_target;
	}
}

// CPU Constructor - initializes all registers, PC, and memory
CPU::CPU(const char instructionMemory[4096])
{
	PC = 0; //set PC to 0

	for(int i = 0; i < 32; ++i){
		regs[i] = 0;
	}

	for (int i = 0; i < 4096; i++) //copy instrMEM
	{
		iMem[i] = instructionMemory[i];
		dmemory[i] = (0);
	}
	
	// Initialize member variables
	immediate = 0;
	rs1_val = 0;
	rs2_val = 0;
	rd_idx = 0;
	rs1_idx = 0;
	rs2_idx = 0;
	funct3 = 0;
	funct7 = 0;
	opcode = 0;
	alu_result = 0;
	alu_zero_flag = false;
	mem_read_data = 0;
	wb_data = 0;
}


unsigned long CPU::readPC()
{
	return PC;
}

void CPU::updatePC()
{
    // PC update using 2-to-1 MUXes 
    
    // all possible next PC values
    unsigned long pc_plus_4 = PC + 4;
    unsigned long pc_plus_imm = PC + immediate;
    unsigned long jalr_target = (rs1_val + immediate) & ~1;
    
    // Determine if branch should be taken
    // Branch is taken if: Branch instruction AND condition is true (not zero)
    bool branch_taken = control.Branch && !alu.is_zero();
    
    // MUX 1: Choose between PC+4 and branch target
    unsigned long mux1_out = pc_branch_mux(pc_plus_4, pc_plus_imm, branch_taken);
    
    // MUX 2: Choose between MUX 1 output and JALR target
    // Use PCSrc==2 to indicate JALR (could also use JALRSel signal)
    bool is_jalr = (control.PCSrc == 2);
    unsigned long next_pc = pc_jalr_mux(mux1_out, jalr_target, is_jalr);
    
    // Update PC with selected value
    PC = next_pc;
}

// Getters
int32_t CPU::getA0()
{
	return regs[10];
}

int32_t CPU::getA1()
{
    return regs[11];
}

// STAGE 1: INSTRUCTION FETCH
// Builds 32-bit instruction from instruction memory in little endian format
uint32_t CPU::fetch(){
	// cout<< "Fetching..";

	// make sure to have enough bytes to read from instruction memory
	if(PC + 3 < 4096){
		uint32_t instruction = 0;

		// Assemble 4 bytes into 32-bit instruction (little endian)
		instruction |= (uint32_t)(uint8_t)iMem[PC]; //least
		instruction |= (uint32_t)(uint8_t)iMem[PC+1] << 8;
		instruction |= (uint32_t)(uint8_t)iMem[PC+2] << 16;
		instruction |= (uint32_t)(uint8_t)iMem[PC+3] << 24; // most
	
	    // cout << "size check :" << sizeof(instruction) <<endl; // 4bytes 
		// cout << "Instruction (hex): 0x" << hex << instruction << dec << endl;
		
		return instruction;
	} else {
		return 0;
	} 

}

// STAGE 2: INSTRUCTION DECODE 
// Extract instruction fields and generate control signals
void CPU::decode(uint32_t instruction)
{
	opcode = instruction & 0b01111111; // 7 bits
	rd_idx = (instruction >> 7) & 0b00011111; // 5 bits
    funct3 = (instruction >> 12) & 0b00000111; // 3 bits
    rs1_idx = (instruction >> 15) & 0b00011111; //5 bits
    rs2_idx = (instruction >> 20) & 0b00011111; // 5 bits
    funct7 = (instruction >> 25) & 0b01111111; // 7 bits

	// read the registers 
    rs1_val = regs[rs1_idx];
    rs2_val = regs[rs2_idx];
    
	// tells Controller to generate signals for current opcode.
	control = controller.generate_signals(opcode);

	// Generate immediate value using ImmediateGenerator
	immediate = immGen.generate(instruction, opcode);
}


// STAGE 3: EXECUTE
void CPU::execute() {

    // either rs2 or immediate based on ALUSrc control
    int32_t alu_op2 = alu_source_mux(rs2_val, immediate, control.ALUSrc);
    
    // CPU asks Controller for the specific ALU operation.
    // returns 4 bits signal
    uint8_t alu_op = controller.aluController(opcode, funct3, funct7, control.ALUOp);

    // CPU tells its ALU to execute the operation.
    alu_result = alu.execute(alu_op, rs1_val, alu_op2);
    
    // LUI: Use immediate instead of ALU result (controlled by LUISel signal)
    if (control.LUISel) {
        alu_result = immediate;
    }
}

//  STAGE 4: MEMORY ACCESS 
void CPU::mem() {
    if (!control.MemRead && !control.MemWrite) 
		return;
    
    // Mask address to fit within memory
    uint32_t addr = (uint32_t)alu_result & 0xFFF;

    if (control.MemWrite) {
        switch (funct3) {
            case 0b001: // SH half word
                if (addr + 1 < 4096) { 
					dmemory[addr] = rs2_val & 0xFF; 
					dmemory[addr + 1] = (rs2_val >> 8) & 0xFF; 
				} 
				break;
            case 0b010: // SW store word 
                if (addr + 3 < 4096) { 
					dmemory[addr] = rs2_val & 0xFF; 
					dmemory[addr + 1] = (rs2_val >> 8) & 0xFF; 
					dmemory[addr + 2] = (rs2_val >> 16) & 0xFF; 
					dmemory[addr + 3] = (rs2_val >> 24) & 0xFF; 
				} 
				break;
        }
    } else if (control.MemRead) {
        switch (funct3) {
            case 0b000: // LB load byte 
				mem_read_data = (int8_t)dmemory[addr];
				break;
            case 0b100: // LBU - load byte unsigned
				mem_read_data = (uint8_t)dmemory[addr]; 
				break;
            case 0b010: // LW - load word (little endian)
                if (addr + 3 < 4096) { 
					mem_read_data = (uint32_t)(uint8_t)dmemory[addr] | 
									((uint32_t)(uint8_t)dmemory[addr+1] << 8) | 
									((uint32_t)(uint8_t)dmemory[addr+2] << 16) | 
									((uint32_t)(uint8_t)dmemory[addr+3] << 24); 
				} 
				break;
        }
    }
}

// STAGE 5: WRITE BACK 
void CPU::wb() {
    if (!control.RegWrite || rd_idx == 0) {
		wb_data = 0; // No data written
		return;
	}

    wb_data = writeback_mux(alu_result, mem_read_data, control.MemtoReg);
    
    // JALR: Write PC+4 to register (controlled by JALRSel signal)
    if (control.JALRSel) {
        wb_data = PC + 4;
    }
    
    regs[rd_idx] = wb_data;
}


// DEBUGGING Helper FUNCTIONS
std::string CPU::disassemble_instruction() {
    std::stringstream ss;
    switch(opcode) {
        case 0b0110011: // R-type
            if (funct3 == 0b000 && funct7 == 0b0000000) ss << "add ";
            else if (funct3 == 0b000 && funct7 == 0b0100000) ss << "sub ";
            else if (funct3 == 0b101 && funct7 == 0b0100000) ss << "sra ";
            else if (funct3 == 0b111 && funct7 == 0b0000000) ss << "and ";
            ss << "x" << rd_idx << ", x" << rs1_idx << ", x" << rs2_idx;
            break;
        case 0b0010011: // I-type
            if (funct3 == 0b000) ss << "addi ";
            else if (funct3 == 0b110) ss << "ori ";
            else if (funct3 == 0b011) ss << "sltiu ";
            ss << "x" << rd_idx << ", x" << rs1_idx << ", " << immediate;
            break;
        case 0b0110111: ss << "lui x" << rd_idx << ", 0x" << std::hex << (immediate >> 12); break;
        case 0b0000011: // Load
            if (funct3 == 0b010) ss << "lw ";
            else if (funct3 == 0b100) ss << "lbu ";
            ss << "x" << rd_idx << ", " << immediate << "(x" << rs1_idx << ")";
            break;
        case 0b0100011: // Store
            if (funct3 == 0b010) ss << "sw ";
            else if (funct3 == 0b001) ss << "sh ";
            ss << "x" << rs2_idx << ", " << immediate << "(x" << rs1_idx << ")";
            break;
        case 0b1100011: ss << "bne x" << rs1_idx << ", x" << rs2_idx << ", " << immediate; break;
        case 0b1100111: ss << "jalr x" << rd_idx << ", x" << rs1_idx << ", " << immediate; break;
        default: ss << "unknown"; break;
    }
    return ss.str();
}

void CPU::print_debug_state(uint32_t instruction, int cycle) {
    std::cout << "================== CYCLE " << std::dec << cycle << " ==================" << std::endl;
    std::cout << "PC: 0x" << std::hex << PC << std::dec << std::endl;
    std::cout << "Instruction: 0x" << std::hex << std::setfill('0') << std::setw(8) << instruction 
              << "  [" << disassemble_instruction() << "]" << std::dec << std::endl;

    std::cout << "--- DECODE ---" << std::endl;
    std::cout << "rs1: x" << (int)rs1_idx << " = " << rs1_val 
              << "   rs2: x" << (int)rs2_idx << " = " << rs2_val << std::endl;
    std::cout << "Immediate: " << immediate << std::endl;

    std::cout << "--- EXECUTE ---" << std::endl;
    std::cout << "ALU Result: " << alu_result << " (Zero Flag: " << (alu.is_zero() ? "T" : "F") << ")" << std::endl;
    
    if (control.MemRead) {
        std::cout << "--- MEMORY ---" << std::endl;
        std::cout << "Reading from address " << alu_result << ", Value: " << mem_read_data << std::endl;
    }
    if (control.MemWrite) {
        std::cout << "--- MEMORY ---" << std::endl;
        std::cout << "Writing " << rs2_val << " to address " << alu_result << std::endl;
    }

    if (control.RegWrite && rd_idx != 0) {
        std::cout << "--- WRITE BACK ---" << std::endl;
        std::cout << "Writing " << wb_data << " to register x" << (int)rd_idx << std::endl;
    }
    std::cout << std::endl;
}
