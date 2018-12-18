#include "MyApp.h"
#include "GLUtils.hpp"

#include <GL/GLU.h>
#include <math.h>

#include "ObjParser_OGL3.h"

CMyApp::CMyApp(void)
{
	m_textureID = 0;
	m_mesh = 0;
}


CMyApp::~CMyApp(void)
{
}


GLuint CMyApp::GenTexture()
{
	unsigned char tex[256][256][3];

	for (int i = 0; i < 256; ++i)
		for (int j = 0; j < 256; ++j)
		{
			tex[i][j][0] = rand() % 256;
			tex[i][j][1] = rand() % 256;
			tex[i][j][2] = rand() % 256;
		}

	GLuint tmpID;

	glGenTextures(1, &tmpID);
	glBindTexture(GL_TEXTURE_2D, tmpID);
	gluBuild2DMipmaps(GL_TEXTURE_2D,
		GL_RGB8, 256, 256,			// define storage on the GPU: RGB, 8bits per channel, 256x256 texture
		GL_RGB, GL_UNSIGNED_BYTE,	// define storate in RAM: RGB layout, unsigned bytes for each channel
		tex);						//						  and source pointer
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// bilinear filter on min and mag
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	return tmpID;
}

void CMyApp::SurfaceEquation(float u, float v, glm::vec3& point, glm::vec3& normal, glm::vec2& texcoord) {
	float r = 2;
	u *= 2 * 3.1415f;
	v *= 3.1415f;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);

	point = glm::vec3(r*cu*sv, r*cv, r*su*sv);
	normal = glm::vec3(cu*sv, cv, su*sv);
	texcoord = glm::vec2(u, v);
}

void CMyApp::genParametricSurface(int n, gVertexBuffer &buffer) {
	for (int i = 0; i < n; ++i) for (int j = 0; j < n; ++j)
	{
		float u = i / (float)n, u_ = (i + 1) / (float)n, v = j / (float)n, v_ = (j + 1) / (float)n;
		glm::vec3 P1, P2, P3, P4, N1, N2, N3, N4;
		glm::vec2 T1, T2, T3, T4;

		SurfaceEquation(u, v, P1, N1, T1);
		SurfaceEquation(u_, v, P2, N2, T2);
		SurfaceEquation(u, v_, P3, N3, T3);
		SurfaceEquation(u_, v_, P4, N4, T4);

		buffer.AddData(0, P1); buffer.AddData(0, P2); buffer.AddData(0, P3);

		buffer.AddData(0, P3); buffer.AddData(0, P2); buffer.AddData(0, P4);

		buffer.AddData(1, N1); buffer.AddData(1, N2); buffer.AddData(1, N3);

		buffer.AddData(1, N3); buffer.AddData(1, N2); buffer.AddData(1, N4);

		buffer.AddData(2, T1); buffer.AddData(2, T2); buffer.AddData(2, T3);

		buffer.AddData(2, T3); buffer.AddData(2, T2); buffer.AddData(2, T4);

		buffer.AddIndex((i*n + j) * 6 + 0, (i*n + j) * 6 + 1, (i*n + j) * 6 + 2);
		buffer.AddIndex((i*n + j) * 6 + 3, (i*n + j) * 6 + 4, (i*n + j) * 6 + 5);
	}
}


void CMyApp::add_triangle(
	const glm::vec3 &P1,
	const glm::vec3 &P2,
	const glm::vec3 &P3,
	gVertexBuffer &buffer) {

	buffer.AddData(0, P1.x, P1.y, P1.z); // P1
	buffer.AddData(0, P2.x, P2.y, P2.z); // P2
	buffer.AddData(0, P3.x, P3.y, P3.z); // P3

	glm::vec3 V1 = P2 - P1; //P2-P1
	glm::vec3 V2 = P3 - P1; //P3-P1
	glm::vec3 normal = glm::normalize(glm::cross(V1, V2));

	//normals
	buffer.AddData(1, normal.x, normal.y, normal.z);
	buffer.AddData(1, normal.x, normal.y, normal.z);
	buffer.AddData(1, normal.x, normal.y, normal.z);

	//texture coordinates
	buffer.AddData(2, 0, 0);
	buffer.AddData(2, 0.5, 1);
	buffer.AddData(2, 1, 0);
}

