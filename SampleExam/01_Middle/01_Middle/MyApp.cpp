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
	
	for (int i = 0; i<256; ++i)
	for (int j = 0; j<256; ++j)
	{
		tex[i][j][0] = 255;
		tex[i][j][1] = 0;
		tex[i][j][2] = 0;
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
	u *= 2*3.1415f;
	v *= 3.1415f;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	
	point = glm::vec3(r*cu*sv, r*cv, r*su*sv);
	normal = glm::vec3(cu*sv, cv, su*sv);
	texcoord = glm::vec2(u, v);
}

void CMyApp::genParametricSurface(int n, gVertexBuffer &buffer) {
	for(int i=0;i<n;++i) for(int j=0;j<n;++j)
	{
		float u = i/(float)n, u_ = (i+1)/(float)n, v = j/(float)n, v_ = (j+1)/(float)n;
		glm::vec3 P1, P2, P3, P4, N1, N2, N3, N4;
		glm::vec2 T1, T2, T3, T4;
		
		SurfaceEquation(u, v,   P1, N1, T1);
		SurfaceEquation(u_, v,  P2, N2, T2);
		SurfaceEquation(u, v_,  P3, N3, T3);
		SurfaceEquation(u_, v_, P4, N4, T4);
		
		buffer.AddData(0, P1); buffer.AddData(0, P2); buffer.AddData(0, P3);
		
		buffer.AddData(0, P3); buffer.AddData(0, P2); buffer.AddData(0, P4);
		
		buffer.AddData(1, N1); buffer.AddData(1, N2); buffer.AddData(1, N3);
		
		buffer.AddData(1, N3); buffer.AddData(1, N2); buffer.AddData(1, N4);
		
		buffer.AddData(2, T1); buffer.AddData(2, T2); buffer.AddData(2, T3);
		
		buffer.AddData(2, T3); buffer.AddData(2, T2); buffer.AddData(2, T4);
		
		buffer.AddIndex((i*n+j)*6+0, (i*n+j)*6+1, (i*n+j)*6+2);
		buffer.AddIndex((i*n+j)*6+3, (i*n+j)*6+4, (i*n+j)*6+5);
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

	// CircleK
	InitCircle();

	// Plane
		m_vb.AddAttribute(0, 3); // positions.
		m_vb.AddAttribute(1, 3); // normals.
		m_vb.AddAttribute(2, 2); // tex coords.

		// postions. (30x30 board)
		m_vb.AddData(0, -15, 0, -15);
		m_vb.AddData(0, -15, 0, 15);
		m_vb.AddData(0, 15, 0, -15);
		m_vb.AddData(0, 15, 0, 15);

		// normals.
		m_vb.AddData(1, 0, 1, 0);
		m_vb.AddData(1, 0, 1, 0);
		m_vb.AddData(1, 0, 1, 0);
		m_vb.AddData(1, 0, 1, 0);

		// tex coords.
		m_vb.AddData(2, 0, 0);
		m_vb.AddData(2, 0, 1);
		m_vb.AddData(2, 1, 0);
		m_vb.AddData(2, 1, 1);

		// indices
		m_vb.AddIndex(1, 2, 0);
		m_vb.AddIndex(2, 1, 3);

		// init buffers
		m_vb.InitBuffers();

	// Suzanne
		m_mesh = ObjParser::parse("Suzanne.obj");
		m_mesh->initBuffers();

	// Texture
		m_texture_wood = TextureFromFile("wood5.jpg");

	// ==== Ivan's code ===

	////m_vb.AddAttribute(0, 3); //positions
	////m_vb.AddAttribute(1, 3); //normals 
	////m_vb.AddAttribute(2, 2); // tex coords

	//////positions
	////m_vb.AddData(0, -10,  0, -10);
	////m_vb.AddData(0,  10,  0, -10);
	////m_vb.AddData(0, -10,  0,  10);
	////m_vb.AddData(0,  10,  0,  10);

	////// normals
	////m_vb.AddData(1, 0, 1, 0);
	////m_vb.AddData(1, 0, 1, 0);
	////m_vb.AddData(1, 0, 1, 0);
	////m_vb.AddData(1, 0, 1, 0);

	////// tex coords
	////m_vb.AddData(2, 0, 0);
	////m_vb.AddData(2, 1, 0);
	////m_vb.AddData(2, 0, 1);
	////m_vb.AddData(2, 1, 1);

	////m_vb.AddIndex(1, 0, 2);
	////m_vb.AddIndex(1, 2, 3);

	////m_vb.InitBuffers();
	////
	////// octahedron - to finish

	////m_vb_octa.AddAttribute(0, 3); //positions
	////m_vb_octa.AddAttribute(1, 3); //normals 
	////m_vb_octa.AddAttribute(2, 2); // tex coords
	////
	////add_triangle(
	////	glm::vec3(0, 0, 1),
	////	glm::vec3(1, 0, 0), 
	////	glm::vec3(0, 1, 0), 
	////	m_vb_octa);

	////m_vb_octa.AddIndex(0,1,2);

	////add_triangle(
	////	glm::vec3(0, 0, 1),
	////	glm::vec3(0, 1, 0),
	////	glm::vec3(-1, 0, 0), 
	////	m_vb_octa);

	////m_vb_octa.AddIndex(3,4,5);

	////add_triangle(
	////	glm::vec3(0, 1, 0),
	////	glm::vec3(1, 0, 0),
	////	glm::vec3(0, 0, -1),
	////	m_vb_octa
	////);
	////m_vb_octa.AddIndex(6, 7, 8);

	////add_triangle(
	////	glm::vec3(0, 1, 0),
	////	glm::vec3(0, 0, -1),
	////	glm::vec3(-1, 0, 0),
	////	m_vb_octa
	////);
	////m_vb_octa.AddIndex(9, 10, 11);

	////m_vb_octa.InitBuffers(); // IMPORTANT!!!! moves vertices to GPU
	////
	////// additional example: sphere
	////
	////m_vb_sphere.AddAttribute(0,3);
	////m_vb_sphere.AddAttribute(1,3);
	////m_vb_sphere.AddAttribute(2,2);
	////
	////genParametricSurface(sphereResolution, m_vb_sphere);
	////
	////m_vb_sphere.InitBuffers();

	//
	// load shaders	
	//
	m_program.AttachShader(GL_VERTEX_SHADER, "dirLight.vert");
	m_program.AttachShader(GL_FRAGMENT_SHADER, "dirLight.frag");

	m_program.BindAttribLoc(0, "vs_in_pos");
	m_program.BindAttribLoc(1, "vs_in_normal");
	m_program.BindAttribLoc(2, "vs_in_tex0");

	if ( !m_program.LinkProgram() )
	{
		return false;
	}

	//
	// misc init
	//

	m_camera.SetProj(45.0f, 640.0f/480.0f, 0.01f, 1000.0f);

	m_textureID = TextureFromFile("texture.png");
	ls = glm::vec4(1);

	/*m_mesh = ObjParser::parse("Suzanne.obj");
	m_mesh->initBuffers();*/

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
	float delta_time = (SDL_GetTicks() - last_time)/1000.0f;

	m_camera.Update(delta_time);

	last_time = SDL_GetTicks();
}

bool CMyApp::InitCircle()
{
	circle_resolution = 6;

	// init attributes
	m_vb_circle.AddAttribute(0, 3); // positions
	m_vb_circle.AddAttribute(1, 3); // normals
	m_vb_circle.AddAttribute(2, 2); // texture

	glm::vec3 normal(0, 1, 0);

	// init vertex data
	double r = 4;
	double h = 8;
	glm::vec3 center = glm::vec3(0, 0, 0);
	glm::vec3 top = glm::vec3(0, 8, 0);
	//m_vb_circle.AddData(0, center);
	m_vb_circle.AddData(0, top);
	m_vb_circle.AddData(1, normal);
	m_vb_circle.AddData(2, 0, 0);
	for (int i = 0; i < circle_resolution; ++i)
	{
		double angle = 2 * M_PI * i / circle_resolution;
		double u = r * cos(angle);
		double v = r * sin(angle);
		m_vb_circle.AddData(0, u, 0, v);
		m_vb_circle.AddData(1, normal);
		m_vb_circle.AddData(2, u, v);
	}
	// init indices
	for (int i = 0; i < circle_resolution; ++i)
	{
		//m_vb_circle.AddIndex(0, i + 1, i);
		m_vb_circle.AddIndex(
			0,
			1 + (i + 1) % circle_resolution, // because the last triangle is (0, circle_resolution, 1);
			i + 1);
	}
	// init buffer
	m_vb_circle.InitBuffers();
	return true;
}

void CMyApp::DrawCircle(const glm::mat4 &trans)
{
	m_program.On();
		//glm::mat4 matWorld = glm::mat4(1.0f)
		//	* glm::translate(glm::vec3(0, 10, 0))
		//	//* glm::rotate<float>(M_PI, glm::vec3(1, 0, 0))
		//	* glm::scale<float>(glm::vec3(
		//		1,
		//		1 + sin(SDL_GetTicks() / 500.0),
		//		1));
		glm::mat4 matWorld = glm::mat4(1.0f)
			* trans
			* glm::scale(glm::vec3(1, 1 + 0.5 * sin(SDL_GetTicks() / 1000.0), 1))
			;
		glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));
		glm::mat4 mvp = m_camera.GetViewProj() * matWorld;

		m_program.SetUniform("world", matWorld);
		m_program.SetUniform("worldIT", matWorldIT);
		m_program.SetUniform("MVP", mvp);
		m_program.SetUniform("eye_pos", m_camera.GetEye());
		//m_program.SetUniform("Kd", glm::vec4(0, 0, 0, 1));

		m_program.SetTexture("texImage", 0, GenTexture());

		m_vb_circle.On();
			m_vb_circle.DrawIndexed(GL_TRIANGLES, 0, circle_resolution * 3 /*3 is the number of vertices to draw a triangle*/, 0);
		m_vb_circle.Off();
	m_program.Off();
}

void CMyApp::DrawMesh(const glm::mat4 &trans)
{
	m_program.On();
	glm::mat4 matWorld = trans;
	glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));
	glm::mat4 mvp = m_camera.GetViewProj() *matWorld;

	m_program.SetUniform("world", matWorld);
	m_program.SetUniform("worldIT", matWorldIT);
	m_program.SetUniform("MVP", mvp);
	m_program.SetUniform("eye_pos", m_camera.GetEye());
	m_program.SetUniform("Kd", glm::vec4(0.75f, 0.25f, 0.125f, 1));

	m_program.SetTexture("texImage", 0, m_texture_wood);

	m_mesh->draw();
	m_program.Off();
}

#include <glm/gtx/string_cast.hpp> // std::cerr << glm::to_string(Matrix);

void CMyApp::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ===== Real code for sample test 
	glm::mat4 matWorld, matWorldIT, mvp;

	// Set Uniform light
	m_program.On();
		m_program.SetUniform("Ls", ls);
	m_program.Off();

	// Create HexagonalPyramid
	DrawCircle(
		glm::translate(glm::vec3(-15, 0, -15))
	);
	DrawCircle(
		glm::translate(glm::vec3(-15, 0, 15))
	);
	DrawCircle(
		glm::translate(glm::vec3(15, 0, -15))
	);
	DrawCircle(
		glm::translate(glm::vec3(15, 0, 15))
	);

	// Draw Suzanne
	DrawMesh(
		glm::translate(glm::vec3(0, 5 + sin(SDL_GetTicks() / 1000.0), 0))
		* glm::scale(glm::vec3(2, 2, 2))
	);
	
	// Create planequad.
	m_program.On();
		matWorld = glm::mat4(1.0f);
		matWorldIT = glm::transpose( glm::inverse( matWorld ) );
		mvp = m_camera.GetViewProj() *matWorld;

		m_program.SetUniform( "world", matWorld );
		m_program.SetUniform( "worldIT", matWorldIT );
		m_program.SetUniform( "MVP", mvp );
		m_program.SetUniform( "eye_pos", m_camera.GetEye() );
		//m_program.SetUniform("Kd", glm::vec4(0.75f, 0.25f, 0.125f, 1));
		double period = 2 * M_PI * SDL_GetTicks() / 5000.0;
		m_program.SetUniform("light_pos", glm::vec3(40 * cos(period), 40 * sin(period), 0));

		m_program.SetTexture("texImage", 0, m_textureID);

		m_vb.On();
			m_vb.DrawIndexed(GL_TRIANGLES, 0, 6, 0);
		m_vb.Off();
	m_program.Off();

	// Draw Suzanne 5
	double r = 10;
	double angle = SDL_GetTicks() / 1000.0;
	double angle1 = SDL_GetTicks() / 1000.0;
	for (int i = 0; i < 5; ++i)
	{
		double u = cos(2 * M_PI * i / 5);
		double v = sin(2 * M_PI * i / 5);
		DrawMesh(glm::mat4(1.0f)
			* glm::rotate<float>(angle, glm::vec3(0, 1, 0))
			* glm::translate(glm::vec3(r * u, 5 + sin(angle), r * v))
			* glm::rotate<float>(angle1, glm::vec3(0, 1, 0))
			//* glm::rotate<float>(M_PI / 2, glm::vec3(0, 0, 1))
			* glm::scale(glm::vec3(1, 1, 1) + glm::vec3(0.5 * sin(angle)))
		);
	}

	// ==========================

	//// draw object 1

	//m_program.On();

	//	glm::mat4 matWorld = glm::mat4(1.0f);
	//	glm::mat4 matWorldIT = glm::transpose( glm::inverse( matWorld ) );
	//	glm::mat4 mvp = m_camera.GetViewProj() *matWorld;

	//	m_program.SetUniform( "world", matWorld );
	//	m_program.SetUniform( "worldIT", matWorldIT );
	//	m_program.SetUniform( "MVP", mvp );
	//	m_program.SetUniform( "eye_pos", m_camera.GetEye() );
	//	m_program.SetUniform("Kd", glm::vec4(1, 0, 0, 1));

	//	m_program.SetTexture("texImage", 0, m_textureID);

	//	// draw with VAO
	//	m_vb.On();

	//		m_vb.DrawIndexed(GL_TRIANGLES, 0, 6, 0); //IB

	//	m_vb.Off();

	//m_program.Off();

	//// draw octa
	//m_program.On();

	//matWorld = glm::scale(glm::vec3(5));
	//matWorldIT = glm::transpose(glm::inverse(matWorld));
	//mvp = m_camera.GetViewProj() *matWorld;

	//m_program.SetUniform("world", matWorld);
	//m_program.SetUniform("worldIT", matWorldIT);
	//m_program.SetUniform("MVP", mvp);
	//m_program.SetUniform("eye_pos", m_camera.GetEye());

	//m_program.SetTexture("texImage", 0, m_textureID);
	//m_program.SetUniform("Kd", glm::vec4(0, 1, 0, 1));

	//// draw with VAO
	//m_vb_octa.On();

	//m_vb_octa.DrawIndexed(GL_TRIANGLES, 0, 12, 0); //IB

	//m_vb_octa.Off();

	//m_program.Off();

	//// draw object 2

	//m_program.On();

	//	matWorld = glm::translate<float>(glm::vec3(0, 1, 0));
	//	matWorldIT = glm::transpose( glm::inverse( matWorld ) );
	//	mvp = m_camera.GetViewProj() *matWorld;

	//	m_program.SetUniform( "world", matWorld );
	//	m_program.SetUniform( "worldIT", matWorldIT );
	//	m_program.SetUniform( "MVP", mvp );
	//	m_program.SetUniform( "eye_pos", m_camera.GetEye() );

	//	m_program.SetTexture("texImage", 0, m_textureID);

	//	m_mesh->draw();

	//m_program.Off();


	//// draw object 2
	//m_program.On();

	//matWorld = glm::translate<float>(glm::vec3(0, 9, 0));
	//matWorldIT = glm::transpose(glm::inverse(matWorld));
	//mvp = m_camera.GetViewProj() *matWorld;

	//m_program.SetUniform("world", matWorld);
	//m_program.SetUniform("worldIT", matWorldIT);
	//m_program.SetUniform("MVP", mvp);
	//m_program.SetUniform("eye_pos", m_camera.GetEye());

	//m_program.SetTexture("texImage", 0, m_textureID);

	//m_mesh->draw();

	//m_program.Off();
	//
	//// draw sphere
	//m_program.On();

	//matWorld = glm::translate<float>(glm::vec3(0, 6, 0)) * glm::rotate<float>(2*M_PI*SDL_GetTicks() / 5000.0f, glm::vec3(0,1,0));
	//matWorldIT = glm::transpose(glm::inverse(matWorld));
	//mvp = m_camera.GetViewProj() *matWorld;

	//m_program.SetUniform("world", matWorld);
	//m_program.SetUniform("worldIT", matWorldIT);
	//m_program.SetUniform("MVP", mvp);
	//m_program.SetUniform("eye_pos", m_camera.GetEye());
	//m_program.SetUniform("Kd", glm::vec4(0, 0, 1, 1));

	//m_program.SetTexture("texImage", 0, m_textureID);

	//// draw with VAO
	//m_vb_sphere.On();

	//m_vb_sphere.DrawIndexed(GL_TRIANGLES, 0, sphereResolution*sphereResolution*6, 0);
	//
	//m_vb_sphere.Off();

	//m_program.Off();

	//// draw circle
	//DrawCircle();
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
	switch (key.keysym.sym)
	{
	case '1':
		m_camera.SetView(glm::vec3(-15, 8, -15), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		break;
	case '2':
		m_camera.SetView(glm::vec3(-15, 8, 15), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		break;
	case '3':
		m_camera.SetView(glm::vec3(15, 8, -15), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		break;
	case '4':
		m_camera.SetView(glm::vec3(15, 8, 15), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		break;
	case 'r':
		ls = glm::vec4(1, 0, 0, 1);
		break;
	case 'g':
		ls = glm::vec4(0, 1, 0, 1);
		break;
	case 'b':
		ls = glm::vec4(0, 0, 1, 1);
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
