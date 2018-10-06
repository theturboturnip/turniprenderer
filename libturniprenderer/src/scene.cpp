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

	void Scene::addModel(std::string scenePath){
		Assimp::Importer importer;
		const aiScene* importedScene = importer.ReadFile(scenePath, aiProcess_Triangulate | aiProcess_FlipUVs);
		if(!importedScene || importedScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !importedScene->mRootNode){
			fprintf(stderr, "Scene at %s failed to load, error %s\n", scenePath.c_str(), importer.GetErrorString());
			return;
		}

		auto glmFromAssimpVec2 = [](auto vec) -> glm::vec2 {
			return glm::vec2(vec[0], vec[1]);
		};
		auto glmFromAssimpVec3 = [](auto vec) -> glm::vec3 {
			return glm::vec3(vec[0], vec[1], vec[2]);
		};
		auto glmFromAssimpVec4 = [](auto vec) -> glm::vec4 {
			return glm::vec4(vec[0], vec[1], vec[2], vec[3]);
		};

		std::vector<ResourceHandle<Mesh>> meshes(importedScene->mNumMeshes);
		{
			auto createMeshFromAssimp = [&](aiMesh* mesh) -> ResourceHandle<Mesh> {
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
			for (unsigned int i = 0; i < meshes.size(); i++){
				meshes[i] = createMeshFromAssimp(importedScene->mMeshes[i]);
			}
		}

		std::vector<ResourceHandle<Material>> materials(importedScene->mNumMaterials);
		{
			auto createMaterialFromAssimp = [&](aiMaterial* material) -> ResourceHandle<Material> {

				Material::TransparencyMode transparencyMode = Material::TransparencyMode::Opaque;
				float assimpOpacity;
				material->Get(AI_MATKEY_OPACITY, assimpOpacity);
				// TODO: Do something to check if the matkey is present
				if (assimpOpacity < 1.0f && assimpOpacity != 0.0f){ // 0.0f is used as a null value
					transparencyMode = Material::TransparencyMode::Translucent;
					fprintf(stdout, "Opacity was %f, => material is translucent\n", assimpOpacity);
				}
				
				ResourceHandle<Texture> texture;
				if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0){
					aiString path;
					material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
					std::string pathRelativeToFile(path.C_Str());
					std::string actualPath = AssetManager::pathRelativeToDirectory(scenePath, pathRelativeToFile);
					fprintf(stdout, "\tLoading texture at %s\n", actualPath.c_str());
					if (*path.C_Str() != '*'){
						texture = context.assetManager.loadAsset<Texture>(actualPath);
					}
				}

				glm::vec4 color;
				switch(transparencyMode){
				case Material::TransparencyMode::Translucent:
				{
					aiColor4D assimpColor;
					material->Get(AI_MATKEY_COLOR_TRANSPARENT, assimpColor);
					color = glmFromAssimpVec4(assimpColor);
					color.a = assimpOpacity;
					break;
				}
				default:
				{
					aiColor4D assimpColor;
					material->Get(AI_MATKEY_COLOR_DIFFUSE, assimpColor);
					color = glmFromAssimpVec4(assimpColor);
				}
				}

				return context.resources.addResource(Material{
						texture,
							transparencyMode,
							color
					});
			};
			fprintf(stdout, "Material Report:\n");
			for (unsigned int i = 0; i < materials.size(); i++){
				const char* name = nullptr;
				importedScene->mMaterials[i]->Get(AI_MATKEY_NAME, name);
				fprintf(stdout, "\t%s: %d diffuse textures\n", name, importedScene->mMaterials[i]->GetTextureCount(aiTextureType_DIFFUSE));
				materials[i] = createMaterialFromAssimp(importedScene->mMaterials[i]);
			}
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
				meshOwner->shader = context.getDefaultShaders().phongOpaqueShader;
				meshOwner->material = materials[
					importedScene->mMeshes[workItem.node->mMeshes[i]]->mMaterialIndex
					];
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
