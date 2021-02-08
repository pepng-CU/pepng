#version 330

layout(location=0) in vec3 a_position;
layout(location=1) in vec3 a_normal;
layout(location=2) in vec2 a_tex_coord;

uniform mat4 u_projection;
uniform mat4 u_world;
uniform mat4 u_view;
uniform mat4 u_normal;

out vec2 tex_coord;
out vec3 light;

void main(void) {
    vec3 light_pos = vec3(0.0, 10.0, 0.0);
    vec4 normal = normalize(transpose(inverse(u_world)) * vec4(a_normal, 1.0));
    float diff = max(dot(normal.xyz, light_pos - a_position), 0.0);
    float dist = 5.0 / distance(light_pos, a_position);
    light = dist * diff * vec3(0.5);
    tex_coord = a_tex_coord;
    gl_Position = u_projection * u_view * u_world * vec4(a_position, 1.0);
}