#include "scene.h"

#include <functional>
#include <queue>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "context.h"
#include "scene_object.h"
#include "mesh.h"
#include "resource.h"

namespace TurnipRenderer {
	Scene::Scene(Context& context) : ContextAware(context), Heirarchy<Entity>(
		std::make_unique<Entity>(context, *this, "ROOT", glm::vec3(0))
		){
	}

	void Scene::addModel(std::string path){
		Assimp::Importer importer;
		const aiScene* importedScene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
		if(!importedScene || importedScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !importedScene->mRootNode){
			fprintf(stderr, "Scene at %s failed to load, error %s\n", path.c_str(), importer.GetErrorString());
			return;
		}

		Entity* modelParent = nullptr;
		
		auto createMeshFromAssimp = [this](aiMesh* mesh) -> ResourceHandle<Mesh> {};
		
		struct QueueItem{
			Entity* nodeParent;
			aiNode* item;
		};
		std::queue<QueueItem> nodesToTraverse;
		nodesToTraverse.push(QueueItem{Heirarchy<Entity>::root.get(), importedScene->mRootNode}); 

		while(!nodesToTraverse.empty()){
			QueueItem toDo = nodesToTraverse.front();
			nodesToTraverse.pop();

			aiVector3t<float> assimpLocalPosition;
			aiQuaterniont<float> assimpLocalRotation;
			aiVector3t<float> assimpLocalScale;
			toDo.item->mTransformation.Decompose(assimpLocalScale, assimpLocalRotation, assimpLocalPosition);
			glm::vec3 localPosition = glm::vec3(assimpLocalPosition.x, assimpLocalPosition.y, assimpLocalPosition.z);
			glm::quat localRotation = glm::quat(assimpLocalRotation.w, assimpLocalRotation.x, assimpLocalRotation.y, assimpLocalRotation.z);
			glm::vec3 localScale = glm::vec3(assimpLocalScale.x, assimpLocalScale.y, assimpLocalScale.z);
			Entity* entity = addObjectToEndOfRoot(std::string(toDo.item->mName.C_Str()), localPosition, localRotation, localScale);

			// TODO: Add Meshes
			{}
			
			// Add the children to the queue
			for(unsigned int i = 0; i < toDo.item->mNumChildren; i++){
				nodesToTraverse.push(QueueItem{
						entity,
							toDo.item->mChildren[i]
							});
			}

			if (toDo.item == importedScene->mRootNode) modelParent = entity;

		}

		// TODO: Clean this up
		//aiReleaseImport(importedScene);
	}
}
