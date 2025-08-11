#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"


enum cull_method{
  CULL_NONE,
  CULL_BACKFACE
} cull_method;

enum render_method {
  RENDER_WIRE,
  RENDER_WIRE_VERTEX,
  RENDER_FILL_TRIANGLE,
  RENDER_FILL_TRIANGLE_WIRE
} render_method;

triangle_t* triangles_to_render = NULL;

bool is_running = false;
int previous_frame_time = 0;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

vec3_t camera_position = { .x = 0, .y = 0, .z = 0 };
SDL_Texture* color_buffer_texture = NULL;
uint32_t* color_buffer = NULL;
float fov_factor = 640;
int window_width = 800;
int window_height = 600;


void setup(void){
  render_method = RENDER_WIRE;
  cull_method = CULL_BACKFACE;

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

  // load_obj_file_data("./assets/cube.obj");
  load_cube_mesh_data();
}

void process_input(void){
  SDL_Event event;
  SDL_PollEvent(&event);

   switch (event.type) {
    case SDL_QUIT:
      is_running = false;
      break;
    case SDL_KEYDOWN:
      if (event.key.keysym.sym == SDLK_ESCAPE)
          is_running = false;
      if (event.key.keysym.sym == SDLK_1)
          render_method = RENDER_WIRE_VERTEX;
      if (event.key.keysym.sym == SDLK_2)
          render_method = RENDER_WIRE;
      if (event.key.keysym.sym == SDLK_3)
          render_method = RENDER_FILL_TRIANGLE;
      if (event.key.keysym.sym == SDLK_4)
          render_method = RENDER_FILL_TRIANGLE_WIRE;
      if (event.key.keysym.sym == SDLK_c)
          cull_method = CULL_BACKFACE;
      if (event.key.keysym.sym == SDLK_d)
          cull_method = CULL_NONE;
      break;
  }
}

vec2_t project(vec3_t point){
  vec2_t projected_point = {
    .x = (fov_factor * point.x)/point.z,
    .y = (fov_factor * point.y)/point.z,
  };

  return projected_point;

}

 int compare (const void * a, const void * b){
    triangle_t fa = *(const triangle_t*) a;
    triangle_t fb = *(const triangle_t*) b;

    // trick to return -1, 0, 1 when comparing floats
    return (fa.avg_depth < fb.avg_depth) - (fa.avg_depth > fb.avg_depth);
  }

void update(void){

  if(SDL_GetTicks() < previous_frame_time + FRAME_TARGET_TIME){
    continue;
  }

  previous_frame_time = SDL_GetTicks();

  triangles_to_render = NULL;

  mesh.rotation.x += 0.02;
  mesh.rotation.y += 0.02;
  mesh.rotation.z += 0.02;

  int num_faces = array_length(mesh.faces);
  for(int i = 0; i < num_faces; i++){
    face_t mesh_face = mesh.faces[i];

    vec3_t face_vertices[3];
    face_vertices[0] = mesh.vertices[mesh_face.a - 1];
    face_vertices[1] = mesh.vertices[mesh_face.b - 1];
    face_vertices[2] = mesh.vertices[mesh_face.c -1];


    vec3_t transformed_vertices[3];

    for(int j = 0; j < 3; j++){
      vec3_t transformed_vertex = face_vertices[j];

      transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
      transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
      transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);

      transformed_vertex.z -= -5;

      transformed_vertices[j] = transformed_vertex;
    }

    if(cull_method == CULL_BACKFACE){

      // check for back-face culling
      vec3_t vector_a = transformed_vertices[0];
      vec3_t vector_b = transformed_vertices[1];
      vec3_t vector_c = transformed_vertices[2];
  
      vec3_t vector_ab = vec3_sub(vector_b, vector_a);
      vec3_t vector_ac = vec3_sub(vector_c, vector_a);
      vec3_normalize(&vector_ab);
      vec3_normalize(&vector_ac);
  
      vec3_t normal = vec3_cross(vector_ab, vector_ac);
      vec3_normalize(&normal);
  
      vec3_t camera_ray = vec3_sub(camera_position, vector_a);
  
      float dot_normal_camera = vec3_dot(normal, camera_ray);
  
      if(dot_normal_camera < 0){
        continue;
      }
    }

    vec2_t projected_points[3];

    // perform projection
    for(int j = 0; j < 3; j++){
      projected_points[j] = project(transformed_vertices[j]);

      projected_points[j].x += (window_width / 2);
      projected_points[j].y += (window_height / 2);

      
    };
    
    // Calculate average depth after transformation
    float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z)/3;

    triangle_t projected_triangle = {
      .points = {
        { projected_points[0].x, projected_points[0].y },
        { projected_points[1].x, projected_points[1].y },
        { projected_points[2].x, projected_points[2].y },
      },
      .color = mesh_face.color,
      .avg_depth = avg_depth
    };
    array_push(triangles_to_render, projected_triangle);
    
  }

  int num_triangles = array_length(triangles_to_render);
  qsort(triangles_to_render, num_triangles, sizeof(triangle_t), compare);
 

}

void render(void){
  SDL_RenderClear(renderer);
  // SDL_SetRenderDrawColor(renderer, 222,210,187,25);
  // draw_grid(0xFFFF0000, 100, 1);

   // Loop all projected triangles and render them
    int num_triangles = array_length(triangles_to_render);
    for (int i = 0; i < num_triangles; i++) {
        triangle_t triangle = triangles_to_render[i];

        // Draw vertex points
        // draw_rectangle(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFFFF00);
        // draw_rectangle(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFFFF00);
        // draw_rectangle(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFFFF00);

        if(render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE){

          // Draw unfilled triangle
          draw_filled_triangle(
              triangle.points[0].x,
              triangle.points[0].y,
              triangle.points[1].x,
              triangle.points[1].y,
              triangle.points[2].x,
              triangle.points[2].y,
              triangle.color
          );
        }

        if (render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX || render_method == RENDER_FILL_TRIANGLE_WIRE){

          draw_triangle(
              triangle.points[0].x,
              triangle.points[0].y,
              triangle.points[1].x,
              triangle.points[1].y,
              triangle.points[2].x,
              triangle.points[2].y,
              0xFF00FF00
          );
        }

          // Draw triangle vertex points
        if (render_method == RENDER_WIRE_VERTEX) {
            draw_rectangle(triangle.points[0].x - 3, triangle.points[0].y - 3, 6, 6, 0xFFFF0000); // vertex A
            draw_rectangle(triangle.points[1].x - 3, triangle.points[1].y - 3, 6, 6, 0xFFFF0000); // vertex B
            draw_rectangle(triangle.points[2].x - 3, triangle.points[2].y - 3, 6, 6, 0xFFFF0000); // vertex C
        }
    }


  // draw_filled_triangle(300, 100, 50, 400, 500, 700, 0xFF00FF00);

  array_free(triangles_to_render);
  
  render_color_buffer();
  clear_color_buffer(0xFF000000);

  SDL_RenderPresent(renderer);
}

void free_resources(void) {
    free(color_buffer);
    array_free(mesh.faces);
    array_free(mesh.vertices);
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
  free_resources();

  return 0;
}