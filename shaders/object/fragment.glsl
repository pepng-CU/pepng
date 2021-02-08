#version 330

out vec4 color;
in vec2 tex_coord;
in vec3 light;

uniform sampler2D u_texture;

void main(void) {
    vec4 tex_color = texture(u_texture, tex_coord);

    color = vec4(tex_color.xyz * light, tex_color.w);
}