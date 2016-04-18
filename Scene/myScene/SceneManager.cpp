 #include "SceneManager.h"


SceneManager::SceneManager(double initialMouseX, double initialMouseY)
{
	// constructor, initialising values.

	cam = nullptr;
	skybx = nullptr;
	generator = nullptr;
	
	// copy vars
	initialX = initialMouseX;
	initialY = initialMouseY;

	current_x = 0;   /// initialise current position of mouse
	current_y = 0;

	firstMouse = false;

	// initialise shadow map with screen size
	shadow = shadow_map(renderer::get_screen_width(), renderer::get_screen_height());

	debug = false;
	ssao = false;
	blur = false;

	// set type of geometry for radii to points.
	radiusGeom.set_type(GL_POINTS);

	fixCull = false; // bool used to turn on/off updating the view frustrum to show culling working

	createLights();
}

void SceneManager::createLights()
{
	directional_light *light = new directional_light;
	light->set_ambient_intensity(vec4(0.5f, 0.5f, 0.5f, 1.0f));

	// Light colour white
	light->set_light_colour(vec4(0.9f, 0.79f, 0.55f, 1.0f));

	// Light direction (1.0, 1.0, -1.0)
	light->set_direction(vec3(1.0f, 1.0f, -1.0f));
	Light *lightPtr = light;
	lightList.push_back(lightPtr);
	
	// create pointlights
	point_light *pointLight = new point_light;
	pointLight->set_light_colour(vec4(1.0f, 1.0f, 0.0f, 1.0f));
	pointLight->set_constant_attenuation(0.01f);
	pointLight->set_linear_attenuation(0.05f);
	pointLight->set_quadratic_attenuation(0.001f);
	Light* pointPtr = pointLight;
	lightList.push_back(pointPtr);
	
	// blue point light
	point_light *pointLightChild = new point_light;
	pointLightChild->set_light_colour(vec4(0.0f, 0.0f, 1.0f, 1.0f));
	pointLightChild->set_constant_attenuation(0.01f);
	pointLightChild->set_linear_attenuation(0.05f);
	pointLightChild->set_quadratic_attenuation(0.001f);
	Light* pointPtrChild = pointLightChild;
	lightList.push_back(pointPtrChild);

	// red point light
	point_light *pointLightChildChild = new point_light;
	pointLightChildChild->set_light_colour(vec4(1.0f, 0.0f, 0.0f, 1.0f));
	pointLightChildChild->set_constant_attenuation(0.01f);
	pointLightChildChild->set_linear_attenuation(0.05f);
	pointLightChildChild->set_quadratic_attenuation(0.001f);
	Light* ptrChildChild = pointLightChildChild;
	lightList.push_back(ptrChildChild);

	// spot light
	spot_light *spot = new spot_light;
	spot->set_direction(normalize(vec3(-1.0f, 0.0f, 0.0f)));
	spot->set_light_colour(vec4(1.0f, 1.0f, 0.0f, 1.0f));
	spot->set_range(100.0f);
	spot->set_power(0.01f);
	Light* spotPtr = spot;
	lightList.push_back(spotPtr);


}

effect* SceneManager::createEffect(char vertPath[], char fragPath[], char partPath1[], char partPath2[])
{
	// takes in paths of shaders, builds the effect and adds to a list of pointers.

	effect* myEffect = new effect;								
	myEffect->add_shader(vertPath, GL_VERTEX_SHADER);
	myEffect->add_shader(fragPath, GL_FRAGMENT_SHADER);

	if (partPath1)
		myEffect->add_shader(partPath1, GL_FRAGMENT_SHADER);

	if (partPath2)
		myEffect->add_shader(partPath2, GL_FRAGMENT_SHADER);
	
	
	myEffect->build();
	effectList.push_back(myEffect);
	
	return myEffect;
}

