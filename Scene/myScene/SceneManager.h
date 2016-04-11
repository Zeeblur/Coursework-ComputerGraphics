/*
 * Scene Manager header file for class, to store scene-wide lists of objects/light pointers, etc.
 * Also calculates view frustrum.
 */


#pragma once

#include <graphics_framework.h>
#include <glm\glm.hpp>
#include "GenerateBack.h"
#include "Light.h"

using namespace std;
using namespace graphics_framework;
using namespace glm;

class GenerateBack;
class Obj;				// forward declaration of object


enum objType { sky, terrn, waterObj, object, pointLightObj, forShade, spotty, glassOb, movingObject }; //  enum
enum planeNum { farN, topN, leftN, nearN, bottN, rightN };
enum planePointNames {ftl, ftr, fbl, fbr, ntl, ntr, nbl, nbr };
class SceneManager
{
private:

	void createLights();

	bool debug;			 // flag for turning on polygon mode to render outlines
	bool fixCull;		 // flag for fixing view culling
	bool firstMouse;	 // flag for first mouse (for free camera)

	double initialX;				// vars for mouse positions initial
	double initialY;
	double current_x;			    // var for current mouse pos
	double current_y;

	float myTime; // float for passing in uniform for water and point light's vertex displacement + movement



	void initQuad();

public:

	Obj* skybx;

	GenerateBack* generator;

	camera* cam;

	vector<camera*> cameraList;
	vector<Light*> lightList;
	vector<vector<texture*>> texList;
	
	vector<effect*> effectList;  // list of all effect pointers for freeing

	vector<Obj*> list;
	vector<Obj*> transparentObjects;

	vector<Obj*> lightObjects;

	effect* shad_eff;
	effect* rad_eff;

	map<string, mesh> meshes;
	map<string, material> materials;
	map<string, cubemap> cubemaps;

	shadow_map shadow;

	geometry radiusGeom;		// geometry for scene debugging 
	geometry frustrumGeom;
	effect	 frustrumEffect;

	vec3 planeNormals[6];		// var for storing view frustrum plane normals.
	vec3 planePoints[8];


	effect* createEffect(char vertPath[], char fragPath[], char partPath1[], char partPath2[]);
	SceneManager(double initialMouseX, double initialMouseY);  // constructor takes in initial mouse positions for free camera setup
	~SceneManager();// deconstructor frees lists

	// recalculates view frustrum from current camera positions
	void calculateFrustrum();
	void generateFrustrumPlanes();

	// getters/setters for values	
	bool getDebugBool(){ return debug; }
	void setDebugBool(const bool &value){ debug = value; }

	bool getFixCullBool(){ return fixCull; }
	void setFixCullBool(const bool &value){ fixCull = value; }

	bool getfirstMouse(){ return firstMouse; }
	void setfirstMouse(const bool &value){ firstMouse = value; }

	double getInitialX(){ return initialX; }
	double getInitialY(){ return initialY; }

	void setCurrX(const double &value) { current_x = value; }
	double getCurrX() { return current_x; }

	void setCurrY(const double &value) { current_y = value; }
	double getCurrY() { return current_y; }

	void incrementMyTime(const float &value){ myTime += value;	}
	float getMyTime(){ return myTime; }

};

