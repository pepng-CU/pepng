#version 330 core
in vec4 FragPos;

uniform vec3 u_light_pos;
uniform float u_far;

void main() {
    float light_distance = length(FragPos.xyz - u_light_pos);

    light_distance /= u_far;

    gl_FragDepth = light_distance;
}