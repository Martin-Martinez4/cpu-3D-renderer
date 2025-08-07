#include "display.h"



bool initialize_window(void){
  if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
    fprintf(stderr, "Error initializing SDL.\n");
    return false;
  }

  SDL_DisplayMode display_mode;
  SDL_GetCurrentDisplayMode(0, &display_mode);

  window_width = display_mode.w;
  window_height = display_mode.h;

  window = SDL_CreateWindow(
    NULL, 
    SDL_WINDOWPOS_CENTERED, 
    SDL_WINDOWPOS_CENTERED, 
    window_width, 
    window_height, 
    SDL_WINDOW_BORDERLESS
  );
  if(!window){
    fprintf(stderr, "Error creating SDL window.\n");
    return false;
  }

  renderer = SDL_CreateRenderer(window, -1, 0);
  if(!renderer){
    fprintf(stderr, "Error creating SDL renderer.\n");
    return false;
  }

  // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
  return true;
}

void render_color_buffer(void){
  SDL_UpdateTexture(
    color_buffer_texture,
    NULL,
    color_buffer,
    (int)(window_width * sizeof(uint32_t))
  );
  SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void clear_color_buffer(uint32_t color){
  for(int y =  0; y < window_height; y++){
    for(int x = 0; x < window_width; x++){
      color_buffer[(window_width * y) + x] = color;
    }
  }
}

void draw_grid(uint32_t color, uint8_t size,uint8_t line_width){
   for(int y =  0; y < window_height; y++){
    for(int x = 0; x < window_width; x++){
      
      if((y % size <= line_width) || (x % size <= line_width)){

        color_buffer[(window_width * y) + x] = color;
      }
    }
  }
}

void draw_pixel(int x, int y, uint32_t color){
  if(x < window_width && y < window_height){

    color_buffer[(window_width * y) + x] = color;
  }
}

void draw_rectangle(int x, int y, int width, int height, uint32_t color){
  for(int h = 0; h < height; h++){
    int current_row = ((h+y)*window_width) + x;

    if(current_row >= (window_height * window_width) || current_row < 0){
      continue;
    }

    for(int w = 0; w < width; w++){


      if((current_row + w) >= window_width*(h+y+1) + x || (current_row + w) < 0){
        continue;
      }
      color_buffer[current_row + w] = color;
    }
  }
}

void render(void){
  SDL_SetRenderDrawColor(renderer, 222,210,187,25);
  SDL_RenderClear(renderer);

  // draw_grid(0xFFFF0000, 100, 20);
  draw_rectangle(window_width/2, window_height/2, 300, 150, 0xFF00FF00);
  draw_rectangle(window_width/2, window_height/2, 75, 300, 0xFF000000);
  draw_pixel(window_width/2, window_height/2, 0xFFFFFFFF);
  render_color_buffer();
  clear_color_buffer(0xFFFFFF00);

  SDL_RenderPresent(renderer);
}

void destroy_window(void){
  free(color_buffer);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
