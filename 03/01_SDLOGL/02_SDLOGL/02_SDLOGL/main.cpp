#include <SDL.h>
#include <SDL_opengl.h>
#include <iostream>
#include <sstream>

void exitProgram()
{
	SDL_Quit();

	std::cout << "Press any key to continue..." << std::endl;
	std::cin.get();
}

int main( int argc, char* args[] )
{
	atexit( exitProgram );

	//
	// 1. step: ininitalize SDL
	//

	// initialize the graphical subsystem only
	if ( SDL_Init( SDL_INIT_VIDEO ) == -1 )
	{
		std::cout << "[SDL_Init]: " << SDL_GetError() << std::endl;
		return 1;
	}
			
	//
	// 2. step: specify the requirements regarding the OpenGL context
	//

	// 2a: configure OpenGL launch - has to be done prior to the creation of the window

	// we can explicitly set the OpenGL version, e.g. to 3.2:
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	// set color buffer bit resolution
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,         32);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,            8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,          8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,           8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,          8);
	// request double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,		1);
	// set depth buffer bit depth
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,          24);
	
	// create a window we can draw to
	SDL_Window *win = 0;
	win = SDL_CreateWindow( "Hello SDL&OpenGL!",				// caption of the window
							100,						// the initial X coordinate of the window
							100,						// the initial Y coordinate of the window
							640,						// width of the window's client area
							480,						// height of the window's client area
							SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);			// display properties


	// should the window creation fail, notify the user
    if (win == 0)
	{
		std::cout << "[SDL_CreateWindow]: " << SDL_GetError() << std::endl;
        return 1;
    }

	//
	// 3. step: create a renderer
	//

	SDL_GLContext	context	= SDL_GL_CreateContext(win);
    if (context == 0)
	{
		std::cout << "[SDL_GL_CreateContext]: " << SDL_GetError() << std::endl;
        return 1;
    }	

	// wait for vsync
	SDL_GL_SetSwapInterval(1);

	// query the OpenGL version we are running on
	int glVersion[2] = {-1, -1}; 
	glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]); 
	glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]); 
	std::cout << "Running OpenGL " << glVersion[0] << "." << glVersion[1] << std::endl;

	if ( glVersion[0] == -1 && glVersion[1] == -1 )
	{
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow( win );

		std::cout << "[OGL context creation]" << std::endl;

		return 1;
	}

	std::stringstream window_title;
	window_title << "OpenGL " << glVersion[0] << "." << glVersion[1];
	SDL_SetWindowTitle(win, window_title.str().c_str());

	//
	// 3. step: start the main loop
	// 

	// do we have to quit?
	bool quit = false;
	// the message of the OS
	SDL_Event ev;
	// the X and Y coordinates of the mouse
	Sint32 mouseX = 0, mouseY = 0;
	float r, g, b;
	while (!quit)
	{
		while ( SDL_PollEvent(&ev) )
		{
			switch (ev.type)
			{
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				if ( ev.key.keysym.sym == SDLK_ESCAPE )
					quit = true;
				break;
			case SDL_MOUSEMOTION:
				mouseX = ev.motion.x;
				mouseY = ev.motion.y;
				r = 1.0 - mouseX * 1.0 / 640;
				g = 1.0 - mouseY * 1.0 / 480;
				b = 1.0 - mouseY * 1.0 / 480;
				break;
			}
		}

		// clear the background
		glClearColor ( r, g, b, 1.0f );

		glClear ( GL_COLOR_BUFFER_BIT );

		SDL_GL_SwapWindow(win);
	}



	//
	// 4. step: clean up
	// 

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow( win );

	return 0;
}