bool CMyApp::Init()
{
	// background color
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	// set cullface and z-buffer ON, cull backfaces
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	//
	// create the geometry
	//

	InitHexagonalPyramid();

	// A monkey-head will do for now
	m_mesh_ellipsoid = ObjParser::parse("Suzanne.obj");
	m_mesh_ellipsoid->initBuffers();
	m_textureID_wood5 = TextureFromFile("wood5.jpg");
	m_textureID_generatedred = GenTextureRed();

	/*
	Create a plane­quad (rectangular geometry using VAO­s and VBO­s or gVertexBuffer) in the y=0 plane, 
	of dimensions 30x30 and place it down as the ‘ground’, the models ‘standing’ on top of it. (1 point) 
	Specify correct texture coordinates (1 point) and normals (1 point) for the plane­quad.
	*/
	//ground
	{
		m_vb.AddAttribute(0, 3); //positions
		m_vb.AddAttribute(1, 3); //normals 
		m_vb.AddAttribute(2, 2); // tex coords

		//positions
		m_vb.AddData(0, -15, 0, -15);
		m_vb.AddData(0, 15, 0, -15);
		m_vb.AddData(0, -15, 0, 15);
		m_vb.AddData(0, 15, 0, 15);

		// normals
		m_vb.AddData(1, 0, 1, 0);
		m_vb.AddData(1, 0, 1, 0);
		m_vb.AddData(1, 0, 1, 0);
		m_vb.AddData(1, 0, 1, 0);

		// tex coords
		m_vb.AddData(2, 0, 0);
		m_vb.AddData(2, 1, 0);
		m_vb.AddData(2, 0, 1);
		m_vb.AddData(2, 1, 1);

		m_vb.AddIndex(1, 0, 2);
		m_vb.AddIndex(1, 2, 3);

		m_vb.InitBuffers();
	}

	//
	// load shaders	
	//
	m_program.AttachShader(GL_VERTEX_SHADER, "dirLight.vert");
	m_program.AttachShader(GL_FRAGMENT_SHADER, "dirLight.frag");

	m_program.BindAttribLoc(0, "vs_in_pos");
	m_program.BindAttribLoc(1, "vs_in_normal");
	m_program.BindAttribLoc(2, "vs_in_tex0");

	if (!m_program.LinkProgram())
	{
		return false;
	}

	//
	// misc init
	//

	m_camera.SetProj(45.0f, 640.0f / 480.0f, 0.01f, 1000.0f);

	m_textureID = TextureFromFile("texture.png");

	m_mesh = ObjParser::parse("Suzanne.obj");
	m_mesh->initBuffers();

	return true;
}

void CMyApp::Clean()
{
	glDeleteTextures(1, &m_textureID);

	m_program.Clean();
}

void CMyApp::Update()
{
	static Uint32 last_time = SDL_GetTicks();
	float delta_time = (SDL_GetTicks() - last_time) / 1000.0f;

	m_camera.Update(delta_time);

	last_time = SDL_GetTicks();
}


void CMyApp::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the light color
	m_program.On();
	{
		m_program.SetUniform("Ld", m_Ld);
	}
	m_program.Off();

	// draw rectangular planequad
	m_program.On();
	{
		glm::mat4 matWorld = glm::mat4(1.0f);
		glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));
		glm::mat4 mvp = m_camera.GetViewProj() *matWorld;

		m_program.SetUniform("world", matWorld);
		m_program.SetUniform("worldIT", matWorldIT);
		m_program.SetUniform("MVP", mvp);
		m_program.SetUniform("eye_pos", m_camera.GetEye());

		m_program.SetTexture("texImage", 0, m_textureID);

		// draw with VAO
		m_vb.On();
		{
			m_vb.DrawIndexed(GL_TRIANGLES, 0, 6, 0); //IB
		}
		m_vb.Off();
	}
	m_program.Off();

	// draw pyramids
	glm::mat4 pyro_high = glm::scale(glm::vec3(1, 1 + 0.5*sin(SDL_GetTicks() / 1000.0), 1)); // Their height scale should change over time, periodically (1 point). 
	DrawHexagonalPyramid(glm::translate(glm::vec3(-15, 0, -15)) * pyro_high);
	DrawHexagonalPyramid(glm::translate(glm::vec3(15, 0, -15)) * pyro_high);
	DrawHexagonalPyramid(glm::translate(glm::vec3(-15, 0, 15)) * pyro_high);
	DrawHexagonalPyramid(glm::translate(glm::vec3(15, 0, 15)) * pyro_high);

	// draw ellipsoid
	DrawEllipsoid(
		glm::translate(glm::vec3(0, 2 + sin(SDL_GetTicks() / 1000.0), 0)) *
		glm::rotate<float>(M_PI / 2, glm::vec3(0, 0, 1)));
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{

	switch (key.keysym.sym)
	{
	case '1':
		m_camera.SetView(glm::vec3(-15, 10, -15), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		break;
	case '2':
		m_camera.SetView(glm::vec3(15, 10, -15), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		break;
	case '3':
		m_camera.SetView(glm::vec3(15, 10, -15), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		break;
	case '4':
		m_camera.SetView(glm::vec3(15, 10, 15), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		break;
	case 'r':
		m_Ld = glm::vec4(1, 0, 0, 1);
		break;
	case 'g':
		m_Ld = glm::vec4(0, 1, 0, 1);
		break;
	case 'b':
		m_Ld = glm::vec4(0, 0, 1, 1);
		break;
	default:
		break;
	}

	m_camera.KeyboardDown(key);
}

void CMyApp::KeyboardUp(SDL_KeyboardEvent& key)
{
	m_camera.KeyboardUp(key);
}

void CMyApp::MouseMove(SDL_MouseMotionEvent& mouse)
{
	m_camera.MouseMove(mouse);
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

void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);

	m_camera.Resize(_w, _h);
}

/*
Create an Hexagonal Pyramid using VAO­s and VBO­s (or VertexBuffer), 
where each edge of the hexagon has a length of 4, and the pyramid has a height of 8 (3 points). 
Specify surface normals and texture coordinates for each vertex (1­-1 point).
*/
void CMyApp::InitHexagonalPyramid()
{
	const int circle_resolution = 6; // hexagonal
	const float radius = 4; // each edge of the hexagon has a length of 4 => for the hexagon, that's the radius too.
	const float height = 8;

	// init attributes
	m_vb_hexagonal_pyramid.AddAttribute(0, 3); // positions
	m_vb_hexagonal_pyramid.AddAttribute(1, 3); // normals
	m_vb_hexagonal_pyramid.AddAttribute(2, 2); // tex.coords.

	// a function to get a point of the circle
	auto get_circle_point = [](int segment, int num_segments, float radius) {
		float angle = segment / (float)num_segments * 2 * M_PI;
		float u = cos(angle);
		float v = sin(angle);
		return glm::vec3(u*radius, 0, v*radius);
	};

	// create the bottom
	for (int i = 0; i < circle_resolution; ++i) {
		glm::vec3 p1 = get_circle_point(i + 0, circle_resolution, radius);
		glm::vec3 p2 = get_circle_point(i + 1, circle_resolution, radius);
		glm::vec3 p3 = glm::vec3(0, 0, 0); // center of the bottom
		add_triangle(p1, p2, p3, m_vb_hexagonal_pyramid); // let's re-use this, it defines normals too!
		m_vb_hexagonal_pyramid.AddIndex(
			(3 * i + 0), 
			(3 * i + 1),
			(3 * i + 2));
	}

	// create the top
	for (int i = 0; i < circle_resolution; ++i) {
		glm::vec3 p3 = glm::vec3(0, height, 0); // center of the top
		glm::vec3 p2 = get_circle_point(i + 1, circle_resolution, radius);
		glm::vec3 p1 = get_circle_point(i + 0, circle_resolution, radius);
		add_triangle(p3, p2, p1, m_vb_hexagonal_pyramid); // let's re-use this, it defines normals too!
		m_vb_hexagonal_pyramid.AddIndex(
			18 + (3 * i + 0),
			18 + (3 * i + 1),
			18 + (3 * i + 2));
	}

	// initbuffers
	m_vb_hexagonal_pyramid.InitBuffers();
}

void CMyApp::DrawHexagonalPyramid(const glm::mat4& world)
{
	m_program.On();
	{
		glm::mat4 matWorld = world;
		glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));
		glm::mat4 mvp = m_camera.GetViewProj() *matWorld;

		m_program.SetUniform("world", matWorld);
		m_program.SetUniform("worldIT", matWorldIT);
		m_program.SetUniform("MVP", mvp);
		m_program.SetUniform("eye_pos", m_camera.GetEye());

		m_program.SetTexture("texImage", 0, m_textureID_generatedred);

		// draw with VAO
		m_vb_hexagonal_pyramid.On();
		{
			m_vb_hexagonal_pyramid.DrawIndexed(GL_TRIANGLES, 0, 6*3*2, 0); //IB
		}
		m_vb_hexagonal_pyramid.Off();
	}
	m_program.Off();
}

GLuint CMyApp::GenTextureRed()
{
	unsigned char tex[256][256][3];

	for (int i = 0; i < 256; ++i)
		for (int j = 0; j < 256; ++j)
		{
			tex[i][j][0] = 255; // red
			tex[i][j][1] = rand() % 64;
			tex[i][j][2] = rand() % 64;
		}

	GLuint tmpID;

	glGenTextures(1, &tmpID);
	glBindTexture(GL_TEXTURE_2D, tmpID);
	gluBuild2DMipmaps(GL_TEXTURE_2D,
		GL_RGB8, 256, 256,			// define storage on the GPU: RGB, 8bits per channel, 256x256 texture
		GL_RGB, GL_UNSIGNED_BYTE,	// define storate in RAM: RGB layout, unsigned bytes for each channel
		tex);						//						  and source pointer
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// bilinear filter on min and mag
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	return tmpID;
}

void CMyApp::DrawEllipsoid(const glm::mat4& world)
{
	m_program.On();
	{
		glm::mat4 matWorld = world;
		glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));
		glm::mat4 mvp = m_camera.GetViewProj() *matWorld;

		m_program.SetUniform("world", matWorld);
		m_program.SetUniform("worldIT", matWorldIT);
		m_program.SetUniform("MVP", mvp);
		m_program.SetUniform("eye_pos", m_camera.GetEye());

		m_program.SetTexture("texImage", 0, m_textureID_wood5);

		m_mesh_ellipsoid->draw();
	}
	m_program.Off();
}