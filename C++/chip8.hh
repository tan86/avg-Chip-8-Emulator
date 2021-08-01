#ifndef _C8E_CHIP8_HH_
#define _C8E_CHIP8_HH_

#include <array>
#include <cstdint>

struct Chip8 {
    uint8_t DT{};  // Delay Timer
    uint8_t SP{};  // Stack Pointer
    uint8_t ST{};  // Sound Timer

    uint16_t I{};
    uint16_t OC{};  // Opcode
    uint16_t PC{};  // Program Counter

    std::array<uint8_t, 16>   Key{};
    std::array<uint8_t, 16>   V{};  // General Purpose Registers
    std::array<uint8_t, 2048> Display{};
    std::array<uint8_t, 4096> Memory{};

    std::array<uint16_t, 16> Stack{};

    void init_or_reset();
    void load_rom(const char* filename);
    void emulate_cycle();
};
#endif
