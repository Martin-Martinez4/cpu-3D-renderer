#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern SDL_Texture* color_buffer_texture;
extern uint32_t* color_buffer;
extern int window_width;
extern int window_height;


#include "display.h"

bool initialize_window(void);
void render_color_buffer(void);
void clear_color_buffer(uint32_t color);
void draw_grid(uint32_t color, uint8_t size,uint8_t line_width);
void draw_rectangle(int x, int y, int width, int height, uint32_t color);
void render(void);



#endif
