#include "scene.h"

#include <functional>
#include <queue>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "context.h"
#include "entity.h"
#include "mesh.h"
#include "resource.h"
#include "system.h"

namespace TurnipRenderer {
	Scene::Scene(Context& context) : ContextAware(context), Heirarchy<Entity>(
		std::make_unique<Entity>(context, *this, "ROOT", glm::vec3(0))
		){
	}
	Scene::~Scene() = default;

	void Scene::addModel(std::string path){
		Assimp::Importer importer;
		const aiScene* importedScene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
		if(!importedScene || importedScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !importedScene->mRootNode){
			fprintf(stderr, "Scene at %s failed to load, error %s\n", path.c_str(), importer.GetErrorString());
			return;
		}

		auto glmFromAssimpVec2 = [](auto vec) -> glm::vec2 {
			return glm::vec2(vec.x, vec.y);
		};
		auto glmFromAssimpVec3 = [](aiVector3D vec) -> glm::vec3 {
			return glm::vec3(vec.x, vec.y, vec.z);
		};
		
		auto createMeshFromAssimp = [this, glmFromAssimpVec2, glmFromAssimpVec3](aiMesh* mesh) -> ResourceHandle<Mesh> {
			Mesh::MeshData meshData;

			// Vertices
			{
				auto* assimpTangents = mesh->mTangents;
				auto* assimpUV0 = mesh->mTextureCoords[0];
				for (unsigned int i = 0; i < mesh->mNumVertices; i++){
					Mesh::Vertex vertex;
					vertex.position = glmFromAssimpVec3(mesh->mVertices[i]);
					vertex.normal = glmFromAssimpVec3(mesh->mNormals[i]);
					if (assimpTangents) vertex.tangent = glmFromAssimpVec3(assimpTangents[i]);
					if (assimpUV0) vertex.uv0 = glmFromAssimpVec2(assimpUV0[i]);
					meshData.vertices.push_back(vertex);
				}
			}

			// Indices
			for(unsigned int i = 0; i < mesh->mNumFaces; i++){
				aiFace face = mesh->mFaces[i];
				for(unsigned int j = 0; j < face.mNumIndices; j++)
					meshData.indices.push_back(face.mIndices[j]);
			}

			return context.resources.addResource(Mesh(std::move(meshData)));
		};
		std::vector<ResourceHandle<Mesh>> meshes(importedScene->mNumMeshes);
		for (unsigned int i = 0; i < meshes.size(); i++){
			meshes[i] = createMeshFromAssimp(importedScene->mMeshes[i]);
		}

		Entity* modelParent = nullptr;
				
		struct QueueItem{
			Entity* parentEntity;
			aiNode* node;
		};
		std::queue<QueueItem> workQueue;
		workQueue.push(QueueItem{Heirarchy<Entity>::root.get(), importedScene->mRootNode}); 

		while(!workQueue.empty()){
			QueueItem workItem = workQueue.front();
			workQueue.pop();

			aiVector3D assimpLocalPosition;
			aiQuaternion assimpLocalRotation;
			aiVector3D assimpLocalScale;
			workItem.node->mTransformation.Decompose(assimpLocalScale, assimpLocalRotation, assimpLocalPosition);
			glm::vec3 localPosition = glmFromAssimpVec3(assimpLocalPosition);
			glm::quat localRotation = glm::quat(assimpLocalRotation.w, assimpLocalRotation.x, assimpLocalRotation.y, assimpLocalRotation.z);
			glm::vec3 localScale = glmFromAssimpVec3(assimpLocalScale);
			Entity* entity = addObjectToEndOfObject(*workItem.parentEntity, std::string(workItem.node->mName.C_Str()), localPosition, localRotation, localScale);

			// Add Meshes
			for(unsigned int i = 0; i < workItem.node->mNumMeshes; i++){
				Entity* meshOwner = (i == 0) ? entity : addObjectToEndOfObject(*entity, "Mesh" + std::to_string(i), glm::vec3(0));
				meshOwner->mesh = meshes[workItem.node->mMeshes[i]];
			}
			
			// Add the children to the queue
			for(unsigned int i = 0; i < workItem.node->mNumChildren; i++){
				workQueue.push(QueueItem{
						entity,
							workItem.node->mChildren[i]
							});
			}

			if (workItem.node == importedScene->mRootNode) modelParent = entity;

		}
		modelParent->transform.setLocalScale(glm::vec3(0.001));

		// TODO: Clean this up
		//aiReleaseImport(importedScene);
	}
}
