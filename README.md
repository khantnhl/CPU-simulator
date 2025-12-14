# RISC-V CPU Simulator

A cycle-accurate RISC-V CPU simulator implemented in C++ that executes RISC-V assembly instructions through a 5-stage pipeline architecture. This project demonstrates the fundamental concepts of computer architecture, including instruction fetching, decoding, execution, memory operations, and register write-back.

## 🚀 Features

- **5-Stage Pipeline Architecture**: Implements Fetch, Decode, Execute, Memory, and Write-Back stages
- **RISC-V Instruction Support**: Supports multiple instruction types including:
  - R-type instructions (arithmetic/logical operations)
  - I-type instructions (immediate operations)
  - Load/Store instructions (LW, SW)
  - LUI (Load Upper Immediate)
  - Branch instructions (BNE)
  - Jump instructions (JALR)
- **Component-Based Design**: Modular architecture with separate ALU, Controller, and Immediate Generator components
- **Byte-Addressable Memory**: 4KB instruction and data memory with little-endian byte ordering
- **32 General-Purpose Registers**: Full RISC-V register file implementation

## 📋 Table of Contents

- [Architecture](#architecture)
- [Project Structure](#project-structure)
- [Building the Project](#building-the-project)
- [Usage](#usage)
- [Input Format](#input-format)
- [Output Format](#output-format)
- [Supported Instructions](#supported-instructions)
- [Example](#example)

## 🏗️ Architecture

The simulator implements a classic 5-stage RISC-V pipeline:

1. **Fetch (IF)**: Retrieves instructions from instruction memory based on the Program Counter (PC)
2. **Decode (ID)**: Extracts instruction fields (opcode, registers, immediate values) and generates control signals
3. **Execute (EX)**: Performs arithmetic/logical operations using the ALU
4. **Memory (MEM)**: Handles load and store operations to/from data memory
5. **Write-Back (WB)**: Writes results back to the register file

### Key Components

- **CPU**: Main processor class orchestrating the pipeline stages
- **ALU**: Arithmetic Logic Unit performing operations based on control signals
- **Controller**: Generates control signals for instruction execution
- **ImmediateGenerator**: Extracts and sign-extends immediate values from various instruction formats

## 📁 Project Structure

```
.
├── cpusim.cpp              # Main entry point
├── CPU.h                   # CPU class header
├── CPU.cpp                 # CPU implementation
├── ALU.h                   # ALU class header
├── ALU.cpp                 # ALU implementation
├── Controller.h            # Controller class header
├── Controller.cpp          # Controller implementation
├── ImmediateGenerator.h    # Immediate generator header
├── ImmediateGenerator.cpp  # Immediate generator implementation
├── *.txt                   # Test instruction memory files
└── README.md               # This file
```

## 🔨 Building the Project

### Prerequisites

- C++ compiler with C++11 support (g++, clang++, etc.)
- Make (optional, for build automation)

### Compilation

Compile the project using your preferred C++ compiler:

```bash
g++ -std=c++11 -o cpusim cpusim.cpp CPU.cpp ALU.cpp Controller.cpp ImmediateGenerator.cpp
```

Or using clang:

```bash
clang++ -std=c++11 -o cpusim cpusim.cpp CPU.cpp ALU.cpp Controller.cpp ImmediateGenerator.cpp
```

## 💻 Usage

Run the simulator with an instruction memory file:

```bash
./cpusim <instruction_memory_file>
```

The simulator will execute all instructions in the file and output the final values of registers `a0` and `a1`.

### Example

```bash
./cpusim 25test.txt
```

Output:
```
(42, 24)
```

## 📥 Input Format

The instruction memory file should contain hexadecimal values, one per line, representing 32-bit RISC-V instructions. Each instruction is stored as 4 bytes in little-endian format.

Example input file format:
```
00010437
04040413
000204b7
fe048493
...
```

## 📤 Output Format

The simulator outputs the final values of registers `a0` (x10) and `a1` (x11) in the format:

```
(a0_value, a1_value)
```

## 🎯 Supported Instructions

### R-Type Instructions (opcode: 0x33)
- `ADD`, `SUB`, `AND`, `OR`, `XOR`, `SLL`, `SRL`, `SRA`, `SLT`, `SLTU`

### I-Type Instructions (opcode: 0x13)
- `ADDI`, `ANDI`, `ORI`, `XORI`, `SLLI`, `SRLI`, `SRAI`, `SLTI`, `SLTIU`

### Load Instructions (opcode: 0x03)
- `LW` (Load Word)
- `LB`, `LBU`, `LH`, `LHU` (Load Byte variants)

### Store Instructions (opcode: 0x23)
- `SW` (Store Word)
- `SB`, `SH` (Store Byte/Halfword)

### U-Type Instructions
- `LUI` (Load Upper Immediate, opcode: 0x37)

### Branch Instructions (opcode: 0x63)
- `BNE` (Branch if Not Equal)

### Jump Instructions (opcode: 0x67)
- `JALR` (Jump and Link Register)

## 📝 Example

Given an instruction file containing:

```
00010437        # lui x8, 0x10
04040413        # addi x8, x8, 64
000204b7        # lui x9, 0x20
fe048493        # addi x9, x9, -32
```

The simulator will execute these instructions sequentially, updating registers and memory as specified by the RISC-V instruction set architecture.

## 🔍 Debug Mode

The simulator includes a debug mode that can be enabled by setting the `debug` flag to `true` in `cpusim.cpp`. When enabled, it prints detailed information about each instruction execution cycle, including:
- Current cycle number
- Instruction being executed
- Register values
- Memory operations
- Control signals

## 📚 Technical Details

- **Memory Size**: 4KB for both instruction and data memory
- **Register File**: 32 registers (x0-x31), where x0 is hardwired to zero
- **Endianness**: Little-endian byte ordering
- **Pipeline**: Non-pipelined execution (sequential stage execution per cycle)
- **Word Size**: 32 bits

## 🛠️ Development

This project was developed as part of a computer architecture course to demonstrate understanding of:
- CPU pipeline design
- Instruction set architecture (ISA) implementation
- Control signal generation
- Memory hierarchy concepts
- RISC-V instruction encoding

## 📄 License

This project is provided for educational purposes. 

---

**Note**: This simulator implements a subset of the RISC-V instruction set architecture and is intended for educational use. For production RISC-V implementations, refer to the official RISC-V specifications.
