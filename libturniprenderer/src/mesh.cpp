#include "mesh.h"

namespace TurnipRenderer {
	Mesh::Mesh(MeshData data) : data(data) {
		createBuffer(); // TODO: Is doing this in the constructor bad?
	}
	Mesh::Mesh(MeshData&& data) : data(data) {
		createBuffer(); // TODO: Is doing this in the constructor bad?
	}

	void Mesh::createBuffer(){
		const GLsizei totalIndicesSize = data.indices.size() * sizeof(data.indices[0]);
		const GLsizei totalVerticesSize = data.vertices.size() * sizeof(Vertex);
		const GLsizei requiredSize = totalIndicesSize + totalVerticesSize;
		
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, totalVerticesSize, data.vertices.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, totalIndicesSize, data.indices.data(), GL_STATIC_DRAW);
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
				glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)( offsetof(Vertex, position)));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)( offsetof(Vertex, normal)));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)( offsetof(Vertex, tangent)));
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)( offsetof(Vertex, uv0)));

	}
};
