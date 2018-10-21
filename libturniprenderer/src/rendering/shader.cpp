#include "turniprenderer/rendering/shader.h"

#include <vector>
#include "turniprenderer/context.h"

namespace TurnipRenderer {
	const char *const ShaderConstants::versionExtensionPrefix = R"(
#version 330 core
// Allows setting the location of inputs/outputs with layout()
#extension GL_ARB_separate_shader_objects : require
// Allows setting the location of uniforms with layout()
#extension GL_ARB_explicit_uniform_location : require
// Allows for constant expressions like "16 + 0" in layout() definitions
#extension GL_ARB_enhanced_layouts : require
)";
	
	void DebugShaders::createShaders(){
		debugOpaqueShader = context.resources.addResource(std::make_unique<UnlitShader>(R"(
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 uv0;


layout(location = 0) uniform mat4 MVP;

layout(location = 0) out vec2 interpolatedUV;

void main() {
    gl_Position = MVP * vec4(position, 1);
    interpolatedUV = uv0;
}
)", R"(
layout(location = 0) in vec2 interpolatedUV;
layout(location = 0) out vec3 color;

void main(){
    ivec2 gridCoords = ivec2(interpolatedUV * 10);
    bool xCoord = gridCoords.x % 2 == 1;
    bool yCoord = gridCoords.y % 2 == 1;
    if (xCoord ^^ yCoord) {
        color = vec3(1,0,1);
    }else{
        color = vec3(0,0,0);
    }
}
)"));
	}

	const char *const Shader::lightingCommonCode = R"(
#line 1000 0
#define TURNIP_CONCAT_IMPL(A, B) A ## B
#define TURNIP_CONCAT(A, B) TURNIP_CONCAT_IMPL(A, B)
#define TURNIP_TEXTURE_NAME(NAME) TURNIP_CONCAT(NAME, _texture)

struct TurnipLight {
	vec3 radiance;
	vec3 direction_worldSpace; // Incoming Direction
};
#line 0 0
)";
	const char *const Shader::lightingAdaptorCode = R"(
#line 2000 0
#ifndef LightingName
#error "LightingName was not defined by the lighting shader!"
#endif

#define TURNIP_LIGHTING_VDATA TURNIP_CONCAT(LightingName, Vertex)
#define TURNIP_LIGHTING_VDATA_FILL_FUNC TURNIP_CONCAT(LightingName, FillVertex)
#define TURNIP_LIGHTING_FDATA TURNIP_CONCAT(LightingName, Fragment)
#define TURNIP_LIGHTING_FDATA_FILL_FUNC TURNIP_CONCAT(LightingName, FillFragment)
#define TURNIP_LIGHTING_FUNC TURNIP_CONCAT(LightingName, Lighting)
)";
	const char *const Shader::lightingVertexDefines = R"(
#line 3000 0
#define TURNIP_FRAGMENT_TEXTURE(A, B)
#define TURNIP_FRAGMENT_SAMPLE(A, B) vec4(0)
)";
	const char *const Shader::lightingVertexCode = R"(
#line 4000 0
#ifndef TURNIP_LIGHTING_VDATA
#error "Something went wrong"
#endif
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 uv0;

layout(location = 0) uniform mat4 MVP;
layout(location = 1) uniform mat4 M;
layout(location = 2) uniform mat4 lightMVP;
layout(location = 5) uniform vec3 camPos;
layout(location = 6) uniform vec3 lightDirection;

layout(location = 0) out struct VertexOut {
    vec2 uv0;
    vec4 shadowmapPos;
    vec3 lightDirection;
    TURNIP_LIGHTING_VDATA vData;
} OUT;

void main(){
    gl_Position = MVP * vec4(position, 1);
    OUT.uv0 = uv0;
    OUT.lightDirection = lightDirection;
    OUT.shadowmapPos = lightMVP * vec4(position, 1);

    OUT.vData = TURNIP_LIGHTING_VDATA_FILL_FUNC (normalize(camPos - (M * vec4(position, 1)).xyz), normalize(vec3(M * vec4(normal, 0))), vec3(0));
}
)";

	const char *const Shader::lightingFragmentDefines = R"(
#line 5000 0
#define TURNIP_FRAGMENT_TEXTURE(A, B) layout(location = 16 + A) \
                                      uniform sampler2D TURNIP_TEXTURE_NAME(B);
