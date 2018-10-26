#include "MyApp.h"
#include <math.h>

CMyApp::CMyApp(void)
{
	m_vaoID = 0;
	m_vboID = 0;
}


CMyApp::~CMyApp(void)
{
}

bool CMyApp::Init()
{
	// set the clear color (background color)
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE);		// enable backface culling
	glEnable(GL_DEPTH_TEST);	// enable depth test (z-buffer)

	//
	// create the geometry
	//

	const int detailness = 20;
	Vertex vertices[3 * detailness];
	float angle_step = 2 * M_PI / detailness;
	float radius = 1;

	for (int i = 0; i < detailness; ++i)
	{
		float x = radius * cos(i * angle_step);
		float y = radius * sin(i * angle_step);

		vertices[3 * i].p = glm::vec3(x, y, 0);
		vertices[3 * i].c = glm::vec3(1, 1, 1);

		x = radius * cos((i + 1) * angle_step);
		y = radius * sin((i + 1) * angle_step);
		if (i % 2)
		{
			vertices[3 * i + 1].p = glm::vec3(0, 0, 0);
			vertices[3 * i + 1].c = glm::vec3(1, 1, 1);

			vertices[3 * i + 2].p = glm::vec3(x, y, 0);
			vertices[3 * i + 2].c = glm::vec3(1, 1, 1);
		}
		else
		{
			vertices[3 * i + 2].p = glm::vec3(0, 0, 0);
			vertices[3 * i + 2].c = glm::vec3(1, 1, 1);

			vertices[3 * i + 1].p = glm::vec3(x, y, 0);
			vertices[3 * i + 1].c = glm::vec3(1, 1, 1);
		}
 	}

	/*Vertex vert[] =
	{ 
		// position x,  y, z;  color     r, g, b
		{glm::vec3(-1, -1, 0), glm::vec3(1, 0, 0)},
		{glm::vec3( 1, -1, 0), glm::vec3(0, 1, 0)},
		{glm::vec3(-1,  1, 0), glm::vec3(0, 0, 1)},
		{glm::vec3( 1,  1, 0), glm::vec3(1, 1, 1)},
	};*/

	// generate a VAO ID
	glGenVertexArrays(1, &m_vaoID);
	// activate the generated VAO
	glBindVertexArray(m_vaoID);
	
	// generate a VBO ID
	glGenBuffers(1, &m_vboID); 
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID); // activate the new VBO
	// allocate the VBO (in the onboard GPU memory) and fill it with data from system RAM (vert[])
	glBufferData( GL_ARRAY_BUFFER,	// which activated buffer object to allocate
				  sizeof(vertices),		// how many bytes should be allocated
				  vertices,				// copy data from system RAM from this memory address
				  GL_STATIC_DRAW);	// usage flags
	

	// register in the VAO that general attribute index 0 is activated and it contains 3 floats per vertex
	glEnableVertexAttribArray(0); // enable channel 0 (position)
	glVertexAttribPointer(
		(GLuint)0,		// we set the descriptor (pointer) of channel 0`s attributes
		3,				// it contains 3
		GL_FLOAT,		// floats
		GL_FALSE,		// not normalized
		sizeof(Vertex),	// stride (0=tightly packed)
		0				// first data at the beginning of the buffer
	); 

	// register in the VAO that general attribute index 3 is activated and it contains 3 floats per vertex
	glEnableVertexAttribArray(3); // enable channel 3 (color)
	glVertexAttribPointer(
		(GLuint)3,
		3, 
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(sizeof(glm::vec3)) ); // stride = 3*sizeof(float) i.e. skip 3 floats at the beginning

	glBindVertexArray(0); // deactivate the VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0); // deactiviate the VBO

	return true;
}

void CMyApp::Clean()
{
	glDeleteBuffers(1, &m_vboID);
	glDeleteVertexArrays(1, &m_vaoID);
}

void CMyApp::Update()
{
}


void CMyApp::Render()
{
	// clear the frame buffer (GL_COLOR_BUFFER_BIT) and z-buffer (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// turn on the VAO
	glBindVertexArray(m_vaoID);

	// draw
	glDrawArrays(	GL_TRIANGLE_STRIP,	// primitive type
					0,					// start index
					20*3);					// number of vertices

	// VAO off
	glBindVertexArray(0);
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
}

void CMyApp::KeyboardUp(SDL_KeyboardEvent& key)
{
}

void CMyApp::MouseMove(SDL_MouseMotionEvent& mouse)
{

}

void CMyApp::MouseDown(SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseUp(SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseWheel(SDL_MouseWheelEvent& wheel)
{
}

// _w: new width, _h: new height
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h );
}