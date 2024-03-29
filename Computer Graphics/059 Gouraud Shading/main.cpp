#include <graphics_framework.h>
#include <glm\glm.hpp>

using namespace std;
using namespace graphics_framework;
using namespace glm;

map<string, mesh> meshes;
map<string, material> materials;  // set dictionary of structs
effect eff;
texture tex;
free_camera cam;
directional_light light;


// initialise params
GLFWwindow* window;
double xpos = 0;
double ypos = 0;

double current_x = 0;
double current_y = 0;

double new_x = 0;
double new_y = 0;

bool firstMouse = true;

bool initialise()
{
	// ********************************
	// Set input mode - hide the cursor
	// ********************************
	window = renderer::get_window();

	if (CHECK_GL_ERROR)
	{
		__debugbreak();
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// ******************************
	// Capture initial mouse position
	// ******************************
	glfwGetCursorPos(window, &xpos, &ypos);

	if (CHECK_GL_ERROR)
	{
		__debugbreak();
	}

	return true;
}



bool load_content()
{
	// Create plane mesh
	meshes["plane"] = mesh(geometry_builder::create_plane());

	// Create scene
	meshes["box"] = mesh(geometry_builder::create_box());
	meshes["tetra"] = mesh(geometry_builder::create_tetrahedron());
	meshes["pyramid"] = mesh(geometry_builder::create_pyramid());
	meshes["disk"] = mesh(geometry_builder::create_disk(20));
	meshes["cylinder"] = mesh(geometry_builder::create_cylinder(20, 20));
	meshes["sphere"] = mesh(geometry_builder::create_sphere(20, 20));
	meshes["torus"] = mesh(geometry_builder::create_torus(20, 20, 1.0f, 5.0f));

	// Transform objects
	meshes["box"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
	meshes["box"].get_transform().translate(vec3(-10.0f, 2.5f, -30.0f));
	meshes["tetra"].get_transform().scale = vec3(4.0f, 4.0f, 4.0f);
	meshes["tetra"].get_transform().translate(vec3(-30.0f, 10.0f, -10.0f));
	meshes["pyramid"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
	meshes["pyramid"].get_transform().translate(vec3(-10.0f, 7.5f, -30.0f));
	meshes["disk"].get_transform().scale = vec3(3.0f, 1.0f, 3.0f);
	meshes["disk"].get_transform().translate(vec3(-10.0f, 11.5f, -30.0f));
	meshes["disk"].get_transform().rotate(vec3(half_pi<float>(), 0.0f, 0.0f));
	meshes["cylinder"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
	meshes["cylinder"].get_transform().translate(vec3(-25.0f, 2.5f, -25.0f));
	meshes["sphere"].get_transform().scale = vec3(2.5f, 2.5f, 2.5f);
	meshes["sphere"].get_transform().translate(vec3(-25.0f, 10.0f, -25.0f));
	meshes["torus"].get_transform().translate(vec3(-25.0f, 10.0f, -25.0f));
	meshes["torus"].get_transform().rotate(vec3(half_pi<float>(), 0.0f, 0.0f));

	// ***********************
	// Set materials
	// - all emissive is black
	// - all specular is white
	// - all shininess is 25
	// ***********************	
	
	//map<string, material>::iterator it = materials.begin(); it!;
	for (auto &m : materials)
	{
		m.second.set_shininess(50);
		m.second.set_specular(vec4(1.0, 1.0, 1.0, 1.0));
		m.second.set_emissive(vec4(0.0, 0.0, 0.0, 1.0));
	}


	// Red box
	materials["box"].set_diffuse(vec4(1.0, 0.0, 0.0, 1.0));

	meshes["box"].set_material(materials["box"]);


	// Green tetra
	materials["tetra"].set_diffuse(vec4(0.0, 1.0, 0.0, 1.0));
	
	meshes["tetra"].set_material( materials["tetra"]);

	// Blue pyramid
	materials["pyramid"].set_diffuse(vec4(0.0, 0.0, 1.0, 1.0));
	
	meshes["pyramid"].set_material(materials["pyramid"]);
	// Yellow disk
	materials["disk"].set_diffuse(vec4(1.0, 1.0, 0.0, 1.0));
	
	meshes["disk"].set_material(materials["disk"]);
	// Magenta cylinder
	materials["cylinder"].set_diffuse(vec4(0.0, 1.0, 1.0, 1.0));
	
	meshes["cylinder"].set_material(materials["cylinder"]);
	// Cyan sphere
	materials["sphere"].set_diffuse(vec4(1.0, 0.0, 1.0, 1.0));

	meshes["sphere"].set_material(materials["sphere"]);

	// White torus
	materials["torus"].set_diffuse(vec4(1.0, 1.0, 1.0, 1.0));

	meshes["torus"].set_material(materials["torus"]);

	// create light source
	meshes["light"] = mesh(geometry_builder::create_sphere(20, 20));

	meshes["light"].get_transform().position = vec3(1.0f, 1.0f, -1.0f);

	// **************************
	// Load texture - checked.gif
	// **************************
	tex = texture("..\\resources\\textures\\checked.gif");


	// *******************
	// Set lighting values
	// *******************
	// ambient intensity (0.3, 0.3, 0.3)
	light.set_ambient_intensity(vec4(0.3, 0.3, 0.3, 1.0));
	
	// Light colour white
	light.set_light_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	
	// Light direction (1.0, 1.0, -1.0)
	light.set_direction(vec3(1.0f, 1.0f, -1.0f));


	// Load in shaders
	eff.add_shader("..\\resources\\shaders\\gouraud.vert", GL_VERTEX_SHADER);
	eff.add_shader("..\\resources\\shaders\\gouraud.frag", GL_FRAGMENT_SHADER);
	// Build effect
	eff.build();

	if (CHECK_GL_ERROR)
	{
		__debugbreak();
	}

	// Set camera properties
	cam.set_position(vec3(50.0f, 10.0f, 50.0f));
	cam.set_target(vec3(0.0f, 0.0f, 0.0f));
	auto aspect = static_cast<float>(renderer::get_screen_width()) / static_cast<float>(renderer::get_screen_height());
	cam.set_projection(quarter_pi<float>(), aspect, 2.414f, 1000.0f);

	if (CHECK_GL_ERROR)
	{
		__debugbreak();
	}
	return true;
}

bool update(float delta_time)
{
	// The ratio of pixels to rotation - remember the fov
	static double ratio_width = quarter_pi<float>() / static_cast<float>(renderer::get_screen_width());
	static double ratio_height = (quarter_pi<float>() * (static_cast<float>(renderer::get_screen_height()) / static_cast<float>(renderer::get_screen_width()))) / static_cast<float>(renderer::get_screen_height());

	// *******************************
	// Get the current cursor position
	// *******************************
	glfwGetCursorPos(window, &new_x, &new_y);


	// ***************************************************
	// Calculate delta of cursor positions from last frame
	// ***************************************************
	if (firstMouse)
	{
		current_x = xpos;
		current_y = ypos;
		firstMouse = false;
	}

	double delta_x = new_x - current_x;
	double delta_y = new_y - current_y;

	// *************************************************************
	// Multiply deltas by ratios - gets actual change in orientation
	// *************************************************************
	delta_x *= ratio_width;
	delta_y *= -ratio_height;

	
	// *************************
	// Rotate cameras by delta
	// delta_y - x-axis rotation
	// delta_x - y-axis rotation
	// *************************
	cam.rotate((float)delta_x, (float)delta_y);

	
	// *******************************
	// Use keyboard to move the camera
	// - WSAD
	// *******************************
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_W))
		cam.move(vec3(0.0f, 0.0f, 1.0f));
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_A))
		cam.move(vec3(-1.0f, 0.0f, 0.0f));
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_D))
		cam.move(vec3(1.0f, 0.0f, 0.0f));
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_S))
		cam.move(vec3(0.0f, 0.0f, -1.0f));

	
	// ***********
	// Move camera
	// ***********

	// *****************
	// Update the camera
	// *****************
	cam.update(delta_time);


	// *****************
	// Update cursor pos
	// *****************
	glfwGetCursorPos(window, &current_x, &current_y);



	return true;

}

