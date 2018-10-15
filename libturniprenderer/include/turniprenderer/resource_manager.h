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
        void addResource(std::unique_ptr<T>&&) = delete;
		template<typename T>
		void addNamedResource(T&&, size_t) = delete;
		template<typename T>
		void addNamedResource(std::unique_ptr<T>&&, std::string&) = delete;
		template<typename T>
		void getNamedResource(ResourceContainer<T>&, std::string&) = delete;
		
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
		// TODO: Only included for compat. Figure out if we want this or not.
		ResourceHandle<T> addResource(T&& resource){
			return addResource(std::make_unique<T>(resource));
		}
        ResourceHandle<T> addResource(std::unique_ptr<T>&& resource){
			unnamedResources.push_back(std::make_unique<ResourceContainer<T>>(std::move(resource)));
			auto toReturn = ResourceHandle<T>(unnamedResources.back().get());
			return toReturn;
		}
		using Base::addNamedResource;
		ResourceHandle<T> addNamedResource(T&& resource, std::string& id){
			return addNamedResource(std::make_unique<T>(resource), id);
		}
		ResourceHandle<T> addNamedResource(std::unique_ptr<T>&& resource, std::string& id){
			auto uniquePtr = std::make_unique<ResourceContainer<T>>(std::move(resource));
			auto toReturn = ResourceHandle<T>(uniquePtr.get());
			namedResources.insert({id, std::move(uniquePtr)});
			return toReturn;
		}
		using Base::getNamedResource;
		void getNamedResource(ResourceHandle<T>& toFill, std::string& id){
			auto resourceContainerIter = namedResources.find(id);
			if (resourceContainerIter == namedResources.end()){
				toFill = ResourceHandle<T>();
			}else{
				toFill = ResourceHandle<T>(resourceContainerIter->second.get());
			}
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
		std::unordered_map<std::string, std::unique_ptr<ResourceContainer<T>>> namedResources;
	};

}
