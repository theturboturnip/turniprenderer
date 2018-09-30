#include "shader.h"

#include <vector>
#include "context.h"

namespace TurnipRenderer {
	void DebugShaders::createShaders(){
		debugOpaqueShader = context.resources.addResource(Shader(R"(
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
)"
															  ));
	}
	void DefaultShaders::createShaders(){
		phongOpaqueShader = context.resources.addResource(Shader(R"(
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
															  ));
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
		transparentColorShader = context.resources.addResource(Shader(mvpVertexShader, R"(
layout(location = 0) out vec4 color;
layout(location = 1) uniform vec4 colorUniform;

void main(){
    color = colorUniform;
}
)"));

		depthOnlyShader = context.resources.addResource(Shader(mvpVertexShader, R"(
void main(){}
)"));
	}
	
	static std::string ShaderPrefix = R"(
#version 330 core
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_explicit_uniform_location : enable
)";

	void Shader::compileShaders(const char* const vertexSrc, const char* const fragmentSrc){
		auto compileShader = [](GLuint& id, GLenum type, const char* const src){
			id = glCreateShader(type);

			char const * SourcePointer[] = {ShaderPrefix.c_str(), src};
			glShaderSource(id, 2, SourcePointer, NULL);
			glCompileShader(id);

			GLint Result = GL_FALSE;
			int InfoLogLength;
			glGetShaderiv(id, GL_COMPILE_STATUS, &Result);
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &InfoLogLength);
			if ( InfoLogLength > 0 ){
				std::vector<char> ShaderErrorMessage(InfoLogLength+1);
				glGetShaderInfoLog(id, InfoLogLength, NULL, &ShaderErrorMessage[0]);
				fprintf(stderr, "%s\n", &ShaderErrorMessage[0]);
			}
		};
		compileShader(vertexId, GL_VERTEX_SHADER, vertexSrc);
		compileShader(fragmentId, GL_FRAGMENT_SHADER, fragmentSrc);

		programId = glCreateProgram();
		glAttachShader(programId, vertexId);
		glAttachShader(programId, fragmentId);
		glLinkProgram(programId);

		GLint Result = GL_FALSE;
		int InfoLogLength;
		// Check the program
		glGetProgramiv(programId, GL_LINK_STATUS, &Result);
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if ( InfoLogLength > 0 ){
			std::vector<char> ProgramErrorMessage(InfoLogLength+1);
			glGetProgramInfoLog(programId, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			fprintf(stderr, "%s\n", &ProgramErrorMessage[0]);
		}
	
		glDetachShader(programId, vertexId);
		glDetachShader(programId, fragmentId);
	
		glDeleteShader(vertexId);
		glDeleteShader(fragmentId);
	}
}
