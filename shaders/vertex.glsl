#version 330

layout(location=0) in vec3 a_position;
layout(location=1) in vec3 a_normal;
layout(location=2) in vec2 a_tex_coord;

uniform mat4 u_projection;
uniform mat4 u_world;
uniform mat4 u_view;

out vec2 tex_coord;
out float light;

void main(void) {
    vec4 normal = normalize(u_world * vec4(a_normal, 1.0));
    light = normal.y;
    tex_coord = a_tex_coord;
    gl_Position = u_projection * u_view * u_world * vec4(a_position, 1.0);
}