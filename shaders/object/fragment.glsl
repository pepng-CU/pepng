#version 330

uniform sampler2D u_texture;
uniform sampler2D u_shadow;

uniform vec3 u_light_pos;
uniform vec3 u_light_color;
uniform vec3 u_camera_pos;

in vec2 tex_coord;
in vec3 position;
in vec4 frag_pos;
in vec3 normal;
in vec4 frag_pos_light_space;

out vec4 color;

float calculate_shadow(vec4 fpls, float bias) {
    vec3 proj_coords = fpls.xyz / fpls.w;
    proj_coords = proj_coords * 0.5 + 0.5;
    float closest_depth = texture(u_shadow, proj_coords.xy).r; 
    float current_depth = proj_coords.z;
    float shadow_value = current_depth > closest_depth ? 0.75 : 0.0;

    return shadow_value;
}

vec3 calculate_light_dir() {
    return normalize(frag_pos.xyz - u_light_pos);
}

vec3 calculate_ambient(float Ka) {
    vec4 texture_point = texture(u_texture, tex_coord);
    vec3 base_color = texture_point.rgb;

    return Ka * base_color;
}

vec3 calculate_diffuse(float Kd, vec3 light_dir) {
    float diff = max(dot(light_dir, normal), 0.0);

    return Kd * diff * u_light_color;
}

vec3 calculate_specular(float Ks, float shine, vec3 light_dir) {
    vec3 view_dir = normalize(u_camera_pos - frag_pos.xyz);
    vec3 halfway_dir = normalize(light_dir + view_dir);  
    float spec = pow(max(dot(normal, halfway_dir), 0.0), shine);

    return Ks * spec * u_light_color;  
}

vec3 calculate_phong(vec3 light_dir) {
    vec3 ambient = calculate_ambient(0.15);
    vec3 diffuse = calculate_diffuse(1.0, light_dir);
    vec3 specular = calculate_specular(1.0, 128, light_dir);

    return ambient + diffuse + specular;
}

vec3 calculate_light() {
    vec3 light_dir = calculate_light_dir();
    vec3 light = calculate_phong(light_dir);

    float bias = max(0.05 * (1.0 - dot(normal, light_dir)), 0.005); 
    float shadow = 1.0 - calculate_shadow(frag_pos_light_space, bias);

    return light * shadow; 
}

vec4 calculate_color() {
    vec4 texture_point = texture(u_texture, tex_coord);

    return vec4(calculate_light() * texture_point.rgb, texture_point.a); 
}

void main() {
    color = calculate_color();
}