#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

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
		// if it fails, signal error and exit
		std::cout << "[SDL_Init]: " << SDL_GetError() << std::endl;
		return 1;
	}
			
	//
	// 2. step: create a window we can draw to
	//

	SDL_Window *win = 0;
    win = SDL_CreateWindow( "Hello SDL!",				// caption of the window
							100,						// the initial X coordinate of the window
							100,						// the initial Y coordinate of the window
							640,						// width of the window's client area
							480,						// height of the window's client area
							SDL_WINDOW_SHOWN);			// display properties

	// should the window creation fail, notify the user
    if (win == 0)
	{
		std::cout << "[SDL_CreateWindow]: " << SDL_GetError() << std::endl;
        return 1;
    }

	//
	// 3. step: create a renderer
	//

    SDL_Renderer *ren = 0;
    ren = SDL_CreateRenderer(	win, // pointer to the window onto which the renderer should draw
								-1,  // the index of the renderer, if -1: the first renderer fitting our needs specified in the last parameter of this function call
									 // a -1 a harmadik paraméterben meghatározott igényeinknek megfelelõ elsõ renderelõt jelenti
								SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);	// we want a HW accelerated, vsync-ed renderer
    if (ren == 0)
	{
        std::cout << "[SDL_CreateRenderer]: " << SDL_GetError() << std::endl;
        return 1;
    }

	//
	// 3. step: load the image file
	//
	SDL_Texture* tex = IMG_LoadTexture( ren, "animation_sheet.gif" );
	if ( tex == 0 )
	{
        std::cout << "[IMG_LoadTexture]: " << IMG_GetError() << std::endl;
        return 1;
	}


	//
	// 4. step: start the main loop
	// 

	// do we have to quit?
	bool quit = false;
	// the message of the OS
	SDL_Event ev;
	// the X and Y coordinates of the mouse
	Sint32 mouseX = 0, mouseY = 0;
	int idx, idy;
	idx = idy = 0;
	int pos_x, pos_y, vel_x, vel_y;
	pos_x = pos_y = 0;
	vel_x = vel_y = 1;
	SDL_RendererFlip flip_flag = SDL_FLIP_NONE;

	while (!quit)
	{
		// process all pending messages in the application message queue
		while ( SDL_PollEvent(&ev) )
		{
			switch (ev.type)
			{
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				switch (ev.key.keysym.sym)
				{
				case SDLK_LEFT:
					if (vel_x > 0)
						vel_x = -vel_x;
					flip_flag = SDL_FLIP_HORIZONTAL;
					break;
				case SDLK_RIGHT:
					if (vel_x < 0)
						vel_x = -vel_x;
					flip_flag = SDL_FLIP_NONE;
					break;
				case SDLK_UP:
					if (vel_y > 0)
						vel_y = -vel_y;
					break;
				case SDLK_DOWN:
					if (vel_y < 0)
						vel_y = -vel_y;
					break;
				case SDLK_ESCAPE:
					quit = true;
					break;
				}
				break;
			case SDL_MOUSEMOTION:
				mouseX = ev.motion.x;
				mouseY = ev.motion.y;
				break;
			case SDL_MOUSEBUTTONUP:
				// mouse messages: SDL_BUTTON_LEFT, SDL_BUTTON_MIDDLE, 
				//		SDL_BUTTON_RIGHT, SDL_BUTTON_WHEELUP, SDL_BUTTON_WHEELDOWN
				break;
			}
		}

		// clear the background with white
		SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
		SDL_RenderClear(ren);


		SDL_Rect image_sized_rect;
		image_sized_rect.x = 0;
		image_sized_rect.y = 0;
		SDL_QueryTexture(tex, 0, 0, &image_sized_rect.w, &image_sized_rect.h);

		SDL_Rect src_rect, dst_rect;
		int frame_index = (SDL_GetTicks() / 50) % 30;
		
		int body_w = image_sized_rect.w / 6;
		int body_h = image_sized_rect.h / 5;

		idx = frame_index % 6;
		idy = frame_index / 6;

		//std::cout << idx << ' ' << idy << '\n';
		src_rect.x = idx * body_w;
		src_rect.y = idy * body_h;
		src_rect.w = body_w;
		src_rect.h = body_h;

		// Update pos
		pos_x = pos_x + vel_x;
		pos_y = pos_y + vel_y;

		if (pos_y > 480 - body_h && vel_y > 0)
		{
			vel_y = -vel_y;
		}

		if (pos_y < 0 && vel_y < 0)
		{
			vel_y = -vel_y;
		}

		if (pos_x > 640 - body_w && vel_x > 0)
		{
			vel_x = -vel_x;
			flip_flag = SDL_FLIP_HORIZONTAL;
		}

		if (pos_x < 0 && vel_x < 0)
		{
			vel_x = -vel_x;
			flip_flag = SDL_FLIP_NONE;
		}
		
		dst_rect.x = pos_x;
		dst_rect.y = pos_y;
		dst_rect.w = src_rect.w;
		dst_rect.h = src_rect.h;

		SDL_Point body_center;
		body_center.x = dst_rect.x + dst_rect.w >> 1;
		body_center.y = dst_rect.y + dst_rect.h >> 1;

		//std::cout << vel_x << ' ' << vel_y << '\n';

		SDL_RenderCopyEx(ren, tex, &src_rect, &dst_rect, 0, &body_center, flip_flag);

		//// draw the image centered at the mouse cursor
		//SDL_Rect image_rect;
		//image_rect.x = mouseX;
		//image_rect.y = mouseY;
		//image_rect.w = image_sized_rect.w;
		//image_rect.h = image_sized_rect.h;

		//SDL_RenderCopy( ren,				// which renderer to use
		//				tex,				// which texture to draw
		//				&image_sized_rect,	// source rect (which part of the texture to draw)
		//				&image_rect);		// target rect

		// display the contents of the backbuffer
		SDL_RenderPresent(ren);
	}



	//
	// 4. step: clean up
	// 


	SDL_DestroyTexture( tex );
	SDL_DestroyRenderer( ren );
	SDL_DestroyWindow( win );

	return 0;
}