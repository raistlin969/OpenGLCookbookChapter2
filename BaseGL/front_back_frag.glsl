#version 400

in vec3 front_color;
in vec3 back_color;

layout (location = 0) out vec4 frag_color;

void main()
{
  if(gl_FrontFacing)
  {
    frag_color = vec4(front_color, 1.0);
  }
  else
  {
    frag_color = vec4(back_color, 1.0);
  }
}