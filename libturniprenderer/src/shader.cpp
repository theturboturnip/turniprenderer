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

layout(location = 0) out vec2 interpolatedUV;

void main() {
    gl_Position = MVP * vec4(position, 1);
    interpolatedUV = uv0;
}
)", R"(
layout(location = 0) in vec2 interpolatedUV;
layout(location = 0) out vec3 color;
layout(location = 1) uniform sampler2D tex;

void main(){
    color = texture(tex, interpolatedUV).rgb;
}
)"
															  ));
	}
	
	static std::string ShaderPrefix = R"(
#version 330 core
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_explicit_uniform_location : enable
)";
	
	Shader::Shader(std::string vertexSrc, std::string fragmentSrc){
		auto compileShader = [](GLuint& id, GLenum type, std::string src){
			id = glCreateShader(type);

			char const * SourcePointer = src.c_str();
			glShaderSource(id, 1, &SourcePointer , NULL);
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
		compileShader(vertexId, GL_VERTEX_SHADER, ShaderPrefix + vertexSrc);
		compileShader(fragmentId, GL_FRAGMENT_SHADER, ShaderPrefix + fragmentSrc);

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
