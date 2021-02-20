#version 330

layout(location=0) in vec3 a_position;
layout(location=1) in vec3 a_normal;
layout(location=2) in vec2 a_tex_coord;

uniform mat4 u_projection;
uniform mat4 u_world;
uniform mat4 u_view;
uniform mat4 u_normal;

uniform mat4 u_light_space_matrix;

out vec2 tex_coord;
out vec3 frag_pos;
out vec3 normal;
out vec4 frag_pos_light_space;

void main() {
    frag_pos = vec3(u_world * vec4(a_position, 1.0));
    normal = normalize(transpose(inverse(mat3(u_world))) * a_normal);
    frag_pos_light_space = u_light_space_matrix * vec4(frag_pos, 1.0);
    tex_coord = a_tex_coord;
    gl_Position = u_projection * u_view * vec4(frag_pos, 1.0);
}