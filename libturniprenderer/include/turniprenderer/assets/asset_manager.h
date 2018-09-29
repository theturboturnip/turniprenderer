#pragma once

#include <vector>

#include "../context_aware.h"
#include "../resource.h"

namespace TurnipRenderer {	
	class AssetManager : ContextAware {
	public:
		AssetManager(Context& context) : ContextAware(context) {}
		
		std::vector<unsigned char> readAsset(std::string path);
		template<class TypeToLoad>
		ResourceHandle<TypeToLoad> loadAsset(std::string path);
		template<class TypeToLoad>
		ResourceHandle<TypeToLoad> loadAsset(std::string path1, std::string path2);

		static std::string directoryName(std::string filepath);
		static std::string pathRelativeToDirectory(std::string directory, std::string relativePath);
	};
};
