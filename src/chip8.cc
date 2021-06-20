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
          drawFlag = true;
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
    case 0x1000:
      // JP addr: Jump to location nnn
      LOG("JP " << NNN);
      PC = NNN;
      break;
    case 0x6000:
      // LD Vx, byte: Set Vx = nn
      LOG("LD V" << X << ", " << NN);
      V[X] = NN;
      PC += 2;
      break;
    case 0xA000:
      // LD I, addr: Set I = nnn
      LOG("LD I, " << NNN);
      I = NNN;
      PC += 2;
      break;
    case 0xD000:
      /* DRW x, y, nibble: Display n-byte sprite starting at memory location I
         at(Vx, Vy), Set VF = collision*/
      LOG("DRW " << X << ", " << Y << ", " << N);
      {
        uint8_t xPos = V[X] % 64;
        uint8_t yPos = V[Y] % 32;

        // Collision Flag = 0
        V[0xF] = 0;
        for (uint8_t row = 0; row < N; ++row) {
          uint8_t spriteByte = Memory[I + row];

          for (uint8_t col = 0; col < 8; ++col) {
            uint8_t spritePixel = (spriteByte >> col) & 0x1;

            uint8_t* pixel = &Display[((yPos + row) * 64) + (xPos + col)];

            if (spritePixel == 1 && *pixel == 1) {
              V[0xF] = 1;
            }

            *pixel ^= spritePixel;
            if (*pixel == 1) {
              *pixel = 0xFF;
            } else {
              *pixel = 0x00;
            }
          }
        }
      }
      drawFlag = true;
      PC += 2;
      break;
    default:
      UNKNOWN_INS;
      break;
  }
  // LOG("PC: " << PC);
}
