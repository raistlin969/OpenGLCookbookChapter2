#version 400

in vec2 tex_coord;
layout (location = 0) out vec4 frag_color;
uniform BlobSettings
{
  vec4 inner_color;
  vec4 outer_color;
  float inner_radius;
  float outer_radius;
};

void main( void )
{
  float dx = tex_coord.x - 0.5;
  float dy = tex_coord.y - 0.5;
  float dist = sqrt(dx * dx + dy * dy);

  frag_color = mix(inner_color, outer_color, smoothstep(inner_radius, outer_radius, dist));
}