#define TURNIP_FRAGMENT_SAMPLE(TEXTURE_NAME, UV) texture(TURNIP_TEXTURE_NAME(TEXTURE_NAME), UV)
)";
	const char *const Shader::lightingFragmentCode = R"(
#line 6000 0
layout(location = 0) in struct {
    vec2 uv0;
    vec4 shadowmapPos;
    vec3 lightingDirection;
    TURNIP_LIGHTING_VDATA vData;
} IN;

layout(location = 0) out vec4 OUTPUT_COLOR;

layout(location = 3) uniform sampler2D shadowmapColor;
layout(location = 4) uniform sampler2DShadow shadowmapDepth;

void main(){
    TurnipLight dirLight;
    {
        vec3 lightSpaceCoord = IN.shadowmapPos.xyz / IN.shadowmapPos.w;
        lightSpaceCoord = lightSpaceCoord * 0.5 + 0.5; // Transform to 0-1
        lightSpaceCoord.z -= 0.05; // Bias
        dirLight.radiance = texture(shadowmapColor, lightSpaceCoord.xy).rgb * texture(shadowmapDepth, lightSpaceCoord);
    }
    dirLight.direction_worldSpace = IN.lightingDirection;

    TURNIP_LIGHTING_FDATA fData = TURNIP_LIGHTING_FDATA_FILL_FUNC (IN.vData, IN.uv0);
    OUTPUT_COLOR = TURNIP_LIGHTING_FUNC (IN.vData, fData, dirLight);
}
)";
	
	void DefaultShaders::createShaders(){
		phongOpaqueShader = context.resources.addResource(std::make_unique<Shader>(lightingCode));
		fprintf(stdout, "Finished compiling PBR shader\n");
		std::string mvpVertexShader = R"(
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 uv0;

layout(location = 0) uniform mat4 MVP;

void main() {
    gl_Position = MVP * vec4(position, 1);
}
)";
		// TODO: Make this a lighting shader
		transparentColorShader = context.resources.addResource(std::make_unique<UnlitShader>(mvpVertexShader, R"(
layout(location = 0) out vec4 color;
layout(location = 1) uniform vec4 colorUniform;

void main(){
    color = colorUniform;
}
)"));
		//transparentColorShader = context.resources.addResource(Shader(lightingCode));

		depthOnlyShader = context.resources.addResource(std::make_unique<UnlitShader>(mvpVertexShader, R"(
void main(){}
)"));
	}

	void ShaderBase::compileShaders(ShaderSourceArray vertexSources, size_t vertexSourceCount,
									ShaderSourceArray fragmentSources, size_t fragmentSourceCount){
		GLuint vertexId, fragmentId;
		
		auto compileShader = [](GLuint& id, GLenum type, ShaderSourceArray sources, size_t sourceCount){
			id = glCreateShader(type);

			glShaderSource(id, sourceCount, sources, nullptr);
			glCompileShader(id);

			GLint compileResult = GL_FALSE;
			int messageLength;
			glGetShaderiv(id, GL_COMPILE_STATUS, &compileResult);
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &messageLength);
			if (messageLength > 0){
				std::vector<char> message(messageLength + 1);
				glGetShaderInfoLog(id, messageLength, nullptr, message.data());
				
				if (compileResult == GL_FALSE) throw std::runtime_error(message.data());
				else fprintf(stdout, "Shader Compilation Info: %s\n", message.data());
			}
		};
		compileShader(vertexId, GL_VERTEX_SHADER, vertexSources, vertexSourceCount);
		compileShader(fragmentId, GL_FRAGMENT_SHADER, fragmentSources, fragmentSourceCount);

		programId = glCreateProgram();
		glAttachShader(programId, vertexId);
		glAttachShader(programId, fragmentId);
		glLinkProgram(programId);

		GLint linkResult = GL_FALSE;
		int linkMessageLength;
		// Check the program
		glGetProgramiv(programId, GL_LINK_STATUS, &linkResult);
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &linkMessageLength);
		if (linkMessageLength > 0){
			std::vector<char> linkMessage(linkMessageLength + 1);
			glGetProgramInfoLog(programId, linkMessageLength, nullptr, linkMessage.data());
			
			if (linkResult == GL_FALSE) throw std::runtime_error(linkMessage.data());
			else fprintf(stdout, "Link Info: %s\n", linkMessage.data());
		}
	
		glDetachShader(programId, vertexId);
		glDetachShader(programId, fragmentId);
	
		glDeleteShader(vertexId);
		glDeleteShader(fragmentId);
	}
}
