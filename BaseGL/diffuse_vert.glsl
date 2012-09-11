#version 400

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;

out vec3 light_intensity;

uniform vec4 light_position;
uniform vec3 kd;
uniform vec3 ld;
uniform mat4 model_view_matrix;
uniform mat3 normal_matrix;
uniform mat4 mvp;

void main()
{
  //convert normal and position to eye coords
  vec3 tnorm = normalize(normal_matrix * vertex_normal);
  vec4 eye_coords = model_view_matrix * vec4(vertex_position, 1.0);
  vec3 s = normalize(vec3(light_position - eye_coords));

  //diffuse shading equation
  light_intensity = ld * kd * max(dot(s, tnorm), 0.0);

  gl_Position = mvp * vec4(vertex_position, 1.0);
}