# computer_graphics
cg.elte.hu
vision.sztaki.hu/~danini

1 exam 4hrs?

Click on project name -> Properties -> VC++ Directories -> (Include + Lib) directories -> Debugging -> Env

These lines will force VS use my NVIDIA VGA instead of Intel.
```
#include <windows.h>
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
```

Write email for confirming plus point. +1


Daniel Barath
majti89@gmail.com

Lab 2:
- Double buffering.
- (Evan) GDC conference => Lecture, video Youtube..

Lab 4:
- Since the appearance of z-buffer. If you don't see the color when drawing the triangle, swap the order of 2 points
(It will flip the triangle, because you probably just saw the back face of it).

============================
What is myFrag and myVert????????