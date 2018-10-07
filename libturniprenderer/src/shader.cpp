#include "shader.h"

#include <vector>
#include "context.h"

namespace TurnipRenderer {
	void DebugShaders::createShaders(){
		debugOpaqueShader = context.resources.addResource(UnlitShader(R"(
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
#define TURNIP_CONCAT_IMPL(A, B) A ## B
#define TURNIP_CONCAT(A, B) TURNIP_CONCAT_IMPL(A, B)
#define TURNIP_TEXTURE_NAME(NAME) TURNIP_CONCAT(NAME, _texture)

struct TurnipLight {
	vec3 color;
	vec3 direction_worldSpace; // Incoming Direction
};
)";
	const char *const Shader::lightingAdaptorCode = R"(
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
#define TURNIP_FRAGMENT_TEXTURE(A, B)
#define TURNIP_FRAGMENT_SAMPLE(A, B) vec4(0)
)";
	const char *const Shader::lightingVertexCode = R"(
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

layout(location = 0) out struct VertexOut {
    vec2 uv0;
    vec4 shadowmapPos;
    vec3 lightDirection;
    TURNIP_LIGHTING_VDATA vData;
} OUT;

void main(){
    gl_Position = MVP * vec4(position, 1);
    OUT.uv0 = uv0;
    OUT.lightDirection = normalize( (lightMVP * vec4(0,0,1,0)).xyz ); // Directional Light
    OUT.shadowmapPos = lightMVP * vec4(position, 1);

    OUT.vData = TURNIP_LIGHTING_VDATA_FILL_FUNC (vec3(0), normalize(vec3(M * vec4(normal, 0))), vec3(0));
}
)";

	const char *const Shader::lightingFragmentDefines = R"(
#define TURNIP_FRAGMENT_TEXTURE(A, B) layout(location = 16 + A) \
                                      uniform sampler2D TURNIP_TEXTURE_NAME(B);
#define TURNIP_FRAGMENT_SAMPLE(TEXTURE_NAME, UV) texture(TURNIP_TEXTURE_NAME(TEXTURE_NAME), UV)
)";
	const char *const Shader::lightingFragmentCode = R"(
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
        lightSpaceCoord.z -= 0.005; // Bias
        dirLight.color = texture(shadowmapColor, lightSpaceCoord.xy).rgb * texture(shadowmapDepth, lightSpaceCoord);
    }
    dirLight.direction_worldSpace = IN.lightingDirection;

    TURNIP_LIGHTING_FDATA fData = TURNIP_LIGHTING_FDATA_FILL_FUNC (IN.vData, IN.uv0);
    OUTPUT_COLOR = TURNIP_LIGHTING_FUNC (IN.vData, fData, dirLight);
}
)";
	
	void DefaultShaders::createShaders(){
		/*phongOpaqueShader = context.resources.addResource(Shader(R"(
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 uv0;

layout(location = 0) uniform mat4 MVP;
layout(location = 1) uniform mat4 M;
layout(location = 2) uniform mat4 lightMVP;

layout(location = 0) out struct {
vec2 uv0;
vec3 normal;
vec3 lightDirection;
vec4 shadowmapPos;
} OUT;

void main() {
    gl_Position = MVP * vec4(position, 1);
    OUT.uv0 = uv0;
    OUT.normal = normalize((M * vec4(normal, 1)).xyz);
OUT.lightDirection = normalize( (lightMVP * vec4(0,0,1,0)).xyz ); // Directional Light
OUT.shadowmapPos = lightMVP * vec4(position, 1);
}
)", R"(
layout(location = 0) in struct {
vec2 uv0;
vec3 normal;
vec3 lightDirection;
vec4 shadowmapPos;
} IN;
layout(location = 0) out vec3 color;
layout(location = 3) uniform sampler2D shadowmapColor;
layout(location = 4) uniform sampler2DShadow shadowmapDepth;
layout(location = 16) uniform sampler2D tex;

void main(){
    vec3 lightSpaceCoord = IN.shadowmapPos.xyz / IN.shadowmapPos.w;
lightSpaceCoord = lightSpaceCoord * 0.5 + 0.5; // Transform to 0-1
lightSpaceCoord.z -= 0.005; // Bias

    vec3 albedo = texture(tex, IN.uv0).rgb;
    vec3 lightLevel = max(0.0, dot(IN.normal, -IN.lightDirection)) * texture(shadowmapColor, lightSpaceCoord.xy).rgb;
lightLevel *= texture(shadowmapDepth, lightSpaceCoord);
vec3 ambient = vec3(0.3, 0.3, 0.15);
color = albedo * (lightLevel + ambient);
//color = IN.normal;
}
)"
));*/

		phongOpaqueShader = context.resources.addResource(Shader(lightingCode));
		
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
		transparentColorShader = context.resources.addResource(UnlitShader(mvpVertexShader, R"(
layout(location = 0) out vec4 color;
layout(location = 1) uniform vec4 colorUniform;

void main(){
    color = colorUniform;
}
)"));

		depthOnlyShader = context.resources.addResource(UnlitShader(mvpVertexShader, R"(
void main(){}
)"));
	}
	
	std::string ShaderConstants::versionExtensionPrefix = R"(
#version 330 core
#extension GL_ARB_separate_shader_objects : enable//require
#extension GL_ARB_explicit_uniform_location : enable//require
#extension GL_ARB_enhanced_layouts : enable//require
)";

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
