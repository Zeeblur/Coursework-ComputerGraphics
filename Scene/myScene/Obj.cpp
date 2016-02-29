#include <graphics_framework.h>
#include <glm\glm.hpp>
#include "Obj.h"


using namespace std;
using namespace graphics_framework;
using namespace glm;

Obj::Obj()
{
}

Obj::Obj(vec3 pos, vec3 rot, float theta, vec3 scal,
	mesh* me, material* mate, vector<texture*> texture,
	effect* eff, float myType)
{
	mat4 T = translate(mat4(1.0f), pos);
	if (myType == spotty)
		T = translate(mat4(1.0f), me->get_transform().position);
	mat4 R = rotate(mat4(1.0f), theta, rot);
	mat4 S = scale(mat4(1.0f), scal);


	mat4 trans = T * (R* S);

	this->mlocal = trans;		// copy vars
	this->m = me;
	this->mat = mate;
	this->eff = eff;
	this->myType = myType;
	this->tex = texture;
	this->theta = theta;
	this->rotV = rot;

	// trying mat
	
}

void Obj::update(Obj* parent, float delta_time)
{
	// used to recurse through children and concatenate transforms

	//transform by camera positon

	extern SceneManager* myScene;

	camera* cam = myScene->cam;			 // camera pointer 

	mworld = mlocal;

	if (myType == sky)
	{
		//vec3 difference = cam->get_position();// -m->get_transform().position;  // get difference in position
		mat4 trans = translate(mat4(1.0f), cam->get_position());

		mat4 rotation = rotate(mat4(1.0f), theta, rotV);
		theta += pi<float>() * delta_time * 0.01f;   // increment theta over time

		mworld = trans * rotation * mworld;
	}


	if (parent){
		if (parent->myType != sky && myType != sky)
		{
			mworld *= parent->mworld;
		}
	}

	for (auto &e : children)
	{
		Obj* child = e.second;
		child->update(this, delta_time);
	}

	calculateSphere();
}

void Obj::calculateSphere()
{
	// need to calculate bounding sphere for the object.
	
	// radius - find biggest length then half
	// center is the center of the object, position times transform.

	cent = vec3(mworld * vec4(m->get_transform().position, 1.0));
	//vec3 max = myScene->meshes["platform"].get_geometry().get_maximal_point();
	
	vec3 scale = vec3(mworld[0].x, mworld[1].y, mworld[2].z);
	
	vec3 maxPoints = scale * m->get_geometry().get_maximal_point();
	
	if (maxPoints.x >= maxPoints.y) // x bigger y, x bigger than z x biggest
	{
		if (maxPoints.x >= maxPoints.z)
		{
			radius = maxPoints.x;
		}
		else
		{
			radius = maxPoints.z;
		}

	}
	else (maxPoints.y >= maxPoints.z)
	{
		radius = maxPoints.y;
	}

}

void Obj::addChild(Obj* child, string name)
{
	child->parent = this;
	this->children[name] = child;
}

void Obj::render()
{
	/*
	 * method to recurse through branch and render all objects
	 */ 
	extern SceneManager* myScene;

    camera* cam = myScene->cam;			 // camera pointer 

	// get matrices + eye postion from the camera
	mat4 P = cam->get_projection();
	mat4 V = cam->get_view();
	vec3 eyeP = cam->get_position();

	
	// get normal matrix from mesh
	mat3 N = m->get_transform().get_normal_matrix();

	// calculate MVP from world
	auto MVP = P * V * mworld;

	if (myType == sky)
	{
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
	}

	// Bind the effect
	renderer::bind(*eff);

	// Set MVP matrix uniform
	glUniformMatrix4fv(
		eff->get_uniform_location("MVP"), // Location of uniform
		1,									    // Number of values - 1 mat4
		GL_FALSE,							    // Transpose the matrix?
		value_ptr(MVP));						// Pointer to matrix data


	// Set M matrix Uniform
	glUniformMatrix4fv(
		eff->get_uniform_location("M"),
		1,
		GL_FALSE,
		value_ptr(mworld));

	// Set 3x3 normal matrix from mesh
	glUniformMatrix3fv(
		eff->get_uniform_location("N"),
		1,
		GL_FALSE,
		value_ptr(N));


	auto T = glm::translate(mat4(1.0f), myScene->spot->get_position());
	auto R = glm::mat4_cast(glm::quat(myScene->spot->get_direction()));
	auto matrix = T * R;
	auto lV = myScene->shadow.get_view();

	auto lMVP = P * lV * matrix;

	if (myType == forShade)
	{
		glUniformMatrix4fv(
			eff->get_uniform_location("lightMVP"),
			1,
			GL_FALSE,
			value_ptr(lMVP));
	}

	if (waterObj)  // water flag to assign uniform moving water!
	{
		static float dd = 0.0f;
		dd += 0.002f;
		glUniform1f(eff->get_uniform_location("myTime"), dd );
	}

	// Bind Materials/lights/texture
	renderer::bind(*mat, "mat");

	renderer::bind(*myScene->light, "light");
	renderer::bind(*myScene->pointLight, "point");
	renderer::bind(*myScene->spot, "spot");
	renderer::bind(myScene->shadow.buffer->get_depth(), 1);


	for (int i = 0; i < tex.size(); ++i)  // bind every texture from object's list
	{
		renderer::bind(*tex[i], i);
		stringstream stream;
		stream << "tex[" << i << "]";

		glUniform1i(eff->get_uniform_location(stream.str()), i);
	}

	// set eye position (from active camera)
	glUniform3f(eff->get_uniform_location("eye_pos"), eyeP.x, eyeP.y, eyeP.z);




	// render mesh
	renderer::render(*m);

	if (myType == sky)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
	}

	// recurse for children
	for (auto &e : children)
	{
		Obj* child = e.second;
		child->render();
	}

}