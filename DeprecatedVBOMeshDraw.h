#ifndef DEPRECATED_VBO_MESH_DRAW_H_INCLUDED
#define DEPRECATED_VBO_MESH_DRAW_H_INCLUDED

#include "Mesh.h"

class DeprecatedVBOMeshDraw {
public:
	DeprecatedVBOMeshDraw() {}

	static const int INDEX_INVALID = -1;

	int vao = INDEX_INVALID;

	int vertexVBO = INDEX_INVALID;

	int indexPointVBO = INDEX_INVALID;
	int indexLineVBO = INDEX_INVALID;
	int indexTriangleVBO = INDEX_INVALID;
	int indexQuadVBO = INDEX_INVALID;

	int pointCount = 0;
	int lineCount = 0;
	int triangleCount = 0;
	int quadCount = 0;

	template <typename VertType>
	void init (Mesh<VertType>& mesh) {
		if (mesh.vertexList.size() == 0)
			return;

		if (mesh.elementIndex.size() == 0)
			return;

		std::vector<int> pointElemnts;
		std::vector<int> lineElemnts;
		std::vector<int> triangleElemnts;
		std::vector<int> quadElemnts;

		glGenVertexArrays(1, (GLuint*)&vao);
		glBindVertexArray(vao);

		for (int i = 0; i < mesh.elementIndex.size(); i++) {
			auto& face = mesh.elementIndex[i];

			if (face.size() == 1) {
				pointCount++;
				for (auto&& index : face) {
					pointElemnts.push_back(index);
				}
			}

			if (face.size() == 2) {
				lineCount++;
				for (auto&& index : face) {
					lineElemnts.push_back(index);
				}
			}

			if (face.size() == 3) {
				triangleCount++;
				for (auto&& index : face) {
					triangleElemnts.push_back(index);
				}
			}

			if (face.size() == 4) {
				quadCount++;
				for (auto&& index : face) {
					quadElemnts.push_back(index);
				}
			}
		}

		auto storeElements = [] (int &indexVBO, std::vector<int>& buffer) {
			if (buffer.size() == 0) {
				indexVBO = INDEX_INVALID;
				return false;
			}

			glGenBuffers(1, (GLuint*)&indexVBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer.size() * sizeof(int),
					&(buffer[0]), GL_STATIC_DRAW);	
		};

		storeElements(indexPointVBO, pointElemnts);
		storeElements(indexLineVBO, lineElemnts);
		storeElements(indexTriangleVBO, triangleElemnts);
		storeElements(indexQuadVBO, quadElemnts);

		glGenBuffers(1, (GLuint*)&vertexVBO);
		glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
		glBufferData(GL_ARRAY_BUFFER, mesh.vertexList.size() * sizeof(mesh.vertexList[0]),
				&(mesh.vertexList[0]), GL_STATIC_DRAW);

		char *baseAddr = (char *)&(mesh.vertexList[0]);

		if constexpr (VertType::template hasVertexAttribute<VertexPosition>()) {
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, sizeof(mesh.vertexList[0]),
					(void *)((char *)&(mesh.vertexList[0].template get<VertexPosition>()) - baseAddr));
		}

		if constexpr (VertType::template hasVertexAttribute<VertexNormal>()) {
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, sizeof(mesh.vertexList[0]),
					(void *)((char *)&(mesh.vertexList[0].template get<VertexNormal>()) - baseAddr));
		}

		if constexpr (VertType::template hasVertexAttribute<VertexTexCoord>()) {
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, sizeof(mesh.vertexList[0]),
					(void *)((char *)&(mesh.vertexList[0].template get<VertexTexCoord>()) - baseAddr));
		}

		if constexpr (VertType::template hasVertexAttribute<VertexColor>()) {
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4, GL_FLOAT, sizeof(mesh.vertexList[0]),
					(void *)((char *)&(mesh.vertexList[0].template get<VertexColor>()) - baseAddr));
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void draw(ShaderProgram& shader) {
		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
		
		if (indexPointVBO != INDEX_INVALID) {
			glDrawElements(GL_POINTS, pointCount * 1, GL_UNSIGNED_INT, (char*)NULL + 0);
		}

		if (indexLineVBO != INDEX_INVALID) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexLineVBO);
			glDrawElements(GL_LINES, lineCount * 2, GL_UNSIGNED_INT, (char*)NULL + 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		if (indexTriangleVBO != INDEX_INVALID) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexTriangleVBO);
			glDrawElements(GL_TRIANGLES, triangleCount * 3, GL_UNSIGNED_INT, (char*)NULL + 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		if (indexQuadVBO != INDEX_INVALID) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexQuadVBO);
			glDrawElements(GL_QUADS, quadCount * 4, GL_UNSIGNED_INT, (char*)NULL + 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	~DeprecatedVBOMeshDraw() {
		glDeleteBuffers(1, (GLuint*)&vertexVBO);

		if (indexPointVBO != INDEX_INVALID)
			glDeleteBuffers(1, (GLuint*)&indexPointVBO);
		
		if (indexLineVBO != INDEX_INVALID)
			glDeleteBuffers(1, (GLuint*)&indexLineVBO);
		
		if (indexTriangleVBO != INDEX_INVALID)
			glDeleteBuffers(1, (GLuint*)&indexTriangleVBO);

		if (indexQuadVBO != INDEX_INVALID)
			glDeleteBuffers(1, (GLuint*)&indexQuadVBO);
	}
};

#endif