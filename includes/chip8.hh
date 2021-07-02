#ifndef _C8E_CHIP8_HH_
#define _C8E_CHIP8_HH_

#include <array>
#include <cstdint>

class Chip8 {
   public:
    // TODO(tan): Create functions for Display and Key array. don't keep them
    // public
    bool                      drawFlag = false;
    std::array<uint8_t, 2048> Display{};

    void init_or_reset();
    void load_rom(const char* filename);
    void emulate_cycle();

    void setKey(const uint8_t index, const uint8_t value);

   private:
    uint8_t DT{};  // Delay Timer
    uint8_t ST{};  // Sound Timer
    uint8_t SP{};  // Stack Pointer

    uint16_t PC{};  // Program Counter
    uint16_t I{};
    uint16_t OC{};  // Opcode

    std::array<uint8_t, 16>   V{};  // General Purpose Registers
    std::array<uint8_t, 16>   Key{};
    std::array<uint8_t, 4096> Memory{};

    std::array<uint16_t, 16> Stack{};
};
#endif
