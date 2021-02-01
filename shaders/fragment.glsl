varying vec3 color_factor;

void main(void) {
    // gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    gl_FragColor = vec4(color_factor.xyz, 1.0);
}