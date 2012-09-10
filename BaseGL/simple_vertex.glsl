#version 400

layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec2 vertex_tex_coord;

out vec2 tex_coord;

void main( void )
{
  tex_coord = vertex_tex_coord;
  gl_Position = vec4(vertex_pos, 1.0);
}
