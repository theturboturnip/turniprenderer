#pragma once

#include "private/external/gl.h"

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
	class DebugShaders {
	public:
		std::unique_ptr<Shader> debugOpaqueShader = nullptr;

		void createShaders();
	};
};
