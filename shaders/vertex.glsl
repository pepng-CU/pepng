attribute vec3 a_position;

uniform mat4 u_projection;
uniform mat4 u_world;
uniform mat4 u_view;

varying vec3 color_factor;

void main(void) {
    color_factor = a_position;
    gl_Position = u_projection * u_world * u_view * vec4(a_position, 1.0);
}