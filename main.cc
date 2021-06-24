#include <SDL2/SDL.h>

#include <array>
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

    constexpr std::array<uint8_t, 16> keyMap{
        SDLK_x, SDLK_1, SDLK_2, SDLK_3, SDLK_q, SDLK_w, SDLK_e, SDLK_a,
        SDLK_s, SDLK_d, SDLK_z, SDLK_c, SDLK_4, SDLK_r, SDLK_r, SDLK_v};

    SDL_Init(SDL_INIT_VIDEO);
    pwin = SDL_CreateWindow("C8E", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, scrWidth, scrHeight,
                            SDL_WINDOW_INPUT_FOCUS);
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
            if (event.type == SDL_KEYDOWN &&
                event.key.keysym.sym == SDLK_ESCAPE) {
                isRunning = false;
            }
            for (uint8_t i = 0; i < 16; ++i) {
                if (event.key.keysym.sym == keyMap[i]) {
                    if (event.type == SDL_KEYDOWN) {
                        c8.setKey(i, 1);
                    } else {
                        c8.setKey(i, 0);
                    }
                }
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