void SceneManager::calculateFrustrum()
{
	// method to calculate view frustrum based on camera postion. Called in update function unless fixCull is true.

	//near plane
	float fov = quarter_pi<float>();
	float near = 2.414f;
	float far = 1000.f;
	auto aspect = static_cast<float>(renderer::get_screen_width()) / static_cast<float>(renderer::get_screen_height());

	float hNear = 2 * tan(fov / 2) * near;		// height of near
	float wNear = hNear * aspect;				// width of near
	float hFar = 2 * tan(fov / 2) * far;		// height of far
	float wFar = hFar * aspect;					// width of far

	vec3 currentCamPos = cam->get_position();

	vec3 lookAt;
	lookAt = normalize(cam->get_target() - cam->get_position());
	vec3 right = cross(lookAt, vec3(0.0f, 1.0f, 0.0f));					// up cross lookat
	right = normalize(right);

	vec3 up = normalize(cross(lookAt, right));  //"real up"

	vec3 farCent = currentCamPos + (lookAt * far);		// center point of far plane look at* distance add camera pos
	vec3 nearCent = currentCamPos + (lookAt * near);

	planePoints[ftl] = farCent + (up * hFar * 0.5f) - (right * wFar * 0.5f);  // far top left - far center + up*half height - right*half width (minus because left)
	planePoints[ftr] = farCent + (up * hFar * 0.5f) + (right * wFar * 0.5f);  // far top right
	planePoints[fbl] = farCent - (up * hFar * 0.5f) - (right * wFar * 0.5f);  // far bottom left
	planePoints[fbr] = farCent - (up * hFar * 0.5f) + (right * wFar * 0.5f);  // far bottom right


	planePoints[ntl] = nearCent + (up * hNear * 0.5f) - (right * wNear * 0.5f);  // near top left
	planePoints[ntr] = nearCent + (up * hNear * 0.5f) + (right * wNear * 0.5f);  // near top right
	planePoints[nbl] = nearCent - (up * hNear * 0.5f) - (right * wNear * 0.5f);  // near bottom left
	planePoints[nbr] = nearCent - (up * hNear * 0.5f) + (right * wNear * 0.5f);  // near bottom right


	// Calculate the near and far planes (using camDir and the previously calculated centres)
	planeNormals[nearN] = lookAt;
	planeNormals[farN] = -lookAt;

	// Calculate the left and right planes (cross product to get the normals of the triangles and a point on the planes)
	planeNormals[leftN] = cross(up, (planePoints[fbl] - planePoints[nbl])); 
	planeNormals[rightN] = -cross(up, (planePoints[fbl] - planePoints[nbl]));

	// Calculate the top and bottom planes (similar to the left and right)
	planeNormals[topN] = cross(-right, (planePoints[ntr] - planePoints[ftr]));
	planeNormals[bottN] = cross(-right, (planePoints[fbr] - planePoints[nbr]));

	// normalise normals
	for (int i = 0; i < 6; ++i)
	{
		planeNormals[i] = normalize(planeNormals[i]);
	}

}

void SceneManager::generateFrustrumPlanes()
{
	// method to regenerate the frustrum geometry from the plane positions, called when culling is fixed.

	vector<vec3> positions
	{
		//near plane
		planePoints[ntl],
		planePoints[nbl],
		planePoints[nbr],
		planePoints[ntr],

		// far plane
		planePoints[ftl],
		planePoints[ftr],
		planePoints[fbr],
		planePoints[fbl],

		// left plane
		planePoints[ftl],
		planePoints[fbl],
		planePoints[nbl],
		planePoints[ntl],

		// right plane 
		planePoints[ntr],
		planePoints[nbr],
		planePoints[fbr],
		planePoints[ftr],

		// top plane
		planePoints[ftl],
		planePoints[ntl],
		planePoints[ntr],
		planePoints[ftr],

		// bottom plane
		planePoints[nbl],
		planePoints[fbl],
		planePoints[fbr],
		planePoints[nbr],
	};

	frustrumGeom.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);

}

void SceneManager::initQuad()
{
	frame = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());
	ssaoframe = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());
	vigFrame = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());

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

	greyEff.add_shader("..\\resources\\shaders\\simple_texture.vert", GL_VERTEX_SHADER);
	greyEff.add_shader("..\\resources\\shaders\\greyscale.frag", GL_FRAGMENT_SHADER);
	greyEff.build();

	blurEff.add_shader("..\\resources\\shaders\\simple_texture.vert", GL_VERTEX_SHADER);
	blurEff.add_shader("..\\resources\\shaders\\blur.frag", GL_FRAGMENT_SHADER);
	blurEff.build();

	simpleTex.add_shader("..\\resources\\shaders\\simple_texture.vert", GL_VERTEX_SHADER);
	simpleTex.add_shader("..\\resources\\shaders\\simple_texture.frag", GL_FRAGMENT_SHADER);
	simpleTex.build();

	ssao_Position.add_shader("..\\resources\\shaders\\ssaoGeomPass.vert", GL_VERTEX_SHADER);
	ssao_Position.add_shader("..\\resources\\shaders\\ssaoGeomPass.frag", GL_FRAGMENT_SHADER);
	ssao_Position.build();

	vignetteEff.add_shader("..\\resources\\shaders\\simple_texture.vert", GL_VERTEX_SHADER);
	vignetteEff.add_shader("..\\resources\\shaders\\vignette.frag", GL_FRAGMENT_SHADER);
	vignetteEff.build();

}



SceneManager::~SceneManager()
{
	// deconstructor

	// free all memory for lists and clear the pointers

	for (unsigned int i = 0; i < lightList.size(); ++i)
		delete lightList[i];

	lightList.clear();

	for (unsigned int i = 0; i < effectList.size(); ++i)
		delete effectList[i];

	effectList.clear();

	for (unsigned int i = 0; i < cameraList.size(); ++i)
		delete cameraList[i];

	cameraList.clear();

	for (unsigned int i = 0; i < texList.size(); ++i)			// texlist is a vector list of a vector list of texture pointers
	{
		for (unsigned int j = 0; j < texList[i].size(); ++j)     // for each element in texlist delete each element of list inside
			delete texList[i][j]; 
	}
	texList.clear();
}