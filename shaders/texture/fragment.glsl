#version 330

uniform sampler2D u_texture;
uniform sampler2D u_shadow;

in vec2 tex_coord;

out vec4 color;

void main() {
    color = vec4(vec3(1.0 - texture(u_shadow, tex_coord).r), 1.0);
}