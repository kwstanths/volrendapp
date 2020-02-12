#version 330

layout (location = 0) in vec3 vert;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(void)  {
  gl_PointSize = 10.0;
  gl_Position = projection * view * model * vec4(vert, 1);
}
