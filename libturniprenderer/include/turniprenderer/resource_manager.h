#pragma once

#include <algorithm>
#include <vector>
#include <memory>
#include <unordered_map>

#include "resource.h"

namespace TurnipRenderer {
	template<typename... TRemaining>
	class ResourceManager;

	template<>
	class ResourceManager<>{
    public:
        template<typename T>
        void addResource(T&&) = delete;
		template<typename T>
		void addResource(T&&, size_t) = delete;
		template<typename T>
		ResourceHandle<T> getNamedResource(size_t){
			return ResourceHandle<T>();
		}
		
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
			unnamedResources.push_back(std::make_unique<ResourceContainer<T>>(std::move(resource)));
			auto toReturn = ResourceHandle<T>(unnamedResources.back().get());
			return toReturn;
		}
		ResourceHandle<T> addNamedResource(T&& resource, size_t id){
			auto uniquePtr = std::make_unique<ResourceContainer<T>>(std::move(resource));
			auto toReturn = ResourceHandle<T>(uniquePtr.get());
			namedResources.insert(id, std::move(uniquePtr));
			return toReturn;
		}
		template<class TypeToGet>
		ResourceHandle<TypeToGet> getNamedResource(size_t id){
			if constexpr (!std::is_same<TypeToGet, T>::value) {
					return Base::template getNamedResource<TypeToGet>(id);
				}
			auto resourceContainerIter = namedResources.find(id);
			if (resourceContainerIter == namedResources.end()){
				return ResourceHandle<T>();
			}
			return ResourceHandle<TypeToGet>(resourceContainerIter->second);
		}

		void destroyAllUnused(){
			Base::destroyAllUnused();
			this->unnamedResources.erase(std::remove_if(
											 unnamedResources.begin(),
											 unnamedResources.end(),
											 [](const std::unique_ptr<ResourceContainer<T>>& r){
												 return !r->hasRefs();
											 }
											 ), unnamedResources.end());

			for(auto iter = this->namedResources.begin(); iter != this->namedResources.end(); ) {
				if (!iter->second->hasRefs()) {
                    iter = this->namedResources.erase(iter);
				} else {
                    ++iter;
				}
			}
		}

		inline size_t totalResources() const {
			return unnamedResources.size() + namedResources.size() + Base::totalResources();
		}
	private:
		// Vector of unique_ptr so that ResourceHandles can take pointers to the
		// ResourceContainer without worrying about invalidation when the vector resizes
		std::vector<std::unique_ptr<ResourceContainer<T>>> unnamedResources;
		std::unordered_map<size_t, std::unique_ptr<ResourceContainer<T>>> namedResources;

	};
}
