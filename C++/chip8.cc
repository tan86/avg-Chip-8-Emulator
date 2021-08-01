#include "chip8.hh"

#include <ctime>
#include <fstream>
#include <iostream>

#ifdef Debug
#define LOG(x)      std::cerr << x << "\n"
#define UNKNOWN_INS std::cerr << "Unknown Instruction: " << std::hex << OC << "\n"
#else
#define LOG(x)
#define UNKNOWN_INS
#endif

void Chip8::init_or_reset() {
    constexpr std::array<uint8_t, 80> font_set{
        0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
        0x20, 0x60, 0x20, 0x20, 0x70,  // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
        0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
        0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
        0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
        0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
        0xF0, 0x80, 0xF0, 0x80, 0x80,  // F
    };

    DT = 0;
    I  = 0;
    OC = 0;
    PC = 0x200;
    SP = 0;
    ST = 0;

    V.fill(0);
    Key.fill(0);
    Stack.fill(0);
    Display.fill(0);
    Memory.fill(0);

    // Load font_set into Memory
    std::copy(font_set.begin(), font_set.end(), Memory.begin());

    // use current time as seed
    std::srand(std::time(nullptr));
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

        for (auto i = 0; i != ssize; ++i) {
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
    const auto X   = ((OC & 0x0F00) >> 8);
    const auto Y   = ((OC & 0x00F0) >> 4);

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
                    PC = Stack[SP--];
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
        case 0x2000:
            // CALL addr: Call subroutine at nnn
            LOG("CALL " << NNN);
            Stack[++SP] = PC + 2;
            PC          = NNN;
            break;
        case 0x3000:
            // SE Vx, byte: Skip next instruction if Vx = nn
            LOG("SE V" << X << ", " << NN);
            PC += (V[X] == NN) ? 4 : 2;
            break;
        case 0x4000:
            // SNE Vx, byte: Skip next instruction if Vx != nn
            LOG("SNE V" << X << ", " << NN);
            PC += (V[X] != NN) ? 4 : 2;
            break;
        case 0x5000:
            // SE Vx, Vy: Skip next instruction if Vx = Vy
            LOG("SE V" << X << ", V" << Y);
            PC += (V[X] == V[Y]) ? 4 : 2;
            break;
        case 0x6000:
            // LD Vx, byte: Set Vx = nn
            LOG("LD V" << X << ", " << NN);
            V[X] = NN;
            PC += 2;
            break;
        case 0x7000:
            // ADD Vx, byte: Set Vx = Vx + nn
            LOG("ADD V" << X << ", " << NN);
            V[X] += NN;
            PC += 2;
            break;
        case 0x8000:
            switch (N) {
                case 0x0:
                    // LD Vx, Vy: Set Vx = Vy
                    LOG("LD V" << X << ", V" << Y);
                    V[X] = V[Y];
                    break;
                case 0x1:
                    // OR Vx, Vy: Set Vx = Vx OR Vy
                    LOG("OR V" << X << ", V" << Y);
                    V[X] |= V[Y];
                    break;
                case 0x2:
                    // AND Vx, Vy: Set Vx = Vx AND Vy
                    LOG("AND V" << X << ", V" << Y);
                    V[X] &= V[Y];
                    break;
                case 0x3:
                    // XOR Vx, Vy: Set Vx = Vx XOR Vy
                    LOG("XOR V" << X << ", V" << Y);
                    V[X] ^= V[Y];
                    break;
                case 0x4:
                    // ADD Vx, Vy: Set Vx = Vx + Vy, VF = carry
                    LOG("ADD V" << X << ", V" << Y << " With Carry");
                    V[0xF] = (V[X] + V[Y]) > 255 ? 1 : 0;
                    V[X] += V[Y];
                    break;
                case 0x5:
                    // SUB Vx, Vy: Set Vx = Vx - Vy, VF = NOT Borrow
                    LOG("SUB V" << X << ", V" << Y << " With Borrow");
                    V[0xF] = V[X] > V[Y];
                    V[X] -= V[Y];
                    break;
                case 0x6:
                    // SHR Vx {, Vy}: Set Vx = SHR 1
                    LOG("SHR V" << X << ", V" << Y);
                    V[0xF] = V[X] & 0x1;
                    V[X] >>= 1;
                    break;
                case 0x7:
                    // SUBN Vx, Vy: Set Vx = Vy - Vx, Set VF = NOT Borrow
                    LOG("SUBN V" << X << ", V" << Y);
                    V[0xF] = V[Y] > V[X];
                    V[X]   = V[Y] - V[X];
                    break;
                case 0xE:
                    // SHL Vx {, Vy}: Set Vx = Vx SHL 1
                    LOG("SHL V" << X << ", V" << Y);
                    V[0xF] = V[X] >> 7;
                    V[X] <<= 1;
                    break;
                default:
                    UNKNOWN_INS;
                    break;
            }
            PC += 2;
            break;
        case 0x9000:
            // SNE Vx, Vy: Skip next instruction if Vx != Vy
            LOG("SNE V" << X << ", V" << Y);
            PC += (V[X] != V[Y]) ? 4 : 2;
            break;
        case 0xA000:
            // LD I, addr: Set I = nnn
            LOG("LD I, " << NNN);
            I = NNN;
            PC += 2;
            break;
        case 0xB000:
            // JP V0, addr: Jump to location nnn + V0
            LOG("JP V0 + " << NNN);
            PC = NNN + V[0x0];
            break;
        case 0xC000:
            // RND Vx, byte: Set Vx = random byte AND nn
            LOG("RND V" << X << ", " << NN);
            V[X] = (rand() % 256) & NN;
            PC += 2;
            break;
        case 0xD000:
            // DRW x, y, nibble: Display n-byte sprite starting at memory
            // location I at(Vx, Vy), Set VF = collision
            LOG("DRW " << X << ", " << Y << ", " << N);
            {
                uint8_t xPos = V[X] % 64;
                uint8_t yPos = V[Y] % 32;

                V[0xF] = 0;
                for (uint8_t row = 0; row != N; ++row) {
                    uint8_t spriteByte = Memory[I + row];

                    for (uint8_t col = 0; col != 8; ++col) {
                        uint8_t spritePixel = (spriteByte >> (7 - col)) & 0x1;

                        uint8_t* pixel = &Display[((yPos + row) * 64) + (xPos + col)];

                        if (spritePixel == 1 && *pixel == 1) {
                            V[0xF] = 1;
                        }

                        *pixel ^= spritePixel;
                    }
                }
            }
            PC += 2;
            break;
        case 0xE000:
            switch (NN) {
                case 0x9E:
                    // SKP Vx: Skip next instruction if key with the value of Vx
                    // is pressed
                    LOG("SKP V" << X);
                    PC += (Key[V[X]]) ? 4 : 2;
                    break;
                case 0xA1:
                    // SKNP Vx: Skip next instruction if key with the value of
                    // Vx is NOT pressed
                    LOG("SKNP V" << X);
                    PC += (!Key[V[X]]) ? 4 : 2;
                    break;
                default:
                    UNKNOWN_INS;
                    break;
            }
            break;
        case 0xF000:
            switch (NN) {
                case 0x07:
                    // LD Vx, DT: Set Vx = Delay Timer value
                    LOG("LD V" << X << ", DT: " << DT);
                    V[X] = DT;
                    break;
                case 0x0A:
                    // LD Vx, K: Wait for a key press, store value of the key in
                    // Vx
                    LOG("LD V" << X << " K: Wait for a key press");
                    for (uint8_t i = 0; i != 16; ++i) {
                        if (Key[i] == 1) {
                            V[X] = i;
                        }
                    }
                    PC -= 2;
                    break;
                case 0x15:
                    // LD DT, Vx: Set Delay Timer = Vx
                    LOG("LD DT: " << DT << ", V" << X);
                    DT = V[X];
                    break;
                case 0x18:
                    // LD ST, Vx: Set Sound Timer = Vx
                    LOG("LD ST: " << ST << ", V" << X);
                    ST = V[X];
                    break;
                case 0x1E:
                    // ADD I, Vx: Set I = I + Vx
                    LOG("ADD I, V" << X);
                    I += V[X];
                    break;
                case 0x29:
                    // LD F, Vx: Set I = location of sprite for digit Vx
                    LOG("LD F, V" << X);
                    I = 5 * V[X];
                    break;
                case 0x33:
                    // LD B, Vx: Store BCD representation of Vx in memory
                    // locations I, I + 1, and I + 2
                    LOG("LD B, V" << X << "Store BCD");
                    Memory[I]     = (V[X] % 1000) / 100;
                    Memory[I + 1] = (V[X] % 100) / 10;
                    Memory[I + 2] = V[X] % 10;
                    break;
                case 0x55:
                    // LD {I}, Vx: Store registers V0 through Vx in memory
                    // starting at location I
                    LOG("LD {I}, "
                        << "Store Regs V0 through V" << X);
                    for (uint8_t i = 0; i <= X; ++i) Memory[I + i] = V[i];
                    break;
                case 0x65:
                    // LD Vx, {I}: Read registers V0 through Vx from memory
                    // starting at location I
                    LOG("LD Read Regs V0 through V" << X << ", {I}");
                    for (uint8_t i = 0; i <= X; ++i) V[i] = Memory[I + i];
                    break;
                default:
                    UNKNOWN_INS;
                    break;
            }
            PC += 2;
            break;
        default:
            UNKNOWN_INS;
            break;
    }
    // Tick
    if (DT > 0) {
        --DT;
    }
    if (ST > 0) {
        --ST;
    }
}
