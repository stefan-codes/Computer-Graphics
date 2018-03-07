#version 440

// The model transformation
uniform mat4 M;
// The transformation matrix
uniform mat4 MVP;
// The normal matrix
uniform mat3 N;
// The ambient intensity of the scene
uniform vec4 ambient_intensity;
// The light colour of the scene
uniform vec4 light_colour;
// Direction of the light
uniform vec3 light_dir;
// The diffuse reflection colour
uniform vec4 diffuse_reflection;
// The specular reflection colour
uniform vec4 specular_reflection;
// Shininess of the object
uniform float shininess;
// Position of the camera
uniform vec3 eye_pos;

// Incoming position
layout(location = 0) in vec3 position;
// Incoming normal
layout(location = 2) in vec3 normal;
// Outgoing vertex colour
layout(location = 0) out vec4 vertex_colour;

void main() {
  // Calculate position
		gl_Position = MVP * vec4(position, 1.0);
  // Calculate ambient component
		vec4 ambient_component = ambient_intensity*light_colour;



  // Transform the normal
		vec3 normal_transform = N*normal;
  // Calculate k
		float diffuse_k = max(dot(normal_transform, light_dir), 0.0);
  // Calculate diffuse
		vec4 diffuse_component = diffuse_k * (diffuse_reflection * light_colour);



  // Calculate world position of vertex
		vec4 world_position = M * vec4(position, 1.0f);
  // Calculate view direction
		vec3 view_direction = normalize(eye_pos - vec3(world_position));
  // Calculate half vector between view_dir and light_dir
		vec3 half_vector = normalize(light_dir + view_direction);
  // Calculate k
		float specular_k = pow(max(dot(normal_transform, half_vector), 0.0),shininess);
  // Calculate specular
		vec4 specular_component = specular_k * (specular_reflection * light_colour);



  // Output combined components
		vertex_colour = ambient_component + diffuse_component + specular_component;
  // *********************************
  // Ensure alpha is 1
  vertex_colour.a = 1.0;
}