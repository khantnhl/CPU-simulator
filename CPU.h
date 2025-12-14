#include <iostream>
#include <bitset>
#include <stdio.h>
#include<stdlib.h>
#include <string>
#include <sstream>
#include <iomanip>

#include "Controller.h"
#include "ALU.h"
#include "ImmediateGenerator.h"

using namespace std;


class CPU {
private:
	// store actual values
	// Data Memory
	uint8_t dmemory[4096]; //data memory byte addressable in little endian fashion;
	
	// Instruction Memory
	uint8_t iMem[4096];
	unsigned long PC; //pc 
	int32_t regs[32]; // registers

	//Components
	// Controller
	Controller controller;
	// ALU
	ALU alu;
	// Immediate Generator
	ImmediateGenerator immGen;
	// Control Signals
	ControlSignals control;

	// Data
	int32_t immediate;
    int32_t rs1_val, rs2_val;

	// Instruction Fields
    uint8_t rd_idx, rs1_idx, rs2_idx;
    uint8_t funct3, funct7;
    uint8_t opcode;

	// ALU Result
    int32_t alu_result;
    bool alu_zero_flag;

	// Memory Read Data	(MEM)
    int32_t mem_read_data;
	// Write Back Data	(WB)
	int32_t wb_data; 
	
	// Debugger
    std::string disassemble_instruction();

public:
	// Constructor
	CPU(const char instructionMemory[4096]);
	// Getters 
	unsigned long readPC();
	// Update PC
	void updatePC();
	// Getters for A0 and A1
	int32_t getA0();
	int32_t getA1();
	
	// Fetch
	uint32_t fetch();
	// Decode
	void decode(uint32_t instruction);
	// Execute
    void execute();
	// Memory
    void mem();
	// Write Back
    void wb();
	// Debugging	
	void print_debug_state(uint32_t instruction, int cycle);
	
};

// add other functions and objects here
