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

bool load_png_texture_data(const char* filename);

tex2_t tex2_clone(tex2_t* t);

extern const uint8_t REDBRICK_TEXTURE[];
extern uint32_t* mesh_texture;

#endif