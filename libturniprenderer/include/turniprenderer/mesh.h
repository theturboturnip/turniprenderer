#pragma once

#include "glm.h"
#include "private/utils/gl.h"

#include <vector>

namespace TurnipRenderer {
	class Mesh {
	public:
		struct Vertex {
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec3 tangent;
			glm::vec2 uv0;
		};
		struct MeshData {
			std::vector<Vertex> vertices;
			std::vector<GLuint> indices;
		};
		Mesh(MeshData data);
		void createBuffer();

		// Gets the buffer
		inline GLuint getVAO(){
			return vao;
		}
		inline GLuint getVBO(){
			return vbo;
		}
		inline GLuint getIBO(){
			return ibo;
		}
		inline const auto& indices(){
			return data.indices;
		}
	private:
		const MeshData data;
		
		GLuint vao = 0;
		GLuint vbo = 0;
		GLuint ibo = 0;
	};
};
