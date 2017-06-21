#include "sdl_window.h"

namespace SDLWindow
{

#pragma region Macros
#define del_null(x) { delete x; x = nullptr; }
#pragma endregion

	Window::Window(const __int16 width, const __int16 height) :
		width(width),
		height(height)
	{
		std::cout << "Created SDL window!" << std::endl;
	}

	Window::~Window()
	{
		SDL_DestroyRenderer(renderer);
		SDL_DestroyTexture(texture);
		SDL_DestroyWindow(window);

		del_null(buffer);

		SDL_Quit();
	}

	bool Window::init()
	{
		if (SDL_Init(SDL_INIT_VIDEO) != 0)
		{
			return false;
		}

		window = SDL_CreateWindow("CG Virtual Tracer",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			width,
			height,
			SDL_WINDOW_SHOWN);

		if (window == nullptr)
		{
			return false;
		}

		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

		if (renderer == nullptr)
		{
			return false;
		}

		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_STATIC, width, height);

		if (texture == nullptr)
		{
			return false;
		}

		buffer = new Uint32[width * height];
		memset(buffer, 133, width * height * sizeof(Uint32));

		// TODO clean up when done
		std::cout << "size of buffer: " << sizeof(buffer) << "B, " << 
			std::fixed << (sizeof(buffer) * 1e-6) << "MB" << std::endl;
		std::cout << "Suppose we have 1000 textures: " << (sizeof(buffer) * 1e-6) * 1000 << std::endl;

		render();

		return true;
	}

	void Window::render()
	{
		SDL_UpdateTexture(texture, NULL, buffer, width * sizeof(Uint32));
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

	void Window::register_event_processer(const EventProcesser const* e)
	{
		this->event_pr = e;
	}

	bool Window::process()
	{
		SDL_Event event;
		SDL_PollEvent(&event);

		return event_pr->process(event);
	}

	Uint32* const Window::get_buffer() const
	{
		return this->buffer;
	}

	__int16 Window::get_width()
	{
		return this->width;
	}

	__int16 Window::get_height()
	{
		return this->height;
	}

#pragma region PixelManipulation
	Uint32 Window::get_rgb(const COLOR c)
	{
		Uint8 r = 0, g = 0, b = 0;
		Uint32 color = 0;

		switch (c)
		{
		case COLOR::RED:
			r = 255;
			break;
		case COLOR::BLUE:
			b = 255;
			break;
		case COLOR::GREEN:
			g = 255;
			break;
		case COLOR::GREY:
			r = 125;
			g = 125;
			b = 125;
			break;
		default:
			r = 255;
			g = 255;
			b = 255;
			break;
		}

		color += r;
		color <<= 8;
		color += g;
		color <<= 8;
		color += b;
		color <<= 8;

		return color;
	}

	void Window::draw_pixel(const COLOR color, const __int16 x, const __int16 y)
	{
		buffer[(y * width) + x] = this->get_rgb(color);
	}

	void Window::draw_pixel(const COLOR color, const __int16 pos_x, const __int16 pos_y, const __int16 size)
	{
		__int32 _color = this->get_rgb(color);

		for (int y = pos_y; y < pos_y + size; y++)
		{
			for (int x = pos_x; x < size + pos_x; x++)
			{
				buffer[(y * width) + x] = _color;
			}
		}
	}
#pragma endregion

#pragma region Mouse
	void Window::mouse_position(int& x, int& y) const
	{
		SDL_GetMouseState(&x, &y);
	}

#pragma endregion


}