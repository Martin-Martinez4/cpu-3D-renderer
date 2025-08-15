#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <math.h>

#include "display.h"
#include "vector.h"
#include "matrix.h"
#include "mesh.h"
#include "array.h"
#include "light.h"
#include "texture.h"

const float PI = 3.14159265358979323846;

enum cull_method{
  CULL_NONE,
  CULL_BACKFACE
} cull_method;

enum render_method {
  RENDER_WIRE,
  RENDER_WIRE_VERTEX,
  RENDER_FILL_TRIANGLE,
  RENDER_FILL_TRIANGLE_WIRE,
  RENDER_TEXTURED,
  RENDER_TEXTURED_WIRE
} render_method;

triangle_t* triangles_to_render = NULL;

bool is_running = false;
int previous_frame_time = 0;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

vec3_t camera_position = { .x = 0, .y = 0, .z = 0 };
SDL_Texture* color_buffer_texture = NULL;
uint32_t* color_buffer = NULL;
int window_width = 800;
int window_height = 600;
mat4_t proj_matrix;


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

  float fov = PI/3;
  float aspect = (float)window_height / (float)window_width;
  float znear = 0.1;
  float zfar = 100.0;
  proj_matrix = mat4_make_perspective(fov, aspect, znear, zfar);

  mesh_texture = (uint32_t*)REDBRICK_TEXTURE; 
  texture_width = 64;
  texture_height = 64;

  // load_obj_file_data("./assets/f22.obj");
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
      if (event.key.keysym.sym == SDLK_5)
        render_method = RENDER_TEXTURED;
      if (event.key.keysym.sym == SDLK_6)
        render_method = RENDER_TEXTURED_WIRE;
      if (event.key.keysym.sym == SDLK_c)
          cull_method = CULL_BACKFACE;
      if (event.key.keysym.sym == SDLK_d)
          cull_method = CULL_NONE;
      break;
  }
}

// vec2_t project(vec3_t point){
//   vec2_t projected_point = {
//     .x = (fov_factor * point.x)/point.z,
//     .y = (fov_factor * point.y)/point.z,
//   };

//   return projected_point;

// }

 int compare (const void * a, const void * b){
    triangle_t fa = *(const triangle_t*) a;
    triangle_t fb = *(const triangle_t*) b;

    // trick to return -1, 0, 1 when comparing floats
    return (fa.avg_depth < fb.avg_depth) - (fa.avg_depth > fb.avg_depth);
  }

void update(void){

  while(!SDL_TICKS_PASSED(SDL_GetTicks(), previous_frame_time + FRAME_TARGET_TIME))

  previous_frame_time = SDL_GetTicks();

  triangles_to_render = NULL;

  mesh.rotation.x += 0.1;
  // mesh.rotation.y += 0.02;
  mesh.rotation.z += 0.02;

  mesh.scale.x += 0.005;
  mesh.scale.y += 0.005;

  mesh.translation.x += 0.02;
  mesh.translation.y += 0.02;

  // camera translation
  mesh.translation.z = 5.0;


  mesh.transformations = mat4_identity();
  // mesh.transformations = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
  // mesh.transformations = mat4_mul_mat4(mesh.transformations, mat4_make_translate(mesh.translation.x, mesh.translation.y, mesh.translation.z));
   mesh.transformations = mat4_mul_mat4(mesh.transformations, mat4_make_translate(0, 0, mesh.translation.z));
  mesh.transformations = mat4_mul_mat4(mesh.transformations, mat4_make_rotation_along_x(mesh.rotation.x));
  mesh.transformations = mat4_mul_mat4(mesh.transformations, mat4_make_rotation_along_y(mesh.rotation.y));

  // mesh.transformations = mat4_mul_mat4(mesh.transformations, mat4_make_translate(0, 0, mesh.translation.z));


  int num_faces = array_length(mesh.faces);
  for(int i = 0; i < num_faces; i++){
    face_t mesh_face = mesh.faces[i];

    vec3_t face_vertices[3];
    face_vertices[0] = mesh.vertices[mesh_face.a - 1];
    face_vertices[1] = mesh.vertices[mesh_face.b - 1];
    face_vertices[2] = mesh.vertices[mesh_face.c -1];


    vec4_t transformed_vertices[3];

    for(int j = 0; j < 3; j++){
      vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

      // 
      transformed_vertex = mat4_mul_vec4(mesh.transformations, transformed_vertex);

      // transformed_vertex.z -= -5;

      transformed_vertices[j] = transformed_vertex;
    }

      // check for back-face culling
      vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]);
      vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]);
      vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]);
  
      vec3_t vector_ab = vec3_sub(vector_b, vector_a);
      vec3_t vector_ac = vec3_sub(vector_c, vector_a);
      vec3_normalize(&vector_ab);
      vec3_normalize(&vector_ac);
  
      vec3_t normal = vec3_cross(vector_ab, vector_ac);
      vec3_normalize(&normal);
  
      vec3_t camera_ray = vec3_sub(camera_position, vector_a);
  
      float dot_normal_camera = vec3_dot(normal, camera_ray);

      if(cull_method == CULL_BACKFACE){

        if(dot_normal_camera < 0){
          continue;
        }
      }

    vec4_t projected_points[3];

    // perform projection
    for(int j = 0; j < 3; j++){
      projected_points[j] = mat4_mul_vec4_project(proj_matrix, transformed_vertices[j]);

      // scale into view
      projected_points[j].x *= (window_width / 2.0);
      projected_points[j].y *= (window_height / 2.0);

      // invert y to account for flipped screen y coordinate
      projected_points[j].y *= -1;

      // translate to middle of screen
      projected_points[j].x += (window_width / 2.0);
      projected_points[j].y += (window_height / 2.0);

      
    };
    
    // Calculate average depth after transformation
    float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z)/3.0;

    // calculate color based on light angle
    float light_intensity_factor = -vec3_dot(normal, light.direction);

    uint32_t triangle_color = light_apply_intensity(mesh_face.color, light_intensity_factor);

    triangle_t projected_triangle = {
      .points = {
        { projected_points[0].x, projected_points[0].y },
        { projected_points[1].x, projected_points[1].y },
        { projected_points[2].x, projected_points[2].y },
      },
      .textcoords = {
        { mesh_face.a_uv.u, mesh_face.a_uv.v },
        { mesh_face.b_uv.u, mesh_face.b_uv.v },
        { mesh_face.c_uv.u, mesh_face.c_uv.v }
      },
      .color = triangle_color,
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

        // Draw Textured
        if(render_method == RENDER_TEXTURED || render_method == RENDER_TEXTURED_WIRE){
           draw_textured_triangle(
              triangle.points[0].x,
              triangle.points[0].y,
              triangle.textcoords[0].u,
              triangle.textcoords[0].v,

              triangle.points[1].x,
              triangle.points[1].y,
              triangle.textcoords[1].u,
              triangle.textcoords[1].v,

              triangle.points[2].x,
              triangle.points[2].y,
              triangle.textcoords[2].u,
              triangle.textcoords[2].v,
              mesh_texture
          );
        }

        if (render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX || render_method == RENDER_FILL_TRIANGLE_WIRE || render_method == RENDER_TEXTURED_WIRE){

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