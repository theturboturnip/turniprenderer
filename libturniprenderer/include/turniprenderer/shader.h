#pragma once

#include "private/external/gl.h"

#include <string>

namespace TurnipRenderer {
	class Shader {
	public:
		Shader(std::string vertexSrc, std::string fragmentSrc);
		GLuint programId;
	private:
		GLuint vertexId;
		GLuint fragmentId;
	};
};
