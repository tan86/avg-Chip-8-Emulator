#ifndef _CHIP8_H_
#define _CHIP8_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef Debug
#define LOG(x)      printf("OC: %02X ==> %s\n", c8.OC, x)
#define UNKNOWN_INS printf("Unknown Instruction: %02X\n", c8.OC)
#else
#define LOG(x)
#define UNKNOWN_INS
#endif

typedef struct Chip8 {
	uint16_t I;
	uint16_t OC;
	uint16_t PC;
	uint16_t Stack[16];

	uint8_t DT;
	uint8_t Display[2048];
	uint8_t Key[16];
	uint8_t Memory[4096];
	uint8_t SP;
	uint8_t ST;
	uint8_t V[16];
} Chip8;

const uint8_t font_set[80] = {
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

static struct Chip8 c8;

struct Chip8* init_or_reset() {
	c8.DT = 0;
	c8.I  = 0;
	c8.OC = 0;
	c8.PC = 0x200;
	c8.SP = 0;
	c8.ST = 0;

	memset(c8.Display, 0, sizeof(c8.Display));
	memset(c8.Key, 0, sizeof(c8.Key));
	memset(c8.Memory, 0, sizeof(c8.Memory));
	memset(c8.Stack, 0, sizeof(c8.Stack));
	memset(c8.V, 0, sizeof(c8.V));

	memcpy(c8.Memory, font_set, sizeof(font_set));

	srand(time(NULL));

	return &c8;
}

void load_rom(const char* rom_file) {
	FILE* rom = fopen(rom_file, "rb");
	if (rom) {
		fseek(rom, 0L, SEEK_END);
		size_t size = ftell(rom);
		fseek(rom, 0L, SEEK_SET);

		for (size_t i = 0; i != size; ++i) {
			c8.Memory[c8.PC + i] = fgetc(rom);
		}
		fclose(rom);
	}
}

void emulate_cycle() {
	/* Fetch Opcode */
	c8.OC = c8.Memory[c8.PC] << 8 | c8.Memory[c8.PC + 1];

	const uint16_t NNN = (c8.OC & 0x0FFF);
	const uint8_t  NN  = (c8.OC & 0x00FF);
	const uint8_t  N   = (c8.OC & 0x000F);
	const uint8_t  X   = ((c8.OC & 0x0F00) >> 8);
	const uint8_t  Y   = ((c8.OC & 0x00F0) >> 4);

	/* Decode & Execute */
	switch ((c8.OC & 0xF000) >> 12) {
		case 0x0:
			switch (NN) {
				case 0xE0: /* CLS: Clear the display */
					LOG("CLS");
					memset(c8.Display, 0, sizeof(c8.Display));
					c8.PC += 2;
					break;
				case 0xEE: /* RET: Return from a subroutine */
					LOG("RET");
					c8.PC = c8.Stack[c8.SP--];
					break;
				default:
					UNKNOWN_INS;
					break;
			}
			break;
		case 0x1: /* JP addr: Jump to location NNN */
			LOG("JP addr");
			c8.PC = NNN;
			break;
		case 0x2: /* CALL addr: Call subroutine at NNN */
			LOG("CALL addr");
			c8.Stack[++c8.SP] = c8.PC + 2;
			c8.PC             = NNN;
			break;
		case 0x3: /* SE Vx, byte: Skip next instruction if Vx = NN */
			LOG("SE Vx, byte");
			c8.PC += (c8.V[X] == NN) ? 4 : 2;
			break;
		case 0x4: /* SNE Vx, byte: skip next instruction if Vx != NN */
			LOG("SNE Vx, byte");
			c8.PC += (c8.V[X] != NN) ? 4 : 2;
			break;
		case 0x5: /* SE Vx, Vy: Skip next instruction if Vx = Vy */
			LOG("SE Vx, Vy");
			c8.PC += (c8.V[X] == c8.V[Y]) ? 4 : 2;
			break;
		case 0x6: /* LD Vx, byte: Set Vx = NN */
			LOG("LD Vx, byte");
			c8.V[X] = NN;
			c8.PC += 2;
			break;
		case 0x7: /* ADD Vx, byte: Set Vx = Vx + NN */
			LOG("ADD Vx, byte");
			c8.V[X] += NN;
			c8.PC += 2;
			break;
		case 0x8:
			switch (N) {
				case 0x0: /* LD Vx, Vy: Set Vx = Vy */
					LOG("LD Vx, Vy");
					c8.V[X] = c8.V[Y];
					break;
				case 0x1: /* OR Vx, Vy: Set Vx = Vx OR Vy */
					LOG("OR Vx, Vy");
					c8.V[X] |= c8.V[Y];
					break;
				case 0x2: /* AND Vx, Vy: Set Vx = Vx AND Vy */
					c8.V[X] &= c8.V[Y];
					break;
				case 0x3: /* XOR Vx, Vy: Set Vx = Vx XOR Vy */
					LOG("XOR Vx, Vy");
					c8.V[X] ^= c8.V[Y];
					break;
				case 0x4: /* ADD Vx, Vy: Vx = Vx + Vy, VF = carry */
					LOG("ADD Vx, Vy");
					c8.V[0xF] = (c8.V[X] + c8.V[Y]) > 255 ? 1 : 0;
					c8.V[X] += c8.V[Y];
					break;
				case 0x5: /*SUB Vx, Vy: Set Vx = Vx - Vy, VF = not borrow */
					LOG("SUB Vx, Vy");
					c8.V[0xF] = c8.V[X] > c8.V[Y];
					c8.V[X] -= c8.V[Y];
					break;
				case 0x6: /*SHR Vx, Vy: Set Vx = SHR 1 */
					LOG("SHR Vx, Vy");
					c8.V[0xF] = c8.V[X] & 0x1;
					c8.V[X] >>= 1;
					break;
				case 0x7: /*SUBN Vx, Vy: Set Vx = Vy - Vx, VF = not borrow */
					LOG("SUBN Vx, Vy");
					c8.V[0xF] = c8.V[Y] > c8.V[X];
					c8.V[X]   = c8.V[Y] - c8.V[X];
					break;
				case 0xE: /*SHL Vx, Vy: Set Vx = Vx SHL 1 */
					LOG("SHL Vx, Vy");
					c8.V[0xF] = c8.V[X] >> 7;
					c8.V[X] <<= 1;
					break;
				default:
					UNKNOWN_INS;
					break;
			}
			c8.PC += 2;
			break;
		case 0x9: /* SNE Vx, Vy: Skip next instruction Vx != Vy */
			LOG("SNE Vx, Vy");
			c8.PC += (c8.V[X] != c8.V[Y]) ? 4 : 2;
			break;
		case 0xA: /* LD I, addr: Set I = NNN */
			LOG("LD I, addr");
			c8.I = NNN;
			c8.PC += 2;
			break;
		case 0xB: /* JP V0, addr: Jump to location NNN + V0 */
			LOG("JP V0, addr");
			c8.PC = NNN + c8.V[0x0];
			break;
		case 0xC: /* RND Vx, byte: Set Vx = random byte AND NN */
			LOG("RND Vx, byte");
			c8.V[X] = (rand() % 256) & NN;
			c8.PC += 2;
			break;
		case 0xD: /* DRW x, y, nibble: Display n-byte sprite starting at memory
		             location I at (Vx, Vy), Set V[0xF] = collision */
			LOG("DRW x, y, nibble");
			{
				uint8_t xPos = c8.V[X] % 64;
				uint8_t yPos = c8.V[Y] % 32;

				c8.V[0xF] = 0;
				for (uint8_t row = 0; row != N; ++row) {
					uint8_t spriteByte = c8.Memory[c8.I + row];

					for (uint8_t col = 0; col != 8; ++col) {
						uint8_t spritePixel = (spriteByte >> (7 - col)) & 0x1;

						uint8_t* pixel =
						  &c8.Display[((yPos + row) * 64) + (xPos + col)];

						if (spritePixel == 1 && *pixel == 1) { c8.V[0xF] = 1; }

						*pixel ^= spritePixel;
					}
				}
			}
			c8.PC += 2;
			break;
		case 0xE:
			switch (NN) {
				case 0x9E: /*SKP Vx: Skip next instruction if key with the value
				              of Vx is pressed */
					LOG("SKP Vx");
					c8.PC += (c8.Key[c8.V[X]]) ? 4 : 2;
					break;
				case 0xA1: /* SKNP: Vx: Skip next instruction if key with the
				              value of Vx is not pressed */
					LOG("SKNP Vx");
					c8.PC += (!c8.Key[c8.V[X]]) ? 4 : 2;
					break;
				default:
					UNKNOWN_INS;
					break;
			}
			break;
		case 0xF:
			switch (NN) {
				case 0x07: /* LD Vx, DT: Set Vx = delay timer value */
					LOG("LD Vx, DT");
					c8.V[X] = c8.DT;
					break;
				case 0x0A: /* LD Vx, Key: Wait for key press, store value of key
				              in Vx */
					LOG("LD Vx, Key");
					for (uint8_t i = 0; i != 16; ++i) {
						if (c8.Key[i] == 1) { c8.V[X] = i; }
					}
					c8.PC -= 2;
					break;
				case 0x15: /* LD DT, Vx: Set delay timer = Vx */
					LOG("LD DT, Vx");
					c8.DT = c8.V[X];
					break;
				case 0x18: /*LD ST, Vx: Set sound timer = Vx */
					LOG("LD ST, Vx");
					c8.ST = c8.V[X];
					break;
				case 0x1E: /* ADD I, Vx: Set I = I + Vx */
					LOG("ADD I, Vx");
					c8.I += c8.V[X];
					break;
				case 0x29: /*LD F, Vx: Set I = location of sprite for digit Vx
				            */
					LOG("LD F, Vx");
					c8.I = 5 * c8.V[X];
					break;
				case 0x33: /* LD B, Vx: Store BCD representation of Vx in memory
				              locations I, I + 1, and I + 2 */
					LOG("LD B, Vx");
					c8.Memory[c8.I]     = (c8.V[X] % 1000) / 100;
					c8.Memory[c8.I + 1] = (c8.V[X] % 100) / 10;
					c8.Memory[c8.I + 2] = (c8.V[X] % 10);
					break;
				case 0x55: /*LD {I}, Vx: Store registers V0 through Vx in memory
				              starting at location I */
					LOG("LD {i}, Vx");
					for (uint8_t i = 0; i <= X; ++i) {
						c8.Memory[c8.I + i] = c8.V[i];
					}
					break;
				case 0x65: /*LD Vx, {I}: Read registers V0 through Vx from
				              memory starting at location I */
					LOG("LD Vx, {I}");
					for (uint8_t i = 0; i <= X; ++i) {
						c8.V[i] = c8.Memory[c8.I + i];
					}
					break;
				default:
					UNKNOWN_INS;
					break;
			}
			c8.PC += 2;
			break;
		default:
			UNKNOWN_INS;
			break;
	}
	/* Tick */
	if (c8.DT > 0) { --c8.DT; }
	if (c8.ST > 0) { --c8.ST; }
}
#endif
