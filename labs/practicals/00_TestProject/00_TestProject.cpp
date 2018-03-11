#include "graphics_framework.h"
#include <glm/glm.hpp>
//#include <glm/gtx/rotate_vector.hpp>

using namespace std;
using namespace graphics_framework;
using namespace glm;

// Textures and Meshes
map<string, mesh> meshes;
map<string, texture> textures;

// Effects
effect eff;
effect shadow_eff;

//shadow_map shadow;

// Cameras
chase_camera chaseCamera;
free_camera freeCamera;
bool isFreeCamera = true;
float cameraSpeed = 0.02;

// Lights
bool dlightOn = false;
directional_light directionalLight;
bool plightOn = true;
point_light pointLight;
float sunStrength = 80.0f;
  
// for change of clouds  
float starShift;
float starShiftIndex = 0.00001;
float sunShift;
float sunShiftIndex = 0.0001;
float cloudShift;
float cloudShiftIndex = 0.00001;

// cursor vars
double cursor_x = 0.0;
double cursor_y = 0.0;

bool initialise() {
	// Set input mode - hide the cursor
	glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// Capture initial mouse position
	glfwGetCursorPos(renderer::get_window(), &cursor_x, &cursor_y);
	return true;
}

bool load_content() {
	// Create shadow map with screen size
	//shadow = shadow_map(renderer::get_screen_width(), renderer::get_screen_height());

	// Create My meshes
	meshes["stars"] = mesh(geometry_builder::create_box());
	meshes["sun"] = mesh(geometry_builder::create_sphere(40, 40));
	meshes["earth"] = mesh(geometry_builder::create_sphere(40, 40));
	meshes["moon"] = mesh(geometry_builder::create_sphere(40, 40));
	//meshes["sword"] = mesh(geometry("sword/sword.obj"));

	// Set materials
	meshes["stars"].get_material().set_shininess(1.0f);
	meshes["stars"].get_material().set_specular(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	meshes["stars"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	meshes["stars"].get_material().set_diffuse(vec4(1.0f, 1.0f, 1.0f, 1.0f));

	meshes["sun"].get_material().set_shininess(1.0f);
	meshes["sun"].get_material().set_specular(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	meshes["sun"].get_material().set_emissive(vec4(1.0f, 0.5f, 0.0f, 1.0f));
	meshes["sun"].get_material().set_diffuse(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	  
	meshes["earth"].get_material().set_shininess(1.0f);
	meshes["earth"].get_material().set_specular(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	meshes["earth"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	meshes["earth"].get_material().set_diffuse(vec4(1.0f, 1.0f, 1.0f, 1.0f));

	meshes["moon"].get_material().set_shininess(1.5f);
	meshes["moon"].get_material().set_specular(vec4(0.05f, 0.05f, 0.05f, 1.0f));
	meshes["moon"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	meshes["moon"].get_material().set_diffuse(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	   
	// Transform meshes
	meshes["stars"].get_transform().scale = vec3(-100.0f, -100.0f, -100.0f);
	meshes["stars"].get_transform().rotate(vec3(0.0f, 0.0f, 0.0f));
	meshes["stars"].get_transform().translate(vec3(0.0f, 0.0f, 0.0f));

	meshes["sun"].get_transform().scale = vec3(1.0f, 1.0f, 1.0f);
	meshes["sun"].get_transform().rotate(vec3(-half_pi<float>(), 0.0f, 0.0f));
	meshes["sun"].get_transform().translate(vec3(0.0f, 0.0f, 0.0f));

	meshes["earth"].get_transform().scale = vec3(0.3f, 0.3f, 0.3f); //relative to the sun
	meshes["earth"].get_transform().rotate(vec3(-half_pi<float>(), 0.0f, 0.0f));
	meshes["earth"].get_transform().rotate(vec3(0.13f*pi<float>(), 0.0f, 0.0f)); //after -half_pi from sun 23.4 tilt
	meshes["earth"].get_transform().translate(vec3(10.0f, 0.0f, 0.0f)); //from the sun

	meshes["moon"].get_transform().scale = vec3(0.1f, 0.1f, 0.1f); //relative to the earth
	meshes["moon"].get_transform().rotate(vec3(-half_pi<float>(), 0.0f, 0.0f)); // from sun and earth pos
	meshes["moon"].get_transform().rotate(vec3(0.0f, 0.0f, 0.0f));
	//meshes["moon"].get_transform().translate(vec3(2.0f, 0.0f, 0.0f)); // from the earth
	meshes["moon"].get_transform().translate(vec3(8.0f, 0.0f, 0.0f));

	//meshes["sword"].get_transform().translate(vec3(-10.0f,0.0f,0.0f));
	//meshes["sword"].get_transform().rotate(vec3(pi<float>(), 0.0f, 0.0f));

	// Load textures
	textures["stars"] = texture("textures/2k_stars.jpg");
	textures["sun"] = texture("textures/2k_sun.jpg");
	textures["earthDay"] = texture("textures/2k_earth_daymap.jpg");
	textures["earthNight"] = texture("textures/2k_earth_nightmap.jpg");
	textures["earthClouds"] = texture("textures/2k_earth_clouds.jpg");
	textures["earthNormal"] = texture("textures/2k_earth_normal_map.jpg");
	textures["moon"] = texture("textures/2k_moon.jpg");
	//textures["sword"] = texture("sword/SapphireSwordDiffuse.png");
	  
	 
	// Load Lights
	// ambient intensity (0.3, 0.3, 0.3), colour white, direction 1,1,1
	directionalLight.set_ambient_intensity(vec4(0.3f, 0.3f, 0.3f, 1.0f));
	directionalLight.set_light_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	directionalLight.set_direction(vec3(1.0f, 1.0f, -1.0f));
	   
	// Point Light Position, Colour, Range
	pointLight.set_position(vec3(0.0f, 0.0f, 0.0f));
	pointLight.set_light_colour(vec4(1.0f,1.0f,1.0f,1.0f));
	pointLight.set_range(sunStrength);
	 
	// Load in shaders
	eff.add_shader("00_TestProject/simple.vert", GL_VERTEX_SHADER);
	eff.add_shader("00_TestProject/simple.frag", GL_FRAGMENT_SHADER);

	/*shadow_eff.add_shader("00_TestProject/simple.vert", GL_VERTEX_SHADER);
	shadow_eff.add_shader("00_TestProject/simple.frag", GL_FRAGMENT_SHADER);*/

	// Build effects
	eff.build();
	//shadow_eff.build();

	// Load free camera
	freeCamera.set_position(vec3(4.0f, 0.0f, 10.0f));
	freeCamera.set_target(vec3(0.0f, 0.0f, 0.0f));
	freeCamera.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);

	// Load chase camera
	chaseCamera.set_pos_offset(vec3(0.0f, 0.50f, 3.0f));
	chaseCamera.set_springiness(0.5f);
	chaseCamera.move(meshes["earth"].get_transform().position, eulerAngles(meshes["earth"].get_transform().orientation));
	chaseCamera.rotate(vec3(half_pi<float>(),0.0f,0.0f));
	chaseCamera.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);

	return true;
} 

bool update(float delta_time) {
	// The ratio of pixels to rotation - remember the fov
	static const float sh = static_cast<float>(renderer::get_screen_height());
	static const float sw = static_cast<float>(renderer::get_screen_width());
	static const double ratio_width = quarter_pi<float>() / sw;
	static const double ratio_height = (quarter_pi<float>() * (sh / sw)) / sh;

	double current_x;
	double current_y;
	// Get the current cursor position
	glfwGetCursorPos(renderer::get_window(), &current_x, &current_y);
	// Calculate delta of cursor positions from last frame
	double delta_x = current_x - cursor_x;
	double delta_y = current_y - cursor_y;

	if (isFreeCamera) {
		// Multiply deltas by ratios - gets actual change in orientation
		delta_x = delta_x * ratio_width;
		delta_y = delta_y * ratio_height;
		// Rotate cameras by delta
		freeCamera.rotate(delta_x, -delta_y);
		// Controls
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_W)) {
			freeCamera.move(vec3(delta_x, -delta_y, cameraSpeed));
		}

		if (glfwGetKey(renderer::get_window(), GLFW_KEY_S)) {
			freeCamera.move(vec3(delta_x, -delta_y, -cameraSpeed));
		}

		if (glfwGetKey(renderer::get_window(), GLFW_KEY_A)) {
			freeCamera.move(vec3(delta_x, -delta_y, 0) + vec3(-cameraSpeed, 0, 0));
		}

		if (glfwGetKey(renderer::get_window(), GLFW_KEY_D)) {
			freeCamera.move(vec3(delta_x, -delta_y, 0) + vec3(cameraSpeed, 0, 0));
		}

		if (glfwGetKey(renderer::get_window(), GLFW_KEY_SPACE)) {
			freeCamera.move(vec3(freeCamera.get_up()) * vec3(cameraSpeed));
		}

		if (glfwGetKey(renderer::get_window(), GLFW_KEY_LEFT_SHIFT)) {
			freeCamera.move(vec3(freeCamera.get_up()) * vec3(-cameraSpeed));
		}
		//Update the camera
		freeCamera.update(delta_time);
	}
	else {
		// get the target
		static mesh &target_mesh = meshes["earth"];
		// Multiply deltas by ratios and delta_time - gets actual change in orientation
		delta_x = delta_x * ratio_width * 5 * delta_time;
		delta_y = delta_y * ratio_height * 5 * delta_time;
		// Rotate cameras by delta
		// x - delta_y
		// y - delta_x
		// z - 0
		chaseCamera.rotate(vec3(delta_y, delta_x, 0)*5.0f);
		// Move camera - update target position and rotation
		chaseCamera.move(target_mesh.get_transform().position, eulerAngles(target_mesh.get_transform().orientation));
		// Update the camera
		chaseCamera.update(delta_time);
	}

	// update cursor
	cursor_x = current_x;
	cursor_y = current_y;

	// 1 - Free camera
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_1)) {
		isFreeCamera = true;
	}
	// 2 - Chase Camera
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_2)) {
		isFreeCamera = false;
	}
	// P - point light OFF
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_P)) {
		plightOn = false;
	}
	// O - point light ON
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_O)) {
		plightOn = true;
	}
	// 0 - direction light OFF
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_0)) {
		dlightOn = false;
	}
	// 0 - direction light OFF
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_9)) {
		dlightOn = true;
	}
	// Move earth
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_UP)) {
		meshes["earth"].get_transform().translate(vec3(0.0f, 0.5f * delta_time, 0.0f));
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_DOWN)) {
		meshes["earth"].get_transform().translate(vec3(0.0f, -0.5f * delta_time, 0.0f));
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_LEFT)) {
		meshes["earth"].get_transform().translate(vec3(-0.5f * delta_time, 0.0f, 0.0f));
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_RIGHT)) {
		meshes["earth"].get_transform().translate(vec3(0.5f * delta_time, 0.0f, 0.0f));
	}

	// Rotations
	meshes["sun"].get_transform().rotate(vec3(0.0f, 0.0f, half_pi<float>() * delta_time / 5.0f));
	meshes["earth"].get_transform().rotate(vec3(0.0f, 0.0f, pi<float>() * delta_time / 5.0f));
	meshes["moon"].get_transform().rotate(vec3(0.0f, 0.0f, pi<float>() * delta_time / 3.0f));
	
	// Update the shadow map
	//shadow.light_position = pointLight.get_position();
	// will set the direction in the shader

	cout << "FPS: " << 1.0f / delta_time << endl;
	return true;
}
 
