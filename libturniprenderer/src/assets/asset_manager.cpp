#include "turniprenderer/assets/asset_manager.h"

#include <fstream>
#include <iomanip>
#include <assert.h>

#include "turniprenderer/context.h"
#include "turniprenderer/texture.h"
#include "turniprenderer/shader.h"

namespace TurnipRenderer {
	std::string AssetManager::directoryName(std::string filepath){
		return filepath.substr(0, filepath.rfind('/')); // TODO: Multiplat
	}
	std::string AssetManager::pathRelativeToDirectory(std::string directory, std::string relativePath){
		return directoryName(directory) + '/' + relativePath;
	}

	std::vector<unsigned char> AssetManager::readAsset(std::string path){
		std::ifstream in(path, std::ios::in | std::ios::binary);
		in.unsetf(std::ios::skipws);
		if (in)
		{			
			// read the data:
			std::vector<unsigned char> contents((std::istreambuf_iterator<char>(in)),
												 std::istreambuf_iterator<char>());
			in.close();
			return contents;
		}
		assert(false);
		return std::vector<unsigned char>();
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
	// Shaders have changed
	// TODO: Reconsider whether Shaders should be loaded from here at all
	//template ResourceHandle<Shader> AssetManager::loadAsset<Shader>(std::string, std::string);
}
