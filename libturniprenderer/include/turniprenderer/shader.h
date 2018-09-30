#pragma once

#include "private/external/gl.h"
#include "context_aware.h"
#include "resource.h"

#include <string>
#include <memory>
#include <vector>

namespace TurnipRenderer {
	class Shader {
	public:
		Shader(std::string vertexSrc, std::string fragmentSrc){
			compileShaders(vertexSrc.c_str(), fragmentSrc.c_str());
		}
		Shader(std::vector<unsigned char> vertexSrcBytes, std::vector<unsigned char> fragmentSrcBytes){
			compileShaders(
				reinterpret_cast<char*>(vertexSrcBytes.data()),
				reinterpret_cast<char*>(fragmentSrcBytes.data())
				);
		}
		GLuint programId;
	private:
		void compileShaders(const char* const vertexSrc, const char* const fragmentSrc);
		
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
		ResourceHandle<Shader> transparentColorShader;
		ResourceHandle<Shader> depthOnlyShader;
		
		void createShaders();
	};
};
