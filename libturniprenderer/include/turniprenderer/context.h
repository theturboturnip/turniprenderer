#pragma once

#include <string>

#include "resource_manager.h"
#include "engine_fwd.h"
#include "scene.h"
#include "shader.h"

#include <SDL.h>

namespace TurnipRenderer{
	class Context {
	public:
		std::string name = "Context!";
		ResourceManager<Mesh, Shader> resources;
		Scene scene;

		Context(std::string name);
		~Context();
		bool renderFrame();
		void initDemoScene();
		void initWindow();
		
	private:
		constexpr static size_t WIDTH = 1280;
		constexpr static size_t HEIGHT = 720;

		SDL_Window* sdlWindow = nullptr;
		SDL_GLContext openGlContext;
		
		ResourceHandle<Shader> debugProgram;

		void LogAvailableError();
	};
}
