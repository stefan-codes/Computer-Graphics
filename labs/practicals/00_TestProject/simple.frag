#version 440

// A directional light structure
struct directional_light {
  vec4 ambient_intensity;
  vec4 light_colour;
  vec3 light_dir;
};

// Point light information
struct point_light {
  vec4 light_colour;
  vec3 position;
  float constant;
  float linear;
  float quadratic;
};

// A material structure
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};

// Directional light 
uniform directional_light directionalLight;
uniform bool dlightOn;
// Point light
uniform point_light pointLight;
uniform bool plightOn;
// Material of the object
uniform material mat;
// Position of the camera
uniform vec3 eye_pos;
// Number of Textures
uniform sampler2D tex[2];
uniform sampler2D effects[1];
uniform sampler2D normalMap[1];
//uniform sampler2D specularMap[1];
uniform bool normal_map;
uniform bool specular_map;
uniform int meshIndex;
uniform float shift;

float nightIndex = 0.07f;

// Incoming position
layout(location = 0) in vec3 position;
// Incoming normal
layout(location = 1) in vec2 tex_coord;
// Incoming texture coordinate
layout(location = 2) in vec3 normal;
// Incoming binormal
layout(location = 3) in vec3 binormal;
// Incoming tangent
layout(location = 4) in vec3 tangent;
// Incoming light
//layout(location = 5) in vec4 light_position;

// Outgoing colour
layout(location = 0) out vec4 colour;

// Calculate a normal with normal map if present
vec3 calcNormal(in vec3 normal, in vec3 tangent, in vec3 binormal, in sampler2D normalMap, in vec2 tex_coord){
	vec3 sampleNormal = texture(normalMap, tex_coord).xyz;
	sampleNormal = 2.0f * sampleNormal - vec3(1.0f, 1.0f, 1.0f);
	mat3 TBN = mat3(tangent, binormal, normal);
	vec3 newNormal = normalize(TBN * sampleNormal);
	return newNormal;
}

// Calculate the colour from the directional light
vec4 directional_light_colour(in directional_light light, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir, in vec4 tex_sample){
	vec4 result;
	vec4 diffuse;
	vec4 specular;

	// Calculate ambient
	vec4 ambient = mat.diffuse_reflection * light.light_colour;
	
	// Calculate diffuse
	float diffuseK = max(dot(normal, light.light_dir), 0.0f);
	// If earth - light polution
	if(meshIndex == 2 && diffuseK<nightIndex){
		diffuseK = nightIndex;
	}
	diffuse = diffuseK * (mat.diffuse_reflection * light.light_colour);

	// Calculate specular
	if (specular_map) {
		// dont have it yet
	} else {
		// Default specular
		vec3 half_vector = normalize(light.light_dir + view_dir);
		float specularK = pow(max(dot(normal, half_vector), 0.0f), mat.shininess);
		specular = specularK * (mat.specular_reflection * light.light_colour);
	}

	// The colour
	vec4 primary = mat.emissive + diffuse;
	result = (primary*tex_sample) + specular;
	result.a = 1.0f;

	return result;	
}

// Calculate the colour from the point light
vec4 point_light_colour(in point_light light, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir, in vec4 tex_sample) {
	vec4 result;
	vec4 diffuse;
	vec4 specular;

	// get the distance from the light
	float d = distance(light.position, position);
	// get the light colour / attenuation
	vec4 light_colour = light.light_colour / (light.constant + light.linear * d + light.quadratic * pow(d,2));
	// get the light direction
	vec3 light_dir = normalize(light.position - position);

	// Diffuse
	float diffuseK = max(dot(normal, light_dir), 0.0f);
	// Check if we are at "night"
	if(meshIndex == 2 && diffuseK<nightIndex){
		diffuseK = nightIndex;
	}
	diffuse = diffuseK * (mat.diffuse_reflection * light_colour);

	// Specular
	if (specular_map) {
		// dont have yet
	} else {
		// Default specular
		vec3 half_vector = normalize(light_dir + view_dir);
		float specularK = pow(max(dot(normal, half_vector), 0.0f), mat.shininess);
		specular = specularK * (mat.specular_reflection * light_colour);
	}

	// The colour
	vec4 primary = mat.emissive + diffuse;
	result = (primary * tex_sample) + specular;
	result.a = 1.0f;

	return result;
}

void main() {
	// Calculate view direction
	vec3 view_direction = normalize(eye_pos - position);
	// Pass the normal
	vec3 updatedNormal = normal;
	// Sample the texture
	vec4 textureSample;

	// If we have a map, use the map sample
	if (normal_map) {
		updatedNormal = calcNormal(normal, tangent, binormal, normalMap[0], tex_coord);
	}

	switch (meshIndex) {
		case 0:
		// Stars
			textureSample = texture(tex[0], tex_coord + shift);
			break;
		case 1:
		// Sun
			textureSample = texture(tex[0], tex_coord);
			textureSample += texture(effects[0], tex_coord + shift);
			break;
		case 2:
		// Earth
			vec3 light_dir = normalize(pointLight.position - position);
			float n = max(dot(updatedNormal, light_dir), 0.0f);
			if(n<nightIndex){
				textureSample = texture(tex[1], tex_coord);
			} else {
				textureSample = texture(tex[0], tex_coord);
			}
			textureSample += texture(effects[0], tex_coord + shift);
			break;
		case 3:
		// Moon
			textureSample = texture(tex[0], tex_coord);
			break;
		default:
			textureSample = vec4(0.7f,0.7f,0.7f,1.0f);
	}

	// Calculate the actual colour
	if (dlightOn) {
		colour += directional_light_colour(directionalLight, mat, position, updatedNormal, view_direction, textureSample);
	}
	if (plightOn) {
		colour += point_light_colour(pointLight, mat, position, updatedNormal, view_direction, textureSample);
	}
}