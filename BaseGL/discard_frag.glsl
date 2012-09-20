#version 400

in vec3 front_color;
in vec3 back_color;
in vec2 tex_coord;

layout (location = 0) out vec4 frag_color;

void main()
{
  const float scale = 15.0;
  bvec2 to_discard = greaterThan(fract(tex_coord * scale), vec2(0.2, 0.2));
  if(all(to_discard))
  {
    discard;
  }
  else if(gl_FrontFacing)
  {
    frag_color = vec4(front_color, 1.0);
  }
  else
  {
    frag_color = vec4(back_color, 1.0);
  }
}