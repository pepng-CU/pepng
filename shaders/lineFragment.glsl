#version 330

out vec4 color;
in vec3 color_factor;

void main(void) {
    color = vec4(color_factor.xyz, 1.0);
}