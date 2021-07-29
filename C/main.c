#include <SDL2/SDL.h>
#include <stdbool.h>

#include "chip8.h"

int main(int argc, char** argv) {
	if (argc != 2) { return 1; }

	bool           isRunning = true;
	const uint16_t scrWidth  = 1280;
	const uint16_t scrHeight = 640;
	const uint8_t  pixPerRow = 64;
	const uint8_t  pixPerCol = 32;

	SDL_Window*   pwin;
	SDL_Renderer* pren;
	SDL_Texture*  pscr;
	SDL_Event     ev;

	const uint8_t keymap[16] = {SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_q, SDLK_w,
	                            SDLK_e, SDLK_r, SDLK_a, SDLK_s, SDLK_d, SDLK_f,
	                            SDLK_z, SDLK_x, SDLK_c, SDLK_v};

	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(scrWidth, scrHeight, SDL_WINDOW_INPUT_FOCUS,
	                            &pwin, &pren);
	SDL_RenderSetLogicalSize(pren, pixPerRow, pixPerCol);

	pscr = SDL_CreateTexture(pren, SDL_PIXELFORMAT_RGB332,
	                         SDL_TEXTUREACCESS_STREAMING, pixPerRow, pixPerCol);

	struct Chip8* c8 = init_or_reset();
	load_rom(argv[1]);

	while (isRunning) {
		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) {
				isRunning = false;
			}
			for (uint8_t i = 0; i != 16; ++i) {
				if (ev.key.keysym.sym == keymap[i]) {
					if (ev.type == SDL_KEYDOWN) {
						c8->Key[i] = 1;
					} else {
						c8->Key[i] = 0;
					}
				}
			}
		}

		emulate_cycle();

		SDL_UpdateTexture(pscr, 0, (void*)&c8->Display,
		                  pixPerRow * sizeof(uint8_t));
		SDL_RenderClear(pren);
		SDL_RenderCopy(pren, pscr, 0, 0);
		SDL_RenderPresent(pren);
		SDL_Delay(10);
	}

	SDL_Quit();
	return 0;
}
