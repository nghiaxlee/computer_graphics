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

// mesh
#include "ObjParser_OGL3.h"

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
	void DrawMesh(glm::mat4 transformation, // Transformation used for the current object
		Mesh *mesh, // 3D Mesh to render
		GLuint texture); // Texture of the mesh

	GLuint m_programID; 

	glm::mat4 m_matView;
	glm::mat4 m_matProj;

	GLuint	m_loc_world;
	GLuint	m_loc_worldIT;
	GLuint	m_loc_mvp;
	GLuint	m_loc_texture;

	float	m_angleY;

	glm::vec3 m_eye;

	GLuint m_vaoID;
	GLuint m_vboID;
	GLuint m_ibID;
	GLuint m_textureID;	
	GLuint m_modelTextureID;

	Mesh *m_mesh;

	struct Vertex
	{
		glm::vec3 p;	// position
		glm::vec3 n;	// normal
		glm::vec2 t;	// texture coords
	};
};

