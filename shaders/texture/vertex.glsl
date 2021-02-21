#version 330 core
layout (location = 0) in vec2 a_position;

out vec2 tex_coord;

void main() {
    tex_coord = a_position / 2 + vec2(0.5);
    gl_Position = vec4(a_position, -1.0, 1.0);
}