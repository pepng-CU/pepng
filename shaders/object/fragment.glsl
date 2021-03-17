#version 330

uniform sampler2D u_texture;
uniform samplerCube u_shadow;

uniform vec3 u_light_pos;
uniform vec3 u_light_color;
uniform vec3 u_camera_pos;
uniform float u_far;
uniform bool u_receive_shadow;
uniform bool u_display_texture;

in vec2 tex_coord;
in vec3 position;
in vec4 frag_pos;
in vec3 normal;

out vec4 color;

vec3 sampleOffsetDirections[20] = vec3[](
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

float calculate_shadow(float bias) {
    // Point Light + PCF
    vec3 frag_to_light = frag_pos.xyz - u_light_pos;
    float current_depth = length(frag_to_light);

    float shadow = 0.0;
    int samples = 20;
    float view_distance = length(u_camera_pos - frag_pos.xyz);
    float disk_radius = (1.0 + (view_distance / u_far)) / 25.0;  

    for(int i = 0; i < samples; ++i) {
        float closest_depth = texture(u_shadow, frag_to_light + sampleOffsetDirections[i] * disk_radius).r;
        closest_depth *= u_far;
        if(current_depth - bias > closest_depth) shadow += 1.0;
    }
    
    shadow /= float(samples); 

    return shadow;
}

vec3 calculate_light_dir() {
    return normalize(u_light_pos - frag_pos.xyz);
}

vec3 calculate_ambient(float Ka, vec3 base_color) {
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

vec3 calculate_light(vec4 texture_point) {
    vec3 base_color = texture_point.rgb;

    // Phong
    vec3 light_dir = calculate_light_dir();

    vec3 ambient = calculate_ambient(0.3, base_color);
    vec3 diffuse = calculate_diffuse(1.0, light_dir);
    vec3 specular = calculate_specular(1.0, 64, light_dir);

    // Shadow
    if(u_receive_shadow) {
        float bias = max(0.15 * (1.0 - dot(normal, light_dir)), 0.15); 
        float shadow = calculate_shadow(bias);

        return (ambient + (1.0 - shadow) * (diffuse + specular)) * base_color; 
    } else {
        return (ambient + diffuse + specular) * base_color; 
    }
}

vec4 calculate_color() { 
    vec4 texture_point = vec4(1.0);

    if(u_display_texture) texture_point = texture(u_texture, tex_coord);
    
    return vec4(calculate_light(texture_point), texture_point.a); 
}

void main() {
    color = calculate_color();
}