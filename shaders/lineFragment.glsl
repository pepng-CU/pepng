#version 330

out vec4 color;
in vec4 color_factor;

void main(void) {
    color = color_factor;
}