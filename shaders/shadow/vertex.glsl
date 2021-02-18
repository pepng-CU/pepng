#version 330 core
layout (location = 0) in vec3 a_position;

uniform mat4 u_light_space_matrix;
uniform mat4 u_world;

void main() {
    gl_Position = u_light_space_matrix * u_world * vec4(a_position, 1.0);
}