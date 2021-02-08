#version 330

out vec4 color;
in vec2 tex_coord;
in float light;

uniform sampler2D u_texture;

void main(void) {
    color = vec4(light, light, light, 1.0);
}