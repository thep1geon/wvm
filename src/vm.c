#include "include/vm.h"
#include <assert.h>

static Vm vm = {0}; // Single Instance of the Virtual Machine

const char* reg_to_str[REGISTER_COUNT] = {
    [IP] = "IP",
    [RE] = "RE",
    [CR] = "CR",
    [R1] = "R1",
    [R2] = "R2",
    [R3] = "R3",
    [R4] = "R4",
};

// Debug Functions
void vm_register_dump() {
    printf("Registers: \n");
    for (usize i = 0; i < REGISTER_COUNT; ++i) {
        printf("    [%s] 0x%.4X\n", reg_to_str[i], vm.reg[i]);
    }
}

void vm_mem_hex_dump(u16 addr, u16 len) {
    for (u16 i = addr; i < addr + len; ++i) {
        if ((i - addr) % 4 == 0) {
            printf("0x%.4X    ", i);
        }

        printf("%.2X", vm.memory[i]);
        if (i + 1 < addr + len) {
            printf(" ");

            if ((i - 3 - addr) % 4 == 0) {
                putchar('\n');
            }
        }

    }

    putchar('\n');
}

// Register Functions
void vm_set_reg(RegisterName reg, u16 value) {
    if (reg >= REGISTER_COUNT)
        return;

    vm.reg[reg] = value;
}

u16 vm_get_reg(RegisterName reg) {
    if (reg >= REGISTER_COUNT)
        return 0x0000;

    return vm.reg[reg];
}

u8 vm_fetch8() {
    return vm.current_chunk->at[vm.reg[IP]++];
}

u16 vm_fetch16() {
    return (vm_fetch8() << 8) | vm_fetch8();
}

u32 vm_fetch32() {
    return (vm_fetch16() << 16) | vm_fetch16();
}

// Memory Functions
u8 vm_mem_read8(u16 addr) {
    return vm.memory[addr];
} 

u16 vm_mem_read16(u16 addr) {
    return (vm_mem_read8(addr) << 8) | vm_mem_read8(addr+1);
}

u32 vm_mem_read32(u16 addr) {
    return (vm_mem_read16(addr) << 16) | vm_mem_read16(addr+2);
}

void vm_mem_write8(u16 addr, u8 byte) {
    vm.memory[addr] = byte;
}

void vm_mem_write16(u16 addr, u16 word) {
    vm_mem_write8(addr, word >> 8);
    vm_mem_write8(addr+1, word);
}

void vm_mem_write32(u32 addr, u32 word) {
    vm_mem_write16(addr, word >> 16);
    vm_mem_write16(addr+2, word);
}

void __vm_write_bytes(u16* addr, const u8* bytes, usize bytes_len) {
    for (usize i = 0; i < bytes_len; ++i) {
        vm_mem_write8((*addr) + i, bytes[i]);
    }

    *addr += bytes_len;
}

u8 vm_cycle() {
    if (vm.halted) {
        return 1;
    }

    Inst inst = vm_fetch8();

    switch (inst) {
        case HLT: {
            vm.halted = true;
            return 1;
        };

        // Move a literal value (u16) into a register
        case MOVLR: {
            RegisterName dst = vm_fetch8();
            u16 value = vm_fetch16();
            vm_set_reg(dst, value);
        } break;

        // Move the value of one register into another register
        case MOVRR: {
            RegisterName dst_reg = vm_fetch8();
            RegisterName src_reg = vm_fetch8();
            vm_set_reg(dst_reg, vm_get_reg(src_reg));
        } break;

        // Move a value from memory into a register
        case MOVMR: {
            RegisterName dst = vm_fetch8();
            u16 addr = vm_fetch16();
            vm_set_reg(dst, vm_mem_read16(addr));
        } break;

        // Move a value from a register and put it in memory at addr
        case MOVRM: {
            u16 addr = vm_fetch16();
            RegisterName src = vm_fetch8();
            vm_mem_write16(addr, vm_get_reg(src)); 
        } break;

        case ADD: {
            RegisterName dst_reg = vm_fetch8();
            RegisterName left = vm_fetch8();
            RegisterName right = vm_fetch8();
            vm_set_reg(dst_reg, vm_get_reg(left) + vm_get_reg(right));
        } break;

        case SUB: {
            RegisterName dst_reg = vm_fetch8();
            RegisterName left = vm_fetch8();
            RegisterName right = vm_fetch8();
            vm_set_reg(dst_reg, vm_get_reg(left) - vm_get_reg(right));
        } break;

        case MULT: {
            RegisterName dst_reg = vm_fetch8();
            RegisterName left = vm_fetch8();
            RegisterName right = vm_fetch8();
            vm_set_reg(dst_reg, vm_get_reg(left) * vm_get_reg(right));
        } break;

        case DIV: {
            RegisterName dst_reg = vm_fetch8();
            RegisterName left = vm_fetch8();
            RegisterName right = vm_fetch8();
            u16 left_value = vm_get_reg(left);
            u16 right_value = vm_get_reg(right);
            vm_set_reg(dst_reg, left_value / right_value);
            vm_set_reg(RE, left_value % right_value);
        } break;

        case CMP: {
            // 0 => left = right
            // 1 => left < right
            // 2 => left > right

            RegisterName left = vm_fetch8();
            RegisterName right = vm_fetch8();
            u16 left_value = vm_get_reg(left);
            u16 right_value = vm_get_reg(right);
            if (left_value == right_value) {
                vm_set_reg(CR, (u16)0);
            } else if (left_value < right_value) {
                vm_set_reg(CR, (u16)1);
            } else {
                // left > right
                vm_set_reg(CR, (u16)2);
            }

        } break;

        // Unconditional Jump
        case JMP: {
            u16 addr = vm_fetch16();
            vm_set_reg(IP, addr);
        } break;

        // Jump if equal
        case JEQ: {
            u16 addr = vm_fetch16();

            if (vm_get_reg(CR) == 0) {
                vm_set_reg(IP, addr);
            }
        } break;

        // Jump if not equal
        case JNE: {
            u16 addr = vm_fetch16();

            if (vm_get_reg(CR) != 0) {
                vm_set_reg(IP, addr);
            }
        } break;

        case JLT: {
            u16 addr = vm_fetch16();

            if (vm_get_reg(CR) == 1) {
                vm_set_reg(IP, addr);
            }
        } break;

        case JLE: {
            u16 addr = vm_fetch16();

            u8 cmp = vm_get_reg(CR);
            if (cmp == 1 || cmp == 0) {
                vm_set_reg(IP, addr);
            }
        } break;

        case JGT: {
            u16 addr = vm_fetch16();

            u8 cmp = vm_get_reg(CR);
            if (cmp == 2) {
                vm_set_reg(IP, addr);
            }
        } break;

        case JGE: {
            u16 addr = vm_fetch16();

            u8 cmp = vm_get_reg(CR);
            if (cmp == 2 || cmp == 0) {
                vm_set_reg(IP, addr);
            }
        } break;

        default: {
            log_warn("Unknown Instruction");
            vm.halted = true;
            return 1;
        };
    }

    return 0;
}

u8 vm_interpret_chunk(const Chunk* chunk) {
    vm_set_reg(IP, 0);
    vm.current_chunk = chunk;

    u8 status = 0;
    while (status == 0) {
        status = vm_cycle();
    }

    return status;

}
