#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>

#include "display.h"

bool is_running = false;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* color_buffer_texture = NULL;
uint32_t* color_buffer = NULL;
int window_width = 800;
int window_height = 600;


void setup(void){
  color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);
  if(!color_buffer){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }
  // SDL_PIXELFORMAT_ARGB8888 Aplha, Red, Green, Blue, 8, 8, 8, 8
  color_buffer_texture = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_STREAMING,
    window_width,
    window_height
  );
  
  color_buffer[(window_width * 10) + 20] = 0xFFFF0000;


}

void process_input(void){
  SDL_Event event;
  SDL_PollEvent(&event);

  switch (event.type){
    case SDL_QUIT:
      is_running = false;
      break;
    case SDL_KEYDOWN:
      if(event.key.keysym.sym == SDLK_ESCAPE){
        is_running = false;
      }
      break;
  }
}

void update(void){

}


int main(void){
  is_running = initialize_window();

  setup();

  while(is_running){
    process_input();
    update();
    render();
  }

  destroy_window();

  return 0;
}