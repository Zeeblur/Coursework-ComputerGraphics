#version 440

// Cubemap data
uniform samplerCube cubemap;
// Tarnish texture
uniform sampler2D tex;

// Incoming vertex position
layout (location = 0) in vec2 tex_coord;
// Incoming environment map texture coordinate
layout (location = 1) in vec3 env_map_coord;

// Outgoing colour
layout (location = 0) out vec4 colour;

void main()
{
	colour = vec4(0.0, 0.0, 0.0, 1.0);
	// **********************
	// Sample environment map
	// **********************
	
	// **********************
	// Sample tarnish texture
	// **********************
	
	// ******************************************************
	// Final colour is product ot environment map and tarnish
	// ******************************************************
	
}