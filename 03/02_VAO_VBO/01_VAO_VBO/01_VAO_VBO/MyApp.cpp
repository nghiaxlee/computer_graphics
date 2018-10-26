#include "MyApp.h"
#include <math.h>
#include <random>
#include <iostream>

std::random_device rd;
std::mt19937 mt(rd());
#define GetRand std::uniform_real_distribution<double>(0, 1)(mt)

CMyApp::CMyApp(void)
{
	m_vaoID1 = 0;
	m_vaoID2 = 0;
	m_vboID1 = 0;
	m_vboID2 = 0;
}


CMyApp::~CMyApp(void)
{
}

const int detailness = 60;

bool CMyApp::Init()
{
	// set the clear color (background color)
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE);		// enable backface culling
	glEnable(GL_DEPTH_TEST);	// enable depth test (z-buffer)

	//
	// create the geometry
	//

	//const int detailness = 60;
	const double radius1 = 0.5;
	const double radius2 = 1;
	Vertex vert1[detailness * 3];
	Vertex vert2[detailness * 3];
	double angle_step = M_PI * 2 / detailness;
	for (int i = 0; i < detailness; ++i)
	{
		double angle1 = i * angle_step;
		double x11 = radius1 * cos(angle1);
		double y11 = radius1 * sin(angle1);
		double x12 = radius2 * cos(angle1);
		double y12 = radius2 * sin(angle1);


		double angle2 = (i + 1) * angle_step;
		double x21 = radius1 * cos(angle2);
		double y21 = radius1 * sin(angle2);
		double x22 = radius2 * cos(angle2);
		double y22 = radius2 * sin(angle2);

		double x31 = 0;
		double y31 = 0;
		double x32 = 0;
		double y32 = 0;

		double r = abs(sin(angle1));//angle1 / 2 / M_PI;
		double g = abs(cos(angle1));//angle2 / 2 / M_PI;
		double b = angle1 / 2 / M_PI;

		vert1[3 * i + 2] = { glm::vec3(x11, y11, 0), glm::vec3(r, g, b) };
		vert1[3 * i + 0] = { glm::vec3(x21, y21, 0), glm::vec3(r, g, b) };
		vert1[3 * i + 1] = { glm::vec3(x31, y31, 0), glm::vec3(r, g, b) };
		vert2[3 * i + 2] = { glm::vec3(x12, y12, 0), glm::vec3(b, r, g) };
		vert2[3 * i + 0] = { glm::vec3(x22, y22, 0), glm::vec3(b, r, g) };
		vert2[3 * i + 1] = { glm::vec3(x32, y32, 0), glm::vec3(b, r, g) };
	}

	// generate a VAO ID
	glGenVertexArrays(1, &m_vaoID1);
	// activate the generated VAO
	glBindVertexArray(m_vaoID1);
	
	// generate a VBO ID
	glGenBuffers(1, &m_vboID1); 
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID1); // activate the new VBO
	// allocate the VBO (in the onboard GPU memory) and fill it with data from system RAM (vert[])
	glBufferData( GL_ARRAY_BUFFER,	// which activated buffer object to allocate
				  sizeof(vert1),		// how many bytes should be allocated
				  vert1,				// copy data from system RAM from this memory address
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

// Object 2
	// generate a VAO ID
	glGenVertexArrays(1, &m_vaoID2);
	// activate the generated VAO
	glBindVertexArray(m_vaoID2);

	// generate a VBO ID
	glGenBuffers(1, &m_vboID2);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID2); // activate the new VBO
	// allocate the VBO (in the onboard GPU memory) and fill it with data from system RAM (vert[])
	glBufferData(GL_ARRAY_BUFFER,	// which activated buffer object to allocate
		sizeof(vert2),		// how many bytes should be allocated
		vert2,				// copy data from system RAM from this memory address
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
		(void*)(sizeof(glm::vec3))); // stride = 3*sizeof(float) i.e. skip 3 floats at the beginning

	glBindVertexArray(0); // deactivate the VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0); // deactiviate the VBO

	return true;
}

void CMyApp::Clean()
{
	glDeleteBuffers(1, &m_vboID1);
	glDeleteBuffers(1, &m_vboID2);
	glDeleteVertexArrays(1, &m_vaoID1);
	glDeleteVertexArrays(1, &m_vaoID2);
}

void CMyApp::Update()
{
	//Vertex vert[] =
	//{
	//	// position x,  y, z;  color     r, g, b
	//	{glm::vec3(0, 0.5, 0), glm::vec3(GetRand, GetRand, GetRand)},
	//	{glm::vec3(-0.5, 0, 0), glm::vec3(GetRand, GetRand, GetRand)},
	//	{glm::vec3(0.5, 0, 0), glm::vec3(GetRand, GetRand, GetRand)},
	//	{glm::vec3(-0.5, 0, 0), glm::vec3(GetRand, GetRand, GetRand)},
	//	{glm::vec3(0, -0.5, 0), glm::vec3(GetRand, GetRand, GetRand)},
	//	{glm::vec3(0.5, 0, 0), glm::vec3(GetRand, GetRand, GetRand)},
	//};
	//// generate a VAO ID
	//glGenVertexArrays(1, &m_vaoID);
	//// activate the generated VAO
	//glBindVertexArray(m_vaoID);

	//// generate a VBO ID
	//glGenBuffers(1, &m_vboID);
	//glBindBuffer(GL_ARRAY_BUFFER, m_vboID); // activate the new VBO
	//// allocate the VBO (in the onboard GPU memory) and fill it with data from system RAM (vert[])
	//glBufferData(GL_ARRAY_BUFFER,	// which activated buffer object to allocate
	//	sizeof(vert),		// how many bytes should be allocated
	//	vert,				// copy data from system RAM from this memory address
	//	GL_STATIC_DRAW);	// usage flags


	//// register in the VAO that general attribute index 0 is activated and it contains 3 floats per vertex
	//glEnableVertexAttribArray(0); // enable channel 0 (position)
	//glVertexAttribPointer(
	//	(GLuint)0,		// we set the descriptor (pointer) of channel 0`s attributes
	//	3,				// it contains 3
	//	GL_FLOAT,		// floats
	//	GL_FALSE,		// not normalized
	//	sizeof(Vertex),	// stride (0=tightly packed)
	//	0				// first data at the beginning of the buffer
	//);

	//// register in the VAO that general attribute index 3 is activated and it contains 3 floats per vertex
	//glEnableVertexAttribArray(3); // enable channel 3 (color)
	//glVertexAttribPointer(
	//	(GLuint)3,
	//	3,
	//	GL_FLOAT,
	//	GL_FALSE,
	//	sizeof(Vertex),
	//	(void*)(sizeof(glm::vec3))); // stride = 3*sizeof(float) i.e. skip 3 floats at the beginning

	//glBindVertexArray(0); // deactivate the VAO
	//glBindBuffer(GL_ARRAY_BUFFER, 0); // deactiviate the VBO
}


void CMyApp::Render()
{
	// clear the frame buffer (GL_COLOR_BUFFER_BIT) and z-buffer (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// turn on the VAO
	glBindVertexArray(m_vaoID1);

	// draw
	glDrawArrays(	GL_TRIANGLES,	// primitive type
					0,					// start index
					detailness * 3);					// number of vertices

	// VAO off
	glBindVertexArray(0);

	// turn on the VAO
	glBindVertexArray(m_vaoID2);

	// draw
	glDrawArrays(GL_TRIANGLES,	// primitive type
		0,					// start index
		detailness * 3);					// number of vertices

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