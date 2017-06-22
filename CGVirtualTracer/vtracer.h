#pragma once
#include "sdl_window.h"

namespace vtracer
{
	class VTracer
	{
	public:
		VTracer(const __int16 width, const __int16 height);
		~VTracer();
		bool init() const;
		bool process();
		void draw_pixel(Uint16&, Uint16&) const;
		void render() const;
		void mouse_position(int&, int&) const;
		void clear_screen() const;

		// setters
		void set_pixel_size(const Uint16) const;
	private:
		SDLWindow::Window* window = nullptr;
		Uint32* buffer = nullptr;
		mutable Uint16 pixel_size;

	private:
		void find_pixel(Uint16&, Uint16&) const;
	};
}
