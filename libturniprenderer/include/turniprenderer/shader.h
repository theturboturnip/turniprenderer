#pragma once

#include "private/external/gl.h"
#include "context_aware.h"
#include "resource.h"

#include <string>
#include <memory>

namespace TurnipRenderer {
	class Shader {
	public:
		Shader(std::string vertexSrc, std::string fragmentSrc);
		GLuint programId;
	private:
		GLuint vertexId;
		GLuint fragmentId;
	};
	class DebugShaders : ContextAware {
	public:
		DebugShaders(Context& context) : ContextAware(context) {}
		ResourceHandle<Shader> debugOpaqueShader;

		void createShaders();
	};
	class DefaultShaders : ContextAware{
	public:
		DefaultShaders(Context& context) : ContextAware(context) {}
		ResourceHandle<Shader> phongOpaqueShader;
		
		void createShaders();
	};
};
