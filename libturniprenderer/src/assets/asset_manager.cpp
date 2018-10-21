#include "turniprenderer/assets/asset_manager.h"

#include <fstream>
#include <iomanip>
#include <assert.h>

#include "turniprenderer/context.h"
#include "turniprenderer/rendering/texture.h"
#include "turniprenderer/rendering/shader.h"

#include "stb/stb_image.h" // TODO: turniprenderer/external/stb.h

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
		return context.resources.addNamedResource(std::make_unique<T>(readAsset(path)), path);
	}
	template<>
	ResourceHandle<Texture> AssetManager::loadAsset<Texture>(std::string path){
		ResourceHandle<Texture> existingAsset;
		context.resources.getNamedResource(existingAsset, path);
		if (existingAsset) return existingAsset;

		// Use stb to load the image data
		std::vector<unsigned char> assetData = readAsset(path);

		int width, height, channels;
		unsigned char* dataPtr = stbi_load_from_memory(assetData.data(), assetData.size(), &width, &height, &channels, 0);
		assert(dataPtr);
		std::vector<unsigned char> data(dataPtr, dataPtr + (width * height * channels));
		stbi_image_free(dataPtr);
		
		return context.resources.addNamedResource(std::make_unique<Texture>(context, std::move(data), width, height, channels), path);
	}
	template<class T>
	ResourceHandle<T> AssetManager::loadAsset(std::string path1, std::string path2){
		std::string id = path1+path2;
		ResourceHandle<T> existingAsset;
		context.resources.getNamedResource(existingAsset, id);
		if (existingAsset) return existingAsset;
		return context.resources.addNamedResource(std::make_unique<T>(readAsset(path1), readAsset(path2)), id);
	}
	// Shaders have changed
	// TODO: Reconsider whether Shaders should be loaded from here at all
	//template ResourceHandle<Shader> AssetManager::loadAsset<Shader>(std::string, std::string);
}
