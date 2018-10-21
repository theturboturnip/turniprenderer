#pragma once

#include "turniprenderer/external/gl.h"
#include "turniprenderer/util/context_aware.h"
#include "turniprenderer/resource.h"

#include <string>
#include <memory>
#include <vector>

namespace TurnipRenderer {
	namespace ShaderConstants {
		extern const char *const versionExtensionPrefix;
	};
	
	class ShaderBase {
	public:
		using ShaderSource = const char *const;
		using ShaderSourceArray = ShaderSource *const; // const pointer to const pointer to const char
		using ShaderSourceVector = std::vector<const char *>; // vector of const pointer to const char
		/*ShaderBase(){
			compileShaders(vertexSources, fragmentSources);
			}*/
		//ShaderBase(const ShaderSourceVector& vertexSources, const ShaderSourceVector& fragmentSources
		
		GLuint programId = 0;
	protected:
		void compileShaders(ShaderSourceArray vertexSources, size_t vertexSourceCount, ShaderSourceArray fragmentSources, size_t fragmentSourceCount);
		inline void compileShaders(const ShaderSourceVector& vertexSources, const ShaderSourceVector& fragmentSources){
			compileShaders(vertexSources.data(), vertexSources.size(),
						   fragmentSources.data(), fragmentSources.size());
		}
		
		//GLuint vertexId = 0;
		//GLuint fragmentId = 0;
	};
	class UnlitShader : public ShaderBase {
	public:
		UnlitShader(std::string vertexSrc, std::string fragmentSrc){
			ShaderBase::ShaderSource vertexShaders[] = {
				ShaderConstants::versionExtensionPrefix,
				vertexSrc.c_str()
			};
			ShaderBase::ShaderSource fragmentShaders[] = {
				ShaderConstants::versionExtensionPrefix,
				fragmentSrc.c_str()
			};
			compileShaders(vertexShaders, 2, fragmentShaders, 2);
		}
	};
	class Shader : public ShaderBase {
	public:
		Shader(const std::string& lightingCode){
			ShaderBase::ShaderSource vertexShaders[] = {
				ShaderConstants::versionExtensionPrefix,
				lightingVertexDefines,
				lightingCommonCode,
				lightingCode.c_str(),
				lightingAdaptorCode,
				lightingVertexCode
			};
			ShaderBase::ShaderSource fragmentShaders[] = {
				ShaderConstants::versionExtensionPrefix,
				lightingFragmentDefines,
				lightingCommonCode,
				lightingCode.c_str(),
				lightingAdaptorCode,
				lightingFragmentCode
			};
			compileShaders(vertexShaders, 6, fragmentShaders, 6);
		}
		Shader(std::string defines, const std::string& lightingCode){
			ShaderBase::ShaderSource vertexShaders[] = {
				ShaderConstants::versionExtensionPrefix,
				defines.c_str(),
				lightingVertexDefines,
				lightingCommonCode,
				lightingCode.c_str(),
				lightingAdaptorCode,
				lightingVertexCode
			};
			ShaderBase::ShaderSource fragmentShaders[] = {
				ShaderConstants::versionExtensionPrefix,
				defines.c_str(),
				lightingFragmentDefines,
				lightingCommonCode,
				lightingCode.c_str(),
				lightingAdaptorCode,
				lightingFragmentCode
			};
			compileShaders(vertexShaders, 7, fragmentShaders, 7);
		}

		constexpr static unsigned int materialUniformStart = 16;
		
	private:		
		static const char *const lightingCommonCode;
		static const char *const lightingAdaptorCode;
		static const char *const lightingVertexDefines;
		static const char *const lightingVertexCode;
		static const char *const lightingFragmentDefines;
		static const char *const lightingFragmentCode;
	};
	class DebugShaders : ContextAware {
	public:
		DebugShaders(Context& context) : ContextAware(context) {}
		ResourceHandle<UnlitShader> debugOpaqueShader;

		void createShaders();
	};
	class DefaultShaders : ContextAware{
	public:
		DefaultShaders(Context& context) : ContextAware(context) {}
		ResourceHandle<Shader> phongOpaqueShader;
		ResourceHandle<Shader> phongTransparentShader;
		ResourceHandle<Shader> phongTransparentNoShadowShader;

		ResourceHandle<UnlitShader> depthOnlyShader;

	private:
		friend class Context;

		void createShaders();
		inline void setLightingCode(std::string&& lightingCode){
			this->lightingCode = lightingCode;
		}
		std::string lightingCode = R"(#error "lightingCode for DefaultShaders not set!")";
	};
};
