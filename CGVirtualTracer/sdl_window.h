#pragma once

#include <SDL.h>
#include <iostream>

#undef main

namespace SDLWindow {

	enum struct COLOR
	{
		RED,
		GREEN,
		BLUE,
		WHITE,
		BLACK,
		GREY
	};


	class EventProcesser
	{
		friend class Window;
	private:
		virtual bool process (const SDL_Event& e) const
		{
			if (e.type == SDL_QUIT)
			{
				return false;
			}

			return true;
		}
	};

	class Window
	{
	private:
		__int16 width;
		__int16 height;
		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;
		SDL_Texture* texture = nullptr;
		Uint32* buffer = nullptr;

		const EventProcesser* event_pr = nullptr;
	private:
		Uint32 get_rgb(const COLOR);
	public:
		Window (const __int16, const __int16);
		~Window ();
		bool init ();
		void render ();
		void register_event_processer (const EventProcesser const*);
		bool process ();

		// pixel manipulation
		// Draws
		void draw_pixel(const COLOR, const __int16, const __int16);
		void draw_pixel(const COLOR, const __int16, const __int16, const __int16);

		// getters
		Uint32* const get_buffer() const;
		__int16 get_width();
		__int16 get_height();

		// mouse
		void mouse_position(int&, int&) const;
	};
}