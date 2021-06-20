#include "chip8.hh"

#include <fstream>
#include <iostream>

#define LOG(x) std::cerr << x << "\n"
#define UNKNOWN_INS \
  std::cerr << "Unknown Instruction: " << std::hex << OC << "\n"

void Chip8::init_or_reset() {
  DT = 0;
  ST = 0;
  SP = 0;
  PC = 0x200;
  I  = 0;
  OC = 0;

  V.fill(0);
  Key.fill(0);
  Stack.fill(0);
  Display.fill(0);
  Memory.fill(0);

  // Load font_set into Memory
  std::copy(font_set.begin(), font_set.end(), Memory.begin());

  // use current time as seed
  // std::srand(std::time(nullptr));
}

// Temp use of fstream.
void Chip8::load_rom(const char* filename) {
  std::ifstream rom(filename, std::ios::binary | std::ios::ate);

  if (rom.is_open()) {
    std::streampos ssize = rom.tellg();

    char* buffer = new char[ssize];

    rom.seekg(0, std::ios::beg);
    rom.read(buffer, ssize);
    rom.close();

    for (auto i = 0; i < ssize; ++i) {
      Memory[0x200 + i] = buffer[i];
    }

    delete[] buffer;
  }
}

void Chip8::emulate_cycle() {
  // Fetch Opcode
  OC = Memory[PC] << 8 | Memory[PC + 1];

  const auto NNN = (OC & 0x0FFF);
  const auto NN  = (OC & 0x00FF);
  const auto N   = (OC & 0x000F);

  const auto X = ((OC & 0x0F00) >> 8);
  const auto Y = ((OC & 0x00F0) >> 4);

  // Decode & Execute
  switch (OC & 0xF000) {
    case 0x0000:
      switch (NN) {
        case 0x00E0:
          // CLS: Clear the display
          LOG("CLS");
          Display.fill(0);
          PC += 2;
          break;
        case 0x00EE:
          // RET: Return from a subroutine
          LOG("RET");
          PC = Stack[--SP];
          break;
        default:
          UNKNOWN_INS;
          break;
      }
      break;
    default:
      UNKNOWN_INS;
      break;
  }
}
