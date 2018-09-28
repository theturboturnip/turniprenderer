#pragma once

#include <string>

#include "context_aware.h"
#include "resource.h"

namespace TurnipRenderer {	
	class AssetManager : ContextAware {
	public:
		AssetManager(Context& context) : ContextAware(context) {}
		
		std::string readAsset(std::string path);
		template<class TypeToLoad>
		ResourceHandle<TypeToLoad> loadAsset(std::string path);
	};
};