bool render() {
	//// Set render target to shadow map
	//renderer::set_render_target(shadow);
	//// Clear depth buffer
	//glClear(GL_DEPTH_BUFFER_BIT);
	//// Set face cull mode to front
	//glCullFace(GL_FRONT);

	//// Light projection Matrix with fov 90
	//mat4 lightP = perspective<float>(90.0f, renderer::get_screen_aspect(), 0.1f, 1000.0f);
	//// bind shader
	//renderer::bind(shadow_eff);
	//// Render meshes for shadows
	//for (auto &e : meshes) {
	//	auto m = e.second;
	//	// Create MVP matrix
	//	auto M = m.get_transform().get_transform_matrix();
	//	// View matrix taken from shadow map
	//	auto V = shadow.get_view();
	//	auto MVP = lightP * V * M;
	//	// Set MVP matrix uniform
	//	glUniformMatrix4fv(shadow_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
	//	renderer::render(m);
	//}

	//// Set render target back to the screen
	//renderer::set_render_target();
	//// Set face cull mode to back
	//glCullFace(GL_BACK);

	// Bind effect
	renderer::bind(eff);

	// Render meshes
	for (auto &e : meshes) {
		auto m = e.second;
		// Get the mesh's M matrix
		auto M = m.get_transform().get_transform_matrix();
		// Get VP depending on the camera
		mat4 V;
		mat4 P;
		if (isFreeCamera) {
			V = freeCamera.get_view();
			P = freeCamera.get_projection();
		}
		else {
			V = chaseCamera.get_view();
			P = chaseCamera.get_projection();
		}
		 
		// Special conditions
		if (e.first == "stars") {
			// Set an mesh index
			glUniform1i(eff.get_uniform_location("meshIndex"), 0);
			// Are we using some maps?
			glUniform1i(eff.get_uniform_location("specular_map"), false);
			glUniform1i(eff.get_uniform_location("normal_map"), false);
			// Textures
			renderer::bind(textures["stars"], 0);
			glUniform1i(eff.get_uniform_location("tex[0]"), 0);
			// Motion
			glUniform1f(eff.get_uniform_location("shift"), starShift);
			starShift += starShiftIndex;
		}

		if (e.first == "sun") {
			// Set an mesh index
			glUniform1i(eff.get_uniform_location("meshIndex"), 1);
			// Are we using some maps?
			glUniform1i(eff.get_uniform_location("specular_map"), false);
			glUniform1i(eff.get_uniform_location("normal_map"), false);
			// Textures
			renderer::bind(textures["sun"], 0);
			glUniform1i(eff.get_uniform_location("tex[0]"), 0);
			// Motion
			renderer::bind(textures["earthClouds"], 1);
			glUniform1i(eff.get_uniform_location("effects[0]"), 1);
			glUniform1f(eff.get_uniform_location("shift"), sunShift);
			sunShift += sunShiftIndex;
		} 
		    
		if (e.first == "earth") {
			// Set an mesh index
			glUniform1i(eff.get_uniform_location("meshIndex"), 2);
			// Are we using some maps?
			glUniform1i(eff.get_uniform_location("specular_map"), false);
			glUniform1i(eff.get_uniform_location("normal_map"), true);
			renderer::bind(textures["earthNormal"], 3);
			glUniform1i(eff.get_uniform_location("normalMap[0]"), 3);
			// Update the M from the hierarchy
			//auto sunM = meshes["sun"].get_transform().get_transform_matrix();
			//M =  sunM * M;
			// Textures
			renderer::bind(textures["earthDay"], 0);
			renderer::bind(textures["earthNight"], 1);
			glUniform1i(eff.get_uniform_location("tex[0]"), 0);
			glUniform1i(eff.get_uniform_location("tex[1]"), 1);
			// Motion
			renderer::bind(textures["earthClouds"], 2);
			glUniform1i(eff.get_uniform_location("effects[0]"), 2);
			glUniform1f(eff.get_uniform_location("shift"), cloudShift);
			cloudShift += cloudShiftIndex;
		}

		if (e.first == "moon") {
			// Set an mesh index
			glUniform1i(eff.get_uniform_location("meshIndex"), 3);
			// Are we using some maps?
			glUniform1i(eff.get_uniform_location("specular_map"), false);
			glUniform1i(eff.get_uniform_location("normal_map"), false);
			// Update the M from the hierarchy
			//auto sunM = meshes["sun"].get_transform().get_transform_matrix();
			//auto earthM = meshes["earth"].get_transform().get_transform_matrix();
			//M = sunM * earthM * M;
			// Textures
			renderer::bind(textures["moon"], 0);
			glUniform1i(eff.get_uniform_location("tex[0]"), 0);
			// Motions - none for now
		}
		   
		// Populate the MVP
		auto MVP = P * V * M;
		// Set MVP matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
		// Set M matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
		// Set N matrix uniform - remember - 3x3 matrix
		auto N = m.get_transform().get_normal_matrix();
		N = transpose(inverse(N));
		glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));

		//// Light model matrix for the mesh
		//auto lightM = m.get_transform().get_transform_matrix();
		//// Light view matrix from the shadow map
		//auto lightV = shadow.get_view();
		//// Light MVP
		//auto lightMVP = lightP * lightV*lightM;
		//// Set uniform
		//glUniformMatrix4fv(eff.get_uniform_location("lightMVP"), 1, GL_FALSE, value_ptr(lightMVP));


		// Bind material
		renderer::bind(m.get_material(), "mat");
		// Bind lights
		renderer::bind(directionalLight, "directionalLight");
		renderer::bind(pointLight, "pointLight");
		    
		if (dlightOn) {
			glUniform1i(eff.get_uniform_location("dlightOn"), 1);
		}
		else {
			glUniform1i(eff.get_uniform_location("dlightOn"), 0);
		}
		    
		if (plightOn) {
			glUniform1i(eff.get_uniform_location("plightOn"), 1);
		}
		else {
			glUniform1i(eff.get_uniform_location("plightOn"), 0);
		}
		  
		// Set eye position - Get this from active camera
		if (isFreeCamera) {
			glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(freeCamera.get_position()));
		}
		else {
			glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(chaseCamera.get_position()));
		}

		//// Bind shadow map texture - use texture unit 1
		//renderer::bind(shadow.buffer->get_depth(), 1);
		//// Set the shadow_map uniform
		//glUniform1i(eff.get_uniform_location("shadowMap"), 1);
		
		// Render mesh
		renderer::render(m);
	}

	return true;
}

int main() {
	// Create application
	app application("Coursework");
	// Set load content, update and render methods
	application.set_load_content(load_content);
	application.set_initialise(initialise);
	application.set_update(update);
	application.set_render(render);
	// Run application
	application.run();
}