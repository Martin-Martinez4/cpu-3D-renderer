#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <stdbool.h>

#include <stdint.h>

typedef struct {
  float u;
  float v;
} tex2_t;

extern int texture_width;
extern int texture_height;

bool load_png_texture_data(char* filename);

extern const uint8_t REDBRICK_TEXTURE[];
extern uint32_t* mesh_texture;

#endif