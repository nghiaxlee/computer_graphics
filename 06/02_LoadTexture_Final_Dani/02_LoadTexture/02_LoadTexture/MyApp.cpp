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
}


CMyApp::~CMyApp(void)
{
}

GLuint CMyApp::GenTexture()
{
    unsigned char tex[256][256][3];
 
	int rows = 8;
	int cols = 8;
	float cell_height = 256 / rows;
	float cell_width = 256 / cols;

    for (int i = 0; i < 256; ++i)
        for (int j = 0; j < 256; ++j)
        {
			tex[i][j][0] = rand() % 256; // Red channel
			tex[i][j][1] = rand() % 256; // Green channel
			tex[i][j][2] = rand() % 256; // Blue channel
        }
 
	GLuint tmpID;

	// generate a texture resource
    glGenTextures(1, &tmpID);
	// make it active
    glBindTexture(GL_TEXTURE_2D, tmpID);
	// fill the texture with data and build mip-maps
    gluBuild2DMipmaps(  GL_TEXTURE_2D,	// which resource to fill
						GL_RGB8,		// use 8 bits for each of the Red, Green, and Blue channels
						256, 256,		// make the texture 256x256
						GL_RGB,				// the source of the texture (the data with which the texture will be initialized) stores RGB values
						GL_UNSIGNED_BYTE,	//  8 bits used for each channel
						tex);				//  and the location of the data is here
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// bilinear filtering for min filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// bilinear filtering for mag filter
	glBindTexture(GL_TEXTURE_2D, 0);

	return tmpID;
}


bool CMyApp::Init()
{
	// blue-ish clear color
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE); // backface culling: ON
	glEnable(GL_DEPTH_TEST); // depth buffer: ON
	glCullFace(GL_BACK); // GL_BACK: cull back faces

	//
	// create geometry data
	//

	Vertex vert[] =
	{ 
		//          x,  y, z             R, G, B			 s, t
		{glm::vec3(-1, -1, 0), glm::vec3(1, 0, 0), glm::vec2(0, 0)},
		{glm::vec3( 1, -1, 0), glm::vec3(0, 1, 0), glm::vec2(1, 0)},
		{glm::vec3(-1,  1, 0), glm::vec3(0, 0, 1), glm::vec2(0, 1)},
		{glm::vec3( 1,  1, 0), glm::vec3(1, 1, 1), glm::vec2(1, 1)},
	};

	// create index buffer data
    GLushort indices[]=
    {
		// 1st triangle
        0,1,2,
		// 2nd triangle
        2,1,3,
    };

	// create a VAO, create a VBO and an index buffer and fill them
	glGenVertexArrays(1, &m_vaoID);
	glBindVertexArray(m_vaoID);
	
	// VBO creation and initialization
	glGenBuffers(1, &m_vboID); 
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID); 
	glBufferData( GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

	// 0: position channel
	glEnableVertexAttribArray(0); 
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0); 

	// 1: color channel
	glEnableVertexAttribArray(1); 
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)) );

	// 2: texture coordinate channel
	glEnableVertexAttribArray(2); 
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2*sizeof(glm::vec3)) );

	// index buffer creation and initialization
	glGenBuffers(1, &m_ibID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// we are done setting up the VAO, VBO, and element array object buffers, turn them off
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//
	// load the shaders
	//
	GLuint vs_ID = loadShader(GL_VERTEX_SHADER,		"myVert.vert");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER,	"myFrag.frag");

	// create the shader program, attach shaders, bind attribute locations, link, verify log, etc.
	m_programID = glCreateProgram();

	glAttachShader(m_programID, vs_ID);
	glAttachShader(m_programID, fs_ID);

	glBindAttribLocation( m_programID, 0, "vs_in_pos");
	glBindAttribLocation( m_programID, 1, "vs_in_col");
	glBindAttribLocation( m_programID, 2, "vs_in_tex0");

	glLinkProgram(m_programID);

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

		std::cout << "[app.Init()] Error: " << aSzoveg << std::endl;

		delete aSzoveg;
	}

	glDeleteShader( vs_ID );
	glDeleteShader( fs_ID );

	//
	// rest of init
	//
	m_matProj = glm::perspective( 45.0f, 640/480.0f, 1.0f, 1000.0f );

	m_loc_mvp = glGetUniformLocation( m_programID, "MVP");
	m_loc_texture = glGetUniformLocation( m_programID, "texture" );
	
	// generate texture
	m_textureID = GenTexture();

	// load texture from file
	m_waterTextureID = TextureFromFile("texture.bmp");

	return true;
}

void CMyApp::Clean()
{
	glDeleteTextures(1, &m_textureID);

	glDeleteBuffers(1, &m_vboID);
	glDeleteVertexArrays(1, &m_vaoID);

	glDeleteProgram( m_programID );
}

void CMyApp::Update()
{
	// set up view matrix
	m_matView = glm::lookAt(glm::vec3( 0,  0,  5),		// eye
							glm::vec3( 0,  0,  0),		// look at
							glm::vec3( 0,  1,  0));		// up
}


void CMyApp::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 transform_1 = glm::translate<float>(glm::vec3(1, 1, 0));
	glm::mat4 transform_2 = glm::translate<float>(glm::vec3(sin(SDL_GetTicks() / 375.0), 1, 0));

	DrawTexturedPlane(m_waterTextureID, transform_2);
	DrawTexturedPlane(m_textureID, transform_1);
}

void CMyApp::DrawTexturedPlane(GLuint texture_id, 
	glm::mat4 transformation)
{
	glUseProgram(m_programID);
	static float alpha = glGetUniformLocation(m_programID, "alpha");
	glUniform1f(alpha, (sin(SDL_GetTicks() / 1000.0) + 1) / 2.0);

	// world transform
	m_matWorld = transformation;

	glm::mat4 mvp = m_matProj * m_matView * m_matWorld;

	glUniformMatrix4fv(m_loc_mvp, 1, GL_FALSE, &(mvp[0][0]));
	
	// activate texture sampler 0
	glActiveTexture(GL_TEXTURE0);
	// activate texture on sampler0
	glBindTexture(GL_TEXTURE_2D, texture_id);
	// bind shader sampler to sampler 0
	glUniform1i(m_loc_texture,	// bind to this sampler2D in the shader
		0);				// this texture sampler unit

	glBindVertexArray(m_vaoID);

	glDrawElements(GL_TRIANGLES,		// primitive type
		6,					// number of vertices used for rendering
		GL_UNSIGNED_SHORT,	// index type
		0);					// index location

	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);
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

	m_matProj = glm::perspective(  45.0f,			// fovy/2
									_w/(float)_h,	// width:height ration
									0.01f,			// near clipping plane
									100.0f);		// far clipping plane
}