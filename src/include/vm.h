#ifndef __VM_H
#define __VM_H 1

#include "chunk.h"

#include <assert.h>
#include <pigeon/alias.h>
#include <pigeon/log.h>

// Register Utilities
typedef enum {
    IP, // Instruction Pointer
    RE, // Remainder Register. For working with floating point numbers
    CR, // Comparison Register
    R1, // General Register 1
    R2, // General Register 2
    R3, // General Register 3
    R4, // General Register 4
    REGISTER_COUNT, // A count of all the registers
} RegisterName;

// Virtual Machine Specifics
#define VM_MEM_LENGTH (256 * 256)

typedef struct vm_t {
    u16 reg[REGISTER_COUNT]; // The registers for the Virtual Machine
    u8 memory[VM_MEM_LENGTH];
    bool halted;

    const Chunk* current_chunk; // Current Chunk being interpreted
} Vm;

// Useful Enumeration of the instructions
// Saves us from having to remember the hex values
typedef enum {
    HLT = 0x00, // Halt the machine and stop execution

    // Moving data around
    MOVLR = 0x10, // Move u16 literal into register
    MOVRR = 0x11, // Move value from one register into another
    MOVMR = 0x12, // Move the value from addr into register
    MOVRM = 0x13, // Move the value from a register to an addr in memory
    
    // Maths
    ADD = 0x20, // Add two registers and place the value into a third register
    SUB = 0x21, // Subtract two registers and place the value into a third register
    MULT = 0x22, // Mulitple two registers' values and put the product into a third register
    DIV = 0x23, // Divide two registers' values and put the quotient into a third register and the remainder in the RE register

    // Comparison
    CMP = 0x30, // Compare the values of two registers and place the result in CR

    // Jumps
    JMP = 0x40, // Unconditional jump
    JEQ = 0x41, // Jump if CR is 0 (jump if equal)
    JNE = 0x42, // Jump if CR is not 0 (jump if not equal)
    JLT = 0x43, // Jump if CR is 1 (jump if less than)
    JLE = 0x44, // Jump if CR is 1 or 0 (jump if less than or equal)
    JGT = 0x45, // Jump if CR is 2 (jump if greater than)
    JGE = 0x46, // Jump if CR is 2 or 0 (jump if greater than or equal)
} Inst;

// Debug Functions
void vm_register_dump();
void vm_mem_hex_dump(u16 addr, u16 len);

// Register Functions
void vm_set_reg(RegisterName reg, u16 value);
u16  vm_get_reg(RegisterName reg);

// Memory Functions
u8   vm_mem_read8(u16 addr);
u16  vm_mem_read16(u16 addr);
u32  vm_mem_read32(u16 addr);

void vm_mem_write8(u16 addr, u8 byte);
void vm_mem_write16(u16 addr, u16 word);
void vm_mem_write32(u32 addr, u32 word);

void __vm_write_bytes(u16* addr, const u8* bytes, usize bytes_len);
#define vm_write_bytes(addr, bytes_arr) \
    __vm_write_bytes((addr), (bytes_arr), LEN((bytes_arr)))

// Special Memory Functions 
//
// These functions read memory at the IP from the current chunk being
// interpreted as intructions
u8  vm_fetch8();
u16 vm_fetch16();
u32 vm_fetch32();

// Controlling the virtual machine
// I'm really not too sure which category this would go in
u8 vm_cycle();
u8 vm_interpret_chunk(const Chunk* chunk);

#endif
