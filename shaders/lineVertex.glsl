#version 330

layout(location=0) in vec3 a_position;
layout(location=1) in vec3 a_color;

uniform mat4 u_projection;
uniform mat4 u_world;
uniform mat4 u_view;

out vec3 color_factor;

void main(void) {
    color_factor = a_color;
    gl_Position = u_projection * u_view * u_world * vec4(a_position, 1.0);
}