#version 330

out vec4 color;
in vec2 tex_coord;
in float light;

uniform sampler2D u_texture;

void main(void) {
    color = texture(u_texture, tex_coord) * light;
}