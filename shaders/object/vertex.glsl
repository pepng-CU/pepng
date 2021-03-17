#version 330

layout(location=0) in vec3 a_position;
layout(location=1) in vec3 a_normal;
layout(location=2) in vec2 a_tex_coord;

uniform mat4 u_projection;
uniform mat4 u_world;
uniform mat4 u_view;
uniform mat4 u_normal;
uniform float u_far;

out vec2 tex_coord;
out vec4 frag_pos;
out vec3 normal;

void main() {
    frag_pos = u_world * vec4(a_position, 1.0);
    normal = transpose(inverse(mat3(u_world))) * a_normal;
    tex_coord = a_tex_coord;
    gl_Position = u_projection * u_view * frag_pos;
}