#include <SDL2/SDL.h>

#include <iostream>

#include "chip8.hh"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    return -1;
  }
  bool               isRunning = true;
  constexpr uint16_t scrWidth  = 1280;
  constexpr uint16_t scrHeight = 640;
  constexpr uint8_t  pixPerRow = 64;
  constexpr uint8_t  pixPerCol = 32;

  SDL_Window*   pwin    = nullptr;
  SDL_Renderer* pren    = nullptr;
  SDL_Texture*  pscrTxr = nullptr;
  SDL_Event     event;

  SDL_Init(SDL_INIT_VIDEO);
  pwin =
      SDL_CreateWindow("C8E", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                       scrWidth, scrHeight, SDL_WINDOW_INPUT_FOCUS);
  pren = SDL_CreateRenderer(
      pwin, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  SDL_RenderSetLogicalSize(pren, pixPerRow, pixPerCol);
  pscrTxr =
      SDL_CreateTexture(pren, SDL_PIXELFORMAT_RGB332,
                        SDL_TEXTUREACCESS_STREAMING, pixPerRow, pixPerCol);

  // Chip8
  Chip8 c8;
  c8.init_or_reset();
  c8.load_rom(argv[1]);

  while (isRunning) {
    while (SDL_PollEvent(&event) != 0) {
      switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
          if (event.type == SDL_KEYDOWN) {
            isRunning = false;
          }
          break;
        case SDLK_x:
          if (event.type == SDL_KEYDOWN) {
            c8.Key[0x0] = 1;
          } else {
            c8.Key[0x0] = 0;
          }
          break;
        case SDLK_1:
          if (event.type == SDL_KEYDOWN) {
            c8.Key[0x1] = 1;
          } else {
            c8.Key[0x1] = 0;
          }
          break;
        case SDLK_2:
          if (event.type == SDL_KEYDOWN) {
            c8.Key[0x2] = 1;
          } else {
            c8.Key[0x2] = 0;
          }
          break;
        case SDLK_3:
          if (event.type == SDL_KEYDOWN) {
            c8.Key[0x3] = 1;
          } else {
            c8.Key[0x3] = 0;
          }
          break;
        case SDLK_q:
          if (event.type == SDL_KEYDOWN) {
            c8.Key[0x4] = 1;
          } else {
            c8.Key[0x4] = 0;
          }
          break;
        case SDLK_w:
          if (event.type == SDL_KEYDOWN) {
            c8.Key[0x5] = 1;
          } else {
            c8.Key[0x5] = 0;
          }
          break;
        case SDLK_e:
          if (event.type == SDL_KEYDOWN) {
            c8.Key[0x6] = 1;
          } else {
            c8.Key[0x6] = 0;
          }
          break;
        case SDLK_a:
          if (event.type == SDL_KEYDOWN) {
            c8.Key[0x7] = 1;
          } else {
            c8.Key[0x7] = 0;
          }
          break;
        case SDLK_s:
          if (event.type == SDL_KEYDOWN) {
            c8.Key[0x8] = 1;
          } else {
            c8.Key[0x8] = 0;
          }
          break;
        case SDLK_d:
          if (event.type == SDL_KEYDOWN) {
            c8.Key[0x9] = 1;
          } else {
            c8.Key[0x9] = 0;
          }
          break;
        case SDLK_z:
          if (event.type == SDL_KEYDOWN) {
            c8.Key[0xA] = 1;
          } else {
            c8.Key[0xA] = 0;
          }
          break;
        case SDLK_c:
          if (event.type == SDL_KEYDOWN) {
            c8.Key[0xB] = 1;
          } else {
            c8.Key[0xB] = 0;
          }
          break;
        case SDLK_4:
          if (event.type == SDL_KEYDOWN) {
            c8.Key[0xC] = 1;
          } else {
            c8.Key[0xC] = 0;
          }
          break;
        case SDLK_r:
          if (event.type == SDL_KEYDOWN) {
            c8.Key[0xD] = 1;
          } else {
            c8.Key[0xD] = 0;
          }
          break;
        case SDLK_f:
          if (event.type == SDL_KEYDOWN) {
            c8.Key[0xE] = 1;
          } else {
            c8.Key[0xE] = 0;
          }
          break;
        case SDLK_v:
          if (event.type == SDL_KEYDOWN) {
            c8.Key[0xF] = 1;
          } else {
            c8.Key[0xF] = 0;
          }
          break;
      }
    }

    c8.emulate_cycle();  // keep calling till end of file.
    if (c8.drawFlag) {
      SDL_UpdateTexture(pscrTxr, nullptr, (void*)&c8.Display,
                        64 * sizeof(uint8_t));
      SDL_RenderClear(pren);
      SDL_RenderCopy(pren, pscrTxr, nullptr, nullptr);
      SDL_RenderPresent(pren);
    }
  }

  SDL_DestroyTexture(pscrTxr);
  SDL_DestroyRenderer(pren);
  SDL_DestroyWindow(pwin);
  SDL_Quit();
  return 0;
}
