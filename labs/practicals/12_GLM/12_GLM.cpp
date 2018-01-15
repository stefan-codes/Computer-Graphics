#define GLM_ENABLE_EXPERIMENTAL
#include <glm\glm.hpp>
#include <glm\gtc\constants.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtx\euler_angles.hpp>
#include <glm\gtx\projection.hpp>
#include <iostream>

using namespace std;
using namespace glm;

// 2d vectores
vec2 a2 = vec2(1.0, 0.0);
vec2 b2 = vec2(0, 1);
// 3d vectors
vec3 a3 = vec3(1, 0, 0);
vec3 b3 = vec3(0, 0, 1);
// 4d vectors
vec4 a4 = vec4(1, 0, 0, 1);
vec4 b4 = vec4(0, 0, 1, 1);

// 2d vectors
vec2 c2 = vec2(1.0, 0.0);
vec2 d2 = vec2(0, 1);
// 3d vectors
vec3 c3 = vec3(1, 0, 0);
vec3 d3 = vec3(0, 0, 1);
// 4d vectors
vec4 c4 = vec4(1, 0, 0, 1);
vec4 d4 = vec4(0, 0, 1, 1);

int main() {
	//Convert c2 to a 3d vector
	vec3 c2(c2, 0);

	//Convert d2 to a 4d vector
	vec4 d2(d2, 0, 1);

	//Convert c3 to a 2d vector
	vec2 c3(c3);

	//Convert d3 to a 4d vector
	vec4 d3(d3, 1);

	//Convert c4 to a 2d vector
	vec2 c4(c4);

	//Convert d4 to a 3d vector
	vec3 d4(d4);

	std::cout << c2.x << std::endl;
}