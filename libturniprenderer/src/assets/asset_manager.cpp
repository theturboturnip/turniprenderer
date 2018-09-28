#include "assets/asset_manager.h"

#include <fstream>

#include "context.h"
#include "texture.h"
#include "shader.h"

namespace TurnipRenderer {
	std::string AssetManager::readAsset(std::string path){
		std::ifstream in(path, std::ios::in | std::ios::binary);
		if (in)
		{
			std::string contents;
			in.seekg(0, std::ios::end);
			contents.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();
			return(contents);
		}
		return "";
	}

	template<class T>
	ResourceHandle<T> AssetManager::loadAsset(std::string path){
		ResourceHandle<T> existingAsset;
		context.resources.getNamedResource(existingAsset, path);
		if (existingAsset) return existingAsset;
		return context.resources.addNamedResource(T(readAsset(path)), path);
	}
	template ResourceHandle<Texture> AssetManager::loadAsset<Texture>(std::string);
	template<class T>
	ResourceHandle<T> AssetManager::loadAsset(std::string path1, std::string path2){
		std::string id = path1+path2;
		ResourceHandle<T> existingAsset;
		context.resources.getNamedResource(existingAsset, id);
		if (existingAsset) return existingAsset;
		return context.resources.addNamedResource(T(readAsset(path1), readAsset(path2)), id);
	}
	template ResourceHandle<Shader> AssetManager::loadAsset<Shader>(std::string, std::string);
}
