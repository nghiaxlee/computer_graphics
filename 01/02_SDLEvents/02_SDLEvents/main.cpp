#include <SDL.h>
#include <iostream>
#include <string>
#include <string.h>

void exitProgram()
{
	std::cout << "Press any key to exit..." << std::endl;
	std::cin.get();
}

int main( int argc, char* args[] )
{
	//
	// 1.: initilize the video subsystem of SDL
	//

	if (SDL_Init(SDL_INIT_VIDEO) == -1)
	{
		std::cout << "[SDL launch]" << SDL_GetError() << std::endl;
		return 1;
	}

	//
	// 2.: create a window with SDL
	//

	SDL_Window *win = 0;	// identifier (!) of the window to be created
	win = SDL_CreateWindow("Hello SDL!",				// caption of the window
		100,						// initial X
		100,						//	   and Y coordinates (wrt. desktop)
		640,						// width
		480,						// height (both in pixels)
		SDL_WINDOW_SHOWN);			// creation flags

	if (win == 0)
	{
		std::cout << "[Window] " << SDL_GetError() << std::endl;
		return 1;
	}

	//
	// 3.: create a renderer for the window. The renderer can be used to draw on the window
	//

	SDL_Renderer *ren = 0; // the identifier of the renderer
	ren = SDL_CreateRenderer(win, // the window onto which the renderer draws
							-1,  // select the first supported renderer that...
							SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);	// ...satisfies these conditions

	if (ren == 0)
	{
		std::cout << "[Renderer]: " << SDL_GetError() << std::endl;
		return 1;
	}

	//
	// 4.: The Loop
	// 

	bool quit = false;
	SDL_Event ev;
	Sint32 mouseX = 0, mouseY = 0;
	int rec_col = 1;
	int frameID = 0;

	int is_it_already_a_sec = 0;
	int last_time = SDL_GetTicks();
	std::string FPS_string;

	while (!quit)
	{
		int curr_time = SDL_GetTicks();
		int delta = curr_time - last_time;
		last_time = curr_time;
		frameID++;

		is_it_already_a_sec += delta;
		if (is_it_already_a_sec >= 1000)
		{
			FPS_string = std::to_string(frameID);
			frameID = 0;
			is_it_already_a_sec /= 1000;
		}

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
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (ev.button.button == SDL_BUTTON_LEFT)
					rec_col = 2;
				else if (ev.button.button == SDL_BUTTON_RIGHT)
					rec_col = 3;
				break;
			case SDL_MOUSEBUTTONUP:
				// buttons: SDL_BUTTON_LEFT, SDL_BUTTON_MIDDLE, 
				//		SDL_BUTTON_RIGHT, SDL_BUTTON_WHEELUP, SDL_BUTTON_WHEELDOWN
				rec_col = 1;
				break;
			}
		}
		//SDL_Delay(2000);

		// clear the screen with white
		SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
		SDL_RenderClear(ren);

		// draw a green line
		SDL_SetRenderDrawColor(	ren, 0, 255, 0,	 255);	
		SDL_RenderDrawLine(	ren, 0, 0, mouseX, mouseY);
		
		// and a red rectangle
		SDL_Rect cursor_rect;
		cursor_rect.x = mouseX - 10;
		cursor_rect.y = mouseY - 10;
		cursor_rect.w = 20;
		cursor_rect.h = 20;

		SDL_SetWindowTitle(win, FPS_string.c_str());

		if (rec_col == 1)
			SDL_SetRenderDrawColor( ren, 255, 0, 0, 255 );
		if (rec_col == 2)
			SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
		if (rec_col == 3)
			SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
		SDL_RenderFillRect( ren, &cursor_rect);

		// Task: Draw Circle.
		// Polar -> Cartesian.
		const int N = 6;
		SDL_Point p[N];
		double radius = 50;
		for (int i = 0; i < N; ++i)
		{

			double phi = 2 * M_PI * i / (N - 1);
			//double phi = i / (M_PI * 2);
			p[i].x = mouseX + radius * cos(phi);
			p[i].y = mouseY + radius * sin(phi);
		}
		SDL_RenderDrawLines(ren, p, N);

		// present the current frame
		SDL_RenderPresent(ren);
	}



	//
	// 5. exit
	// 

	SDL_DestroyRenderer( ren );
	SDL_DestroyWindow( win );

	SDL_Quit();

	return 0;
}