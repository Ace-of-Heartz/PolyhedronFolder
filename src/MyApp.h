#pragma once
#define GLM_ENABLE_EXPERIMENTAL

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

// GLEW
#include <GL/glew.h>

// SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

// Utils
#include "GLUtils.hpp"
#include "Camera.h"
#include "CameraManipulator.h"
#include "Polyhedron.h"
#include "Light.h"

using namespace PolyhedronFolder;

struct SUpdateInfo
{
	float ElapsedTimeInSec = 0.0f; // Program indulása óta eltelt idő
	float DeltaTimeInSec   = 0.0f; // Előző Update óta eltelt idő
};

class CMyApp
{
public:
	CMyApp();
	~CMyApp();

	bool Init();
	void Clean();

	void LoadTexture(const std::string& filename);

	void Update( const SUpdateInfo& );
	void Render();
	void RenderPolyhedron();
	void RenderObject();
	void RenderAxis();

	void RenderGUI();

	void KeyboardDown(const SDL_KeyboardEvent&);
	void KeyboardUp(const SDL_KeyboardEvent&);
	void MouseMove(const SDL_MouseMotionEvent&);
	void MouseDown(const SDL_MouseButtonEvent&);
	void MouseUp(const SDL_MouseButtonEvent&);
	void MouseWheel(const SDL_MouseWheelEvent&);
	void Resize(int, int);

	void OtherEvent( const SDL_Event& );
protected:
	void SetupDebugCallback();

	//
	// Adat változók
	//

	float m_ElapsedTimeInSec = 0.0f;

	// Kamera
	Camera m_camera;
	CameraManipulator m_cameraManipulator;

	//
	// OpenGL-es dolgok
	//
	
	// shaderekhez szükséges változók
	GLuint m_programID = 0;		  // shaderek programja

	GLuint m_programAxis = 0;

	// Fényforrás- ...

	Light m_light1 = Light(
		glm::vec4( 0.0f, 1.0f, 1.0f, 0.0f ),glm::vec3( 0.125f ),glm::vec3(1.0, 1.0, 1.0 ),glm::vec3(0.0, 0.0, 0.0 )
		);
	Light m_light2 = Light(
		glm::vec4( 0.0f, -1.0f, -1.0f, 0.0f ),glm::vec3( 0.125f ),glm::vec3(1.0, 1.0, 1.0 ),glm::vec3(0.0, 0.0, 0.0 )
		);



	float m_lightConstantAttenuation    = 1.0;
	float m_lightLinearAttenuation      = 0.0;
	float m_lightQuadraticAttenuation   = 0.0;

	// ... és anyagjellemzők
	glm::vec3 m_Ka = glm::vec3( 1.0 );
	glm::vec3 m_Kd = glm::vec3( 1.0 );
	glm::vec3 m_Ks = glm::vec3( 1.0 );

	float m_Shininess = 1.0;



	// Shaderek inicializálása, és törtlése
	void InitShaders();
	void CleanShaders();


	// Geometriával kapcsolatos változók

	OGLObject m_PolyhedronPoly = {};
	OGLObject m_PolyhedronObject = {};

	// Polyhedron
	glm::mat4 m_baseTransformation = glm::mat4(1.0f);
	Polyhedron m_Polyhedron;
	bool m_animate = false;
	float m_animationSpeed = 1.0f;
	float m_currentFoldValue = 0.0f;



	// Geometria inicializálása, és törtlése
	void InitGeometry();
	void CleanGeometry();

	// Textúrázás, és változói
    GLuint m_SamplerID = 0;

	GLuint m_PolyhedronTextureID = 0;

	void InitTextures();
	void CleanTextures();

	void SetLightingUniforms( GLuint program, float Shininess, glm::vec3 Ka = glm::vec3( 1.0 ), glm::vec3 Kd  = glm::vec3( 1.0 ), glm::vec3 Ks  = glm::vec3( 1.0 ) );
};

