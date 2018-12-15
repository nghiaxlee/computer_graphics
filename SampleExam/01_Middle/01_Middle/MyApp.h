#pragma once

// GLEW
#include <GL/glew.h>

// SDL
#include <SDL.h>
#include <SDL_opengl.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

#include "gCamera.h"
#include "gShaderProgram.h"
#include "gVertexBuffer.h"
#include "Mesh_OGL3.h"

class CMyApp
{
public:
	CMyApp(void);
	~CMyApp(void);

	bool Init();
	void Clean();

	void Update();
	void Render();

	void KeyboardDown(SDL_KeyboardEvent&);
	void KeyboardUp(SDL_KeyboardEvent&);
	void MouseMove(SDL_MouseMotionEvent&);
	void MouseDown(SDL_MouseButtonEvent&);
	void MouseUp(SDL_MouseButtonEvent&);
	void MouseWheel(SDL_MouseWheelEvent&);
	void Resize(int, int);
protected:
	GLuint GenTexture();

	GLuint m_textureID; 
	GLuint m_texture_wood; 

	void add_triangle(	const glm::vec3 &P1, 
								const glm::vec3 &P2, 
								const glm::vec3 &P3, 
								gVertexBuffer &buffer);
	
	void SurfaceEquation(float u, float v, glm::vec3& point, glm::vec3& normal, glm::vec2& texture);
	
	void genParametricSurface(int n, gVertexBuffer &buffer);
	
	const float sphereResolution = 20;

	gCamera			m_camera;
	gShaderProgram	m_program;
	gVertexBuffer	m_vb; // plane

	gVertexBuffer	m_vb_octa; // octahedron
	gVertexBuffer	m_vb_sphere; // sphere

	gVertexBuffer	m_vb_circle;
	bool InitCircle();
	void DrawCircle(const glm::mat4 &trans);
	void DrawMesh(const glm::mat4 &trans);
	int circle_resolution = 16;
	glm::vec4 ls;

	Mesh			*m_mesh;
};

