#version 330

uniform sampler2D u_texture;
uniform sampler2D u_shadow;
uniform vec3 u_light_pos;
uniform vec3 u_light_dir;
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

void main() {
    vec4 texture_point = texture(u_texture, tex_coord);

    vec3 base_color = texture_point.rgb;

    vec3 ambient = 0.15 * base_color;

    vec3 light_dir = normalize(u_light_dir);
    float diff = max(dot(light_dir, normal), 0.0);
    vec3 diffuse = diff * u_light_color;

    vec3 view_dir = normalize(u_camera_pos - frag_pos.xyz);
    float spec = 0.0;
    vec3 halfway_dir = normalize(light_dir + view_dir);  
    spec = pow(max(dot(normal, halfway_dir), 0.0), 64.0);
    vec3 specular = spec * u_light_color;    

    float bias = max(0.05 * (1.0 - dot(normal, light_dir)), 0.005); 
    float shadow = calculate_shadow(frag_pos_light_space, bias);  
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * base_color;    
    
    color = vec4(lighting, texture_point.a);
}