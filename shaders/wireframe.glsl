#version 330
layout(triangles) in;
layout(line_strip, max_vertices = 3) out;

uniform mat4 u_projection;
uniform mat4 u_world;
uniform mat4 u_view;

uniform vec3 light_direction;

out vec4 color_factor;

void main() {

    // Normal Calculations
    mediump vec3 normal = normalize(
        cross(
            vec3(gl_in[1].gl_Position - gl_in[0].gl_Position),
            vec3(gl_in[2].gl_Position - gl_in[0].gl_Position)
        )
    );

    vec3 light_direction = vec3(u_projection * u_view * u_world * vec4(normalize(vec3(0.0, -1.0, -0.5)), 0.0));

    float light_magnitude = 0.1 + clamp(dot(normal, light_direction), 0.0, 1.0) * 0.9;

    color_factor = vec4(light_magnitude, 
                        light_magnitude, 
                        light_magnitude, 
                        1.0);

    // Emit Vertices as Lines
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}
