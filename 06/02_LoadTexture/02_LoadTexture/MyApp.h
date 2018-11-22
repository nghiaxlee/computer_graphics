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

class CMyApp
{
public:
	CMyApp(void);
	~CMyApp(void);

	bool Init();
	void Clean();

	void Update();
	void Render1Plane(GLuint texture, glm::mat4 &trans);
	void Render();

	void KeyboardDown(SDL_KeyboardEvent&);
	void KeyboardUp(SDL_KeyboardEvent&);
	void MouseMove(SDL_MouseMotionEvent&);
	void MouseDown(SDL_MouseButtonEvent&);
	void MouseUp(SDL_MouseButtonEvent&);
	void MouseWheel(SDL_MouseWheelEvent&);
	void Resize(int, int);
protected:
	// internal methods
	GLuint GenTexture();

	// shader variables
	GLuint m_programID; 
	GLuint	m_loc_mvp;
	GLuint	m_loc_texture;

	// matrices
	glm::mat4 m_matWorld;
	glm::mat4 m_matView;
	glm::mat4 m_matProj;

	// OpenGL variables
	GLuint m_vaoID; 
	GLuint m_vboID; 
	GLuint m_ibID;  
	GLuint m_textureID; 
	GLuint m_waterTextureID; 

	struct Vertex
	{
		glm::vec3 p; // position
		glm::vec3 c; // color
		glm::vec2 t; // texture coordinates
	};

};

