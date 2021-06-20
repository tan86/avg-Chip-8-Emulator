#include <SDL2/SDL.h>

#include "chip8.hh"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    return -1;
  }
  Chip8 c8;

  // SDL
  constexpr uint16_t scrWidth  = 1280;
  constexpr uint16_t scrHeight = 640;
  constexpr uint8_t  pixPerRow = 64;
  constexpr uint8_t  pixPerCol = 32;

  bool          isRunning = true;
  SDL_Window*   pwin      = nullptr;
  SDL_Renderer* pren      = nullptr;
  SDL_Texture*  pscrTxr   = nullptr;
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
  c8.init_or_reset();
  c8.load_rom(argv[1]);
  // uint8_t* pdsp = &c8.Display[0];

  while (isRunning) {
    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
          case SDLK_ESCAPE:
            isRunning = false;
            break;
          case SDLK_SPACE:
            c8.emulate_cycle();
            break;
        }
      }
    }
    // c8.emulate_cycle();  // keep calling till end of file.
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