bool render()
{
	// Render meshes
	for (auto &e : meshes)
	{
		auto m = e.second;
		
		if (CHECK_GL_ERROR)
		{
			__debugbreak();
		}

		// Bind effect
		renderer::bind(eff);

		if (CHECK_GL_ERROR)
		{
			__debugbreak();
		}

		// Create MVP matrix
		auto M = m.get_transform().get_transform_matrix();
		auto V = cam.get_view();
		auto P = cam.get_projection();
		auto MVP = P * V * M;
		// Set MVP matrix uniform
		glUniformMatrix4fv(
			eff.get_uniform_location("MVP"), // Location of uniform
			1, // Number of values - 1 mat4
			GL_FALSE, // Transpose the matrix?
			value_ptr(MVP)); // Pointer to matrix data

		// ********************
		// Set M matrix uniform
		// ********************
		glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));


		// ***********************
		// Set N matrix uniform
		// - remember - 3x3 matrix
		// ***********************
		mat3 N = mat3(M);
		N = transpose(N);
		glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));


		// *************
		// Bind material
		// *************
		material mm = m.get_material();

		renderer::bind(mm, "mat");
	
		

		// **********
		// Bind light
		// **********
		//glUniform4fv(eff.get_uniform_location("light.ambient_intensity"), 1, value_ptr(light.get_ambient_intensity));
		renderer::bind(light, "light");
		

		// ************
		// Bind texture
		// ************
		renderer::bind(tex, 0);
		

		// ***************
		// Set tex uniform
		// ***************
		glUniform1i(eff.get_uniform_location("tex"), 0);

		// *****************************
		// Set eye position
		// - Get this from active camera
		// *****************************
		vec3 eyeP = cam.get_position();

		glUniform3f(eff.get_uniform_location("eye_pos"), eyeP.x, eyeP.y, eyeP.z);

		


		// Render mesh
		renderer::render(m);
	}

	return true;
}

void main()
{
	// Create application
	app application;
	// Set load content, update and render methods
	application.set_load_content(load_content);
	application.set_initialise(initialise);
	application.set_update(update);
	application.set_render(render);
	// Run application
	application.run();
}