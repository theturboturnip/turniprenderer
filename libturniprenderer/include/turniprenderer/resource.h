#pragma once

#include <atomic>
#include <iostream>

namespace TurnipRenderer {

	template<typename T>
	class ResourceHandle;
	
	template<typename T>
	class ResourceContainer{
	public:
		T resource;

		ResourceContainer() : refCount(0) {}
		ResourceContainer(T&& newResource) : resource(newResource), refCount(0) {}
		ResourceContainer(const ResourceContainer<T>& copyFrom) = delete;
		ResourceContainer(ResourceContainer<T>&& moveFrom)
			: resource(std::move(moveFrom.resource)), refCount(moveFrom.refCount.load()) {
		}
		ResourceContainer<T>& operator = (ResourceContainer<T>&& moveFrom) {
			resource = std::move(moveFrom.resource);
			refCount = moveFrom.refCount.load();
		}
		
		inline bool hasRefs() const {
			return refCount > 0;
		}
	private:
		friend class ResourceHandle<T>;
		friend class ResourceHandle<const T>;
		std::atomic<uint32_t> refCount;
	};

	template<typename T>
	class ResourceHandle{
        using ResourceType = typename std::remove_const<T>::type;
	public:
		constexpr static bool IsConst = std::is_const<T>::value;

		ResourceHandle()
			: ResourceHandle(nullptr, false) {}
		ResourceHandle(ResourceContainer<ResourceType>* newContainer)
			: ResourceHandle(newContainer, true) {}
        ResourceHandle(const ResourceHandle<T>& copyFrom)
			: ResourceHandle(copyFrom.container){}
		ResourceHandle<T>& operator=(ResourceHandle<T> newThing){
			if (container) container->refCount--;
			container = newThing.container;
			if (container) container->refCount++;
            return *this;
		}
        //ResourceHandle(ResourceHandle<T>&& moveFrom)
		/*	: ResourceHandle(moveFrom.container, false){
			moveFrom.container = nullptr;
			}*/
		//ResourceHandle<T>& operator=(ResourceHandle<T>&& newThing)
		/*{
			if (container) container->refCount--;
			container = newThing.container;
			// DO NOT INCREMENT REFCOUNT! This transfers ownership from newThing
			}*/
        ~ResourceHandle(){
            if (container)
				container->refCount--;
        }

		operator ResourceHandle<const T>(){
			return ResourceHandle<const T>(container);
		}

		T* resourcePointer() const {
			return (T*)(*this);
		}
        T* operator->() const {
			return (T*)(*this);
        }
        T& operator*() const {
			// TODO: Fail in some way when container == nullptr?
            return container->resource;
        }
		explicit operator T*() const {
			if (container)
				return &container->resource;
			return nullptr;
		}
		explicit operator bool() const {
			return isValid();
		}
		inline bool isValid() const {
			if (container) return container->refCount > 0;
			return false;
		}

        inline uint32_t refCount() const {
			if (container)
				return container->refCount;
			return 0;
		}
		
	protected:
		ResourceHandle(ResourceContainer<ResourceType>* newContainer, bool addToRefCount)
			: container(newContainer) {
            if (addToRefCount && container) container->refCount++;
        }


        ResourceContainer<ResourceType>* container = nullptr;
    };

	
}
