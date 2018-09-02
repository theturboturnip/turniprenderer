#pragma once

#include <algorithm>
#include <vector>
#include <memory>

#include "resource.h"

namespace TurnipRenderer {
	template<typename... TRemaining>
	class ResourceManager;

	template<>
	class ResourceManager<>{
    public:
        template<typename T>
        void addResource(T&&) = delete;

		void destroyAllUnused(){}
		inline size_t totalResources() const {
			return 0;
		}
	};
	template<typename T, typename... TRemaining>
	class ResourceManager<T, TRemaining...> : public ResourceManager<TRemaining...>{
		using Base = ResourceManager<TRemaining...>;
		
	public:
		using Base::addResource;
        ResourceHandle<T> addResource(T&& resource){
			resources.push_back(std::make_unique<ResourceContainer<T>>(std::move(resource)));
			auto toReturn = ResourceHandle<T>(resources.back().get());
			return ResourceHandle<T>(resources.back().get());
		}

		void destroyAllUnused(){
			Base::destroyAllUnused();
			this->resources.erase(std::remove_if(
								resources.begin(),
								resources.end(),
								[](const std::unique_ptr<ResourceContainer<T>>& r){
									return !r->hasRefs();
								}
							), resources.end());
		}

		inline size_t totalResources() const {
			return resources.size() + Base::totalResources();
		}
	private:
		// Vector of unique_ptr so that ResourceHandles can take pointers to the
		// ResourceContainer without worrying about invalidation when the vector resizes
		std::vector<std::unique_ptr<ResourceContainer<T>>> resources;
	};
}
