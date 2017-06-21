#include "sdl_window.h"
#include "vtracer.h"

#include <iomanip>

using std::cout;
using std::endl;

int main ()
{
	//SDLWindow::Window* const window = new SDLWindow::Window (800, 600);
	vtracer::VTracer* const vtracer = new vtracer::VTracer(800, 600);

	if (!vtracer->init ())
		return 1;

	// main game loop
	while (vtracer->process()) { }

	// dispose
	delete vtracer;

	return 0;
}