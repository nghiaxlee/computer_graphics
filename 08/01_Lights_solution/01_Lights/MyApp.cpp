#include "MyApp.h"
#include "GLUtils.hpp"

#include <GL/GLU.h>
#include <math.h>

CMyApp::CMyApp(void)
{
	m_vaoID = 0;
	m_vboID = 0;
	m_programID = 0;
	m_textureID = 0;

	m_angleY = 0;
}


CMyApp::~CMyApp(void)
{
}


GLuint CMyApp::GenTexture()
{
    unsigned char tex[256][256][3];
 
    for (int i=0; i<256; ++i)
        for (int j=0; j<256; ++j)
        {
			tex[i][j][0] = rand()%256;
			tex[i][j][1] = rand()%256;
			tex[i][j][2] = rand()%256;
        }
 
	GLuint tmpID;

    glGenTextures(1, &tmpID);
    glBindTexture(GL_TEXTURE_2D, tmpID);
    gluBuild2DMipmaps(  GL_TEXTURE_2D, 
						GL_RGB8, 256, 256,			// define storage on the GPU: RGB, 8bits per channel, 256x256 texture
						GL_RGB,	GL_UNSIGNED_BYTE,	// define storate in RAM: RGB layout, unsigned bytes for each channel
						tex);						//						  and source pointer
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// bilinear filter on min and mag
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glBindTexture(GL_TEXTURE_2D, 0);

	return tmpID;
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

	// geometry in RAM
	Vertex vert[] =
	{ 
		//           x,  y,  z             nx,ny,nz			 s, t
		{glm::vec3(-10,  0, -10), glm::vec3(0, 1, 0), glm::vec2(0, 0)},
		{glm::vec3( 10,  0, -10), glm::vec3(0, 1, 0), glm::vec2(1, 0)},
		{glm::vec3(-10,  0,  10), glm::vec3(0, 1, 0), glm::vec2(0, 1)},
		{glm::vec3( 10,  0,  10), glm::vec3(0, 1, 0), glm::vec2(1, 1)},
	};

	// index data in RAM
    GLushort indices[]=
    {
        1,0,2,
        1,2,3,
    };

	// create VAO, VBO, upload data to GPU, and specify storage and indices
	glGenVertexArrays(1, &m_vaoID);
	glBindVertexArray(m_vaoID);
	
	glGenBuffers(1, &m_vboID); 
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
	glBufferData( GL_ARRAY_BUFFER,sizeof(vert),	vert,	GL_STATIC_DRAW);	
	
	// attribute 0: 3 floats, right at the beginning of the VBO with a stride of sizeof(Vertex)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),	0); 

	// attribute 0: 1 floats, with an offset of 3*sizeof(float) from the beginning of the VBO with a stride of sizeof(Vertex)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,
		3, 
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(sizeof(glm::vec3)) );

	// attribute 2: 2 floats, with an offset of 5*sizeof(float) from the beginning of the VBO with a stride of sizeof(Vertex)
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2*sizeof(glm::vec3)) );

	// index buffer
	glGenBuffers(1, &m_ibID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//
	// load shaders
	//
	GLuint vs_ID = loadShader(GL_VERTEX_SHADER,		"myVert.vert");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER,	"myFrag.frag");

	m_programID = glCreateProgram();

	glAttachShader(m_programID, vs_ID);
	glAttachShader(m_programID, fs_ID);

	// bind the VAO attributes to the in variables of the vertex shader
	glBindAttribLocation(	m_programID,	// in our shader
							0,				// bind VAO attribute 0
							"vs_in_pos");	// to shader in variable vs_in_pos
	glBindAttribLocation( m_programID, 1, "vs_in_normal");
	glBindAttribLocation( m_programID, 2, "vs_in_tex0");

	glLinkProgram(m_programID);

	// check link
	GLint infoLogLength = 0, result = 0;

	glGetProgramiv(m_programID, GL_LINK_STATUS, &result);
	glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( GL_FALSE == result )
	{
		std::vector<char> ProgramErrorMessage( infoLogLength );
		glGetProgramInfoLog(m_programID, infoLogLength, NULL, &ProgramErrorMessage[0]);
		fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
		
		char* aSzoveg = new char[ProgramErrorMessage.size()];
		memcpy( aSzoveg, &ProgramErrorMessage[0], ProgramErrorMessage.size());

		std::cout << "[app.Init()] Sáder Huba panasza: " << aSzoveg << std::endl;

		delete aSzoveg;
	}

	glDeleteShader( vs_ID );
	glDeleteShader( fs_ID );

	//
	// misc. init
	//

	m_matProj = glm::perspective( 45.0f, 640/480.0f, 1.0f, 1000.0f );

	// query uniform shader variable locations
	m_loc_world = glGetUniformLocation( m_programID, "world");
	m_loc_worldIT = glGetUniformLocation( m_programID, "worldIT");
	m_loc_mvp  = glGetUniformLocation( m_programID, "MVP" );
	m_loc_texture = glGetUniformLocation( m_programID, "img" );

	// textures
	m_textureID = TextureFromFile("texture.png");
	m_modelTextureID = TextureFromFile("space_frigate.jpg");

	// load mesh
	m_mesh = ObjParser::parse("space_frigate.obj");
	m_mesh->initBuffers();

	return true;
}

void CMyApp::Clean()
{
	glDeleteBuffers(1, &m_vboID);
	glDeleteBuffers(1, &m_ibID);
	glDeleteVertexArrays(1, &m_vaoID);
	glDeleteTextures(1, &m_textureID);

	glDeleteProgram( m_programID );
}

void CMyApp::Update()
{
	m_angleY = -(SDL_GetTicks() / 2000.0);
	// nézeti transzformáció beállítása
	m_eye = glm::vec3( 
		15*cosf( m_angleY ) , 15, 15*sinf( m_angleY ) );

	m_matView = glm::lookAt(m_eye,						// camera position
							glm::vec3( 0,  0,  0),		// look at point
							glm::vec3( 0,  1,  0));		// up direction
}

void CMyApp::DrawMesh(glm::mat4 transformation,
	Mesh *mesh,
	GLuint texture)
{
	glUseProgram(m_programID);

	glm::mat4 mvp = m_matProj * m_matView * transformation;

	glUniformMatrix4fv(m_loc_mvp, 1, GL_FALSE, &(mvp[0][0]));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(m_loc_texture, 0);

	mesh->draw();

	glUseProgram(0);
}

void CMyApp::Render()
{
	// clear the background and z-buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader on
	glUseProgram( m_programID );

	// set uniform variable values
	float angle = 0;// SDL_GetTicks() / 1000.0;
	float c = cos(angle), s = sin(angle);
	glm::vec3 light_pos = glm::vec3(5*c, 2, 5*s);
	static auto loc = glGetUniformLocation(m_programID, "light_pos");
	glUniform3f(loc, 
		light_pos.x, 
		light_pos.y, 
		light_pos.z);

	static auto loc_eye = glGetUniformLocation(m_programID, "eye");
	glUniform3f(
		loc_eye,
		m_eye.x, m_eye.y, m_eye.z);

	// first compute their values
	glm::mat4 matWorld = glm::mat4(1.0f);
	glm::mat4 matWorldIT = glm::transpose( glm::inverse( matWorld ) );

	// then send those down to the GPU
	glUniformMatrix4fv( m_loc_world, 1, GL_FALSE, &( matWorld[0][0]) ); 
	glUniformMatrix4fv( m_loc_worldIT, 1, GL_FALSE, &( matWorldIT[0][0]) ); 
	glm::mat4 mvp = m_matProj*m_matView*matWorld;
	glUniformMatrix4fv( m_loc_mvp,  1, GL_FALSE, &( mvp[0][0]) );

	// activate HW sampler unit 0
	glActiveTexture(GL_TEXTURE0);
	// bind HW sampler unit 0 to m_textureID 
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	// make the shader's sampler query HW sampler unit 0
	glUniform1i(m_loc_texture, 0);				

	// turn VAO on and draw
	glBindVertexArray(m_vaoID);

	glDrawElements(	GL_TRIANGLES,		// a triangle list
					6,					// with 6 vertices
					GL_UNSIGNED_SHORT,	// with unsigned short indices
					0);					// from the first index

	// VAO off
	glBindVertexArray(0);

	// texture off
	glBindTexture(GL_TEXTURE_2D, 0);

	// shader off
	glUseProgram( 0 );


	// Transformations regarding to the loaded mesh
	glm::mat4 trans = glm::translate<float>(glm::vec3(0, 2, 0)); // 2nd transformation applied
	trans = trans * glm::scale<float>(glm::vec3(0.3, 0.3, 0.3)); // 1st transformation applied

	DrawMesh(trans, m_mesh, m_modelTextureID); // Drawing the loaded mesh
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

void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);

	m_matProj = glm::perspective(  45.0f,		
									_w/(float)_h,	
									0.01f,			
									100.0f);		
}