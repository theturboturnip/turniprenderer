#include "shader.h"

#include <vector>

namespace TurnipRenderer {
	Shader::Shader(std::string vertexSrc, std::string fragmentSrc){
		auto compileShader = [](GLuint& id, GLenum type, std::string& src){
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
