#include <graphics_framework.h>
#include <glm\glm.hpp>

using namespace std;
using namespace graphics_framework;
using namespace glm;

map<string, mesh> meshes;
effect eff;
effect tex_eff;
texture tex;
target_camera cam;
directional_light light;
frame_buffer frame;
geometry screen_quad;

bool load_content()
{
	// Create frame buffer - use screen width and height
	frame = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());

	// Create screen quad
	// - positions
	// - tex coords
	vector<vec3> positions
	{
		vec3(1.0f, 1.0f, 0.0f),
		vec3(-1.0f, 1.0f, 0.0f),
		vec3(-1.0f, -1.0f, 0.0f),
		vec3(1.0f, -1.0f, 0.0f)
	};
	vector<vec2> tex_coords
	{
		vec2(1.0f, 1.0f),
		vec2(0.0f, 1.0f),
		vec2(0.0f, 0.0f),
		vec2(1.0f, 0.0f)
	};
	screen_quad.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
	screen_quad.add_buffer(tex_coords, BUFFER_INDEXES::TEXTURE_COORDS_0);
	screen_quad.set_type(GL_QUADS);

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

    // Set materials
    // Red box
    meshes["box"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
    meshes["box"].get_material().set_diffuse(vec4(1.0f, 0.0f, 0.0f, 1.0f));
    meshes["box"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
    meshes["box"].get_material().set_shininess(25.0f);
    // Green tetra
    meshes["tetra"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
    meshes["tetra"].get_material().set_diffuse(vec4(0.0f, 1.0f, 0.0f, 1.0f));
    meshes["tetra"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
    meshes["tetra"].get_material().set_shininess(25.0f);
    // Blue pyramid
    meshes["pyramid"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
    meshes["pyramid"].get_material().set_diffuse(vec4(0.0f, 0.0f, 1.0f, 1.0f));
    meshes["pyramid"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
    meshes["pyramid"].get_material().set_shininess(25.0f);
    // Yellow disk
    meshes["disk"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
    meshes["disk"].get_material().set_diffuse(vec4(1.0f, 1.0f, 0.0f, 1.0f));
    meshes["disk"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
    meshes["disk"].get_material().set_shininess(25.0f);
    // Magenta cylinder
    meshes["cylinder"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
    meshes["cylinder"].get_material().set_diffuse(vec4(1.0f, 0.0f, 1.0f, 1.0f));
    meshes["cylinder"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
    meshes["cylinder"].get_material().set_shininess(25.0f);
    // Cyan sphere
    meshes["sphere"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
    meshes["sphere"].get_material().set_diffuse(vec4(0.0f, 1.0f, 1.0f, 1.0f));
    meshes["sphere"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
    meshes["sphere"].get_material().set_shininess(25.0f);
    // White torus
    meshes["torus"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
    meshes["torus"].get_material().set_diffuse(vec4(1.0f, 1.0f, 1.0f, 1.0f));
    meshes["torus"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
    meshes["torus"].get_material().set_shininess(25.0f);

    // Load texture
    tex = texture("..\\resources\\textures\\checked.gif");

    // Set lighting values
    light.set_ambient_intensity(vec4(0.3f, 0.3f, 0.3f, 1.0f));
    light.set_light_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
    light.set_direction(vec3(1.0f, 1.0f, -1.0f));

    // Load in shaders
    eff.add_shader("..\\resources\\shaders\\phong.vert", GL_VERTEX_SHADER);
    eff.add_shader("..\\resources\\shaders\\phong.frag", GL_FRAGMENT_SHADER);
    
    // ***************************************
    // Change the fragment shader to greyscale
    // ***************************************
    tex_eff.add_shader("..\\resources\\shaders\\simple_texture.vert", GL_VERTEX_SHADER);
    tex_eff.add_shader("..\\resources\\shaders\\greyscale.frag", GL_FRAGMENT_SHADER);
    // Build effects
    eff.build();
    tex_eff.build();

    // Set camera properties
    cam.set_position(vec3(50.0f, 10.0f, 50.0f));
    cam.set_target(vec3(0.0f, 0.0f, 0.0f));
    auto aspect = static_cast<float>(renderer::get_screen_width()) / static_cast<float>(renderer::get_screen_height());
    cam.set_projection(quarter_pi<float>(), aspect, 2.414f, 1000.0f);

    return true;
}
static float floaty;
bool update(float delta_time)
{
    if (glfwGetKey(renderer::get_window(), '1'))
        cam.set_position(vec3(50, 10, 50));
    if (glfwGetKey(renderer::get_window(), '2'))
        cam.set_position(vec3(-50, 10, 50));
    if (glfwGetKey(renderer::get_window(), '3'))
        cam.set_position(vec3(-50, 10, -50));
    if (glfwGetKey(renderer::get_window(), '4'))
        cam.set_position(vec3(50, 10, -50));

    // Rotate the sphere
    meshes["sphere"].get_transform().rotate(vec3(0.0f, half_pi<float>(), 0.0f) * delta_time);

    cam.update(delta_time);

	floaty = delta_time;

    return true;
}

bool render()
{
    // *********************************
    // Set render target to frame buffer
    // *********************************
	renderer::set_render_target(frame);

    // ***********
    // Clear frame
    // ***********
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render meshes
    for (auto &e : meshes)
    {
        auto m = e.second;
        // Bind effect
        renderer::bind(eff);
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
        // Create MV matrix
        auto MV = V * M;
        // Set MV matrix uniform
        glUniformMatrix4fv(
            eff.get_uniform_location("MV"), // Location of uniform
            1, // Number of values - 1 mat4
            GL_FALSE, // Transpose the matrix?
            value_ptr(MV)); // Pointer to matrix data
        // Set M matrix uniform
        glUniformMatrix4fv(
            eff.get_uniform_location("M"),
            1,
            GL_FALSE,
            value_ptr(M));
        // Set N matrix uniform
        glUniformMatrix3fv(
            eff.get_uniform_location("N"),
            1,
            GL_FALSE,
            value_ptr(m.get_transform().get_normal_matrix()));
        // Bind material
        renderer::bind(m.get_material(), "mat");
        // Bind light
        renderer::bind(light, "light");
        // Bind texture
        renderer::bind(tex, 0);
        // Set tex uniform
        glUniform1i(eff.get_uniform_location("tex"), 0);
        // Set eye position
        glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(cam.get_position()));

        // Render mesh
        renderer::render(m);
    }

    // ************************************
    // Set render target back to the screen
    // ************************************
	renderer::set_render_target();

    // Bind texture shader
    renderer::bind(tex_eff);

    // ******************************
    // MVP is now the identity matrix
    // ******************************
	glUniformMatrix4fv(
		tex_eff.get_uniform_location("MVP"), // Location of uniform
		1, // Number of values - 1 mat4
		GL_FALSE, // Transpose the matrix?
		value_ptr(mat4(1.0f))); // Pointer to matrix data

    // ******************************
    // Bind texture from frame buffer
    // ******************************
	renderer::bind(frame.get_frame(), 0);

    // ***************
    // Set the uniform
    // ***************
	glUniform1i(tex_eff.get_uniform_location("tex"), 0);



	glUniform1f(tex_eff.get_uniform_location("time"), floaty);
    

    // **********************
    // Render the screen quad
    // **********************
	renderer::render(screen_quad);

    return true;
}

void main()
{
    // Create application
    app application;
    // Set load content, update and render methods
    application.set_load_content(load_content);
    application.set_update(update);
    application.set_render(render);
    // Run application
    application.run();
}