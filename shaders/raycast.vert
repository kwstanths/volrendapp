#version 330

layout (location = 0) in vec3 vert;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

smooth out vec3 tex_coords;
smooth out vec3 position;
out vec3 camera_position_world;

void main(void)  {
  tex_coords = vert + vec3(0.5);
  position = vert;

  mat4 view_inv = inverse(view * model);
  camera_position_world = vec3(view_inv[3][0], view_inv[3][1], view_inv[3][2]);

  gl_Position = projection * view * model * vec4(vert, 1);
}
