#include "vtracer.h"

// TODO this is temp, I should seperate lua from this file
#include <lua.hpp>
#include <lauxlib.h>
#include <lualib.h>

namespace vtracer
{
	// define lua names
#define LUA_START_X "start_x"
#define LUA_START_Y "start_y"
#define LUA_END_X "end_x"
#define LUA_END_Y "end_y"
	// lua functions
	static int LUA_set_pixel(const Uint8, const Uint8);

	static void on_mouse_down();
	static void on_mouse_up();
	static void stack_dump();
	static void LUA_draw();

	class MyEventProcesser : public SDLWindow::EventProcesser
	{
		bool process(const SDL_Event& e) const override
		{
			switch (e.type)
			{
			case SDL_QUIT:
				return false;
			case SDL_MOUSEBUTTONDOWN:
				on_mouse_down();
				break;
			case SDL_MOUSEBUTTONUP:
				on_mouse_up();
				break;
			}
			return true;
		}
	};

	static const MyEventProcesser event_processer;

	// TODO Clean up this mess later

	// reference
	static const vtracer::VTracer* vtracer_instance = nullptr;
	lua_State* L = nullptr;
	
	static void on_mouse_down()
	{
		lua_getfield(L, LUA_REGISTRYINDEX, "test");

		int x, y;
		::vtracer::vtracer_instance->mouse_position(x, y);

		lua_pushnumber(L, x);
		lua_pushnumber(L, y);
		lua_setfield(L, -3, LUA_START_Y);
		lua_setfield(L, -2, LUA_START_X);

		lua_pop(L, 1);

		std::cout << "Mouse pressed!" << std::endl;
	}

	static void on_mouse_up()
	{
		lua_getfield(L, LUA_REGISTRYINDEX, "test");

		int x, y;
		::vtracer::vtracer_instance->mouse_position(x, y);

		lua_pushnumber(L, x);
		lua_pushnumber(L, y);
		lua_setfield(L, -3, LUA_END_Y);
		lua_setfield(L, -2, LUA_END_X);

		lua_pop(L, 1);

		LUA_draw();

		std::cout << "Mouse released!" << std::endl;
	}

	static void LUA_draw()
	{
		// TODO this is only temp, remove later
		lua_getfield(L, LUA_REGISTRYINDEX, "test");
		lua_getfield(L, -1, "draw");
		lua_pcall(L, 0, 0, 0);
		lua_pop(L, 1);

		::vtracer::vtracer_instance->render();
	}

	// lua definitions
	static int LUA_set_pixel(lua_State* L)
	{
		// get arguments
		Uint16 x = luaL_checknumber(L, 1);
		Uint16 y = luaL_checknumber(L, 2);

		// do whatever you gotta do
		::vtracer::vtracer_instance->draw_pixel(x, y);

		// no results
		return 0;
	}


#pragma region Class Implementation
	VTracer::VTracer(const __int16 width, const __int16 height) :
		window(new SDLWindow::Window(width, height)),
		buffer(window->get_buffer())
	{
		// calculate pixel size
		this->pixel_size = window->get_width() > window->get_height() ?
			window->get_width() : window->get_height();
		this->pixel_size /= (pixel_size / 10);

		// TODO maybe the most stupid thing, should work for now tho
		::vtracer::vtracer_instance = this;

	}

	VTracer::~VTracer()
	{}

	bool VTracer::init() const
	{

		bool ok = true;

		ok = ok && this->window->init();

		// initialize event processor
		this->window->register_event_processer(&event_processer);

		// initialize background
		memset(window->get_buffer(), 150, window->get_width() * window->get_height() * sizeof(Uint32));
		this->window->render();

		// testing purposes
		this->window->draw_pixel(SDLWindow::COLOR::GREEN, 40, 44, this->pixel_size);
		this->window->draw_pixel(SDLWindow::COLOR::GREEN, 400, 44, this->pixel_size);
		this->window->draw_pixel(SDLWindow::COLOR::GREEN, 200, 80, this->pixel_size);
		this->window->draw_pixel(SDLWindow::COLOR::GREEN, 790, 300, this->pixel_size);
		this->window->render();

		// load lua scripts
		using ::vtracer::L;
		L = luaL_newstate();
		luaL_openlibs(L);

		luaL_loadfile(L, "lua/test.lua");

		// TODO move somewhere else, this is temp only
		// save script into registry
		lua_newtable(L);
		lua_newtable(L);
		lua_getglobal(L, "_G");
		lua_setfield(L, -2, "__index");
		lua_setmetatable(L, -2);

		lua_setfield(L, LUA_REGISTRYINDEX, "test");
		lua_getfield(L, LUA_REGISTRYINDEX, "test");
		lua_setupvalue(L, 1, 1);

		lua_pcall(L, 0, LUA_MULTRET, 0);

		lua_getfield(L, LUA_REGISTRYINDEX, "test");
		lua_pushcfunction(L, LUA_set_pixel);
		lua_setfield(L, -2, "draw_pixel");

		lua_pop(L, 1);

		stack_dump();

		return ok;
	}

	bool VTracer::process()
	{
		return this->window->process();
	}

	void VTracer::draw_pixel(Uint16 x, Uint16 y) const
	{
		this->window->draw_pixel(SDLWindow::COLOR::BLUE, x, y, this->pixel_size);
	}

	void VTracer::render() const
	{
		this->window->render();
	}

	void VTracer::mouse_position(int& x, int& y) const
	{
		this->window->mouse_position(x, y);
	}


#pragma endregion

	static void stack_dump()
	{
		int top = lua_gettop(L);

		std::cout << ">>>> STACK DUMP <<<<" << std::endl;

		for (int i = 1; i <= top; i++)
		{
			int t = lua_type(L, i);

			switch (t)
			{
			case LUA_TSTRING:
				std::cout << lua_tostring(L, i) << std::endl;
				break;
			case LUA_TBOOLEAN:
				std::cout << lua_toboolean(L, i) << std::endl;
				break;
			case LUA_TNUMBER:
				std::cout << lua_tonumber(L, i) << std::endl;
				break;
			default:
				std::cout << lua_typename(L, i) << std::endl;
				break;
			}
		}

		std::cout << ">>>> END <<<<" << std::endl;
	}
}