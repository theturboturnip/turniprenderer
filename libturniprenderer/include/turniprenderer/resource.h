#include <atomic>
#include <iostream>

namespace TurnipRenderer {

	template<typename T, bool IsConst>
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
		std::atomic<uint32_t> refCount; // TODO: It would be ideal if this was only accessible by ResourceHandle/ResourceManager
	};

	// TODO: Rewrite the specializations
	template<typename T, bool IsConst=std::is_const<T>::value>
	class ResourceHandle;

	template<typename T>
	class ResourceHandle<const T, true>{
	public:
		constexpr static bool IsConst = true;
        
        const T* operator->() const {
            return &container->resource;
        }
        const T& operator*() const {
            return container->resource;
        }

        ResourceHandle(ResourceContainer<T>* newContainer)
        : container(newContainer) {
            container->refCount++;
			std::cout<< "Construct from container\n";
        }
        // For a ResourceHandle to non-const, this will not be generated to construct from ResourceHandle to const.
        // This constructor would get a ResourceHandle<const T> and NOT a ResourceHandle<T>.
        // The reason this works for a ResourceHandle to const is that T in that case is already const, so a ResourceHandle<const T> will be accepted.
        ResourceHandle(const ResourceHandle<const T>& copyFrom)
        : container(copyFrom.container){
            container->refCount++;
			std::cout << "Copy construct\n";
        }
		ResourceHandle<const T>& operator=(ResourceHandle<const T> newThing){
			// We're about to stop referencing this container, so decrement the refcount
			if (container) container->refCount--;
			container = newThing.container;
			// We now reference this container so increment the refcount
			container->refCount++;
			std::cout << "Copy via =\n";
			return *this;
		}
		ResourceHandle(ResourceHandle<const T>&& moveFrom)
			: container(moveFrom.container){
			moveFrom.container = nullptr;
			std::cout << "Move construct\n";
		}
		ResourceHandle<const T>& operator=(ResourceHandle<const T>&& newThing){
			// We're about to stop referencing this container, so decrement the refcount
			if (container) container->refCount--;
			container = newThing.container;
			container->refCount++;
			std::cout << "Move via =\n";
			return *this;
		}
        ~ResourceHandle(){
            if (container)
				container->refCount--;
			std::cout << "Destruct, container = " << std::hex << container << std::dec << "\n";
        }

		//static_assert(!std::is_trivially_movable(ResourceHandle<const T, true>));


		inline uint32_t refCount() const {
			return container->refCount;
		}

        ResourceContainer<T>* container = nullptr;
	};
	
	template<typename T>
	class ResourceHandle<T, false> : public ResourceHandle<const T, true> {
		using ConstVersion = ResourceHandle<const T, true>;
	public:
		constexpr static bool IsConst = false;

        using ConstVersion::ResourceHandle;
		using ConstVersion::operator =;
        /*ResourceHandle(const ResourceHandle<const T, true>&) = delete;
		template<typename NewT, bool IsConst>
		ResourceHandle<T, false>& operator=(ResourceHandle<NewT, IsConst>) = delete;
		ResourceHandle(ResourceHandle<const T, true>&&) = delete;
		template<typename NewT, bool IsConst>
		ResourceHandle<T, false>& operator=(ResourceHandle<NewT, IsConst>&&) = delete;*/

		operator ResourceHandle<const T>(){
			return ResourceHandle<const T>(*this);
		}

        using ConstVersion::container;
        T* operator->() const {
            return &container->resource;
        }
        T& operator*() const {
            return container->resource;
        }
    };

	//static_assert(!std::is_move_constructible<ResourceHandle<const int, true>>::value, "");
	/*template<typename T>
	class ResourceHandle<T, false> : public ResourceHandle<const T> {
	public:

		template<>
		NonConstT* operator->() {
			return &container->resource;
		}
		const NonConstT* operator->() const {
			return &container->resource;
		}
		template<typename = std::enable_if<!IsConst>>
		NonConstT& operator*() {
			return container->resource;
		}
		const NonConstT& operator*() const {
			return container->resource;
		}
		void operator=(const NonConstT& newVersion) {
			container->resource = newVersion;
		}

		inline uint32_t refCount() const {
			return container->refCount.load();
		}

		ResourceHandle() : container(nullptr){
			std::cout << "Construct from nothing\n";
		}
		ResourceHandle(ResourceContainer<NonConstT>* newContainer)
			: container(newContainer) {
			std::cout << "Construct from container\n";
			container->refCount++;
		}
		ResourceHandle(const ResourceHandle<NonConstT>& copyFrom)
			: container(copyFrom.container) {
			std::cout << "Copy from handle\n";
			container->refCount++;
		}
		template<typename = std::enable_if<IsConst>>
		ResourceHandle(const ResourceHandle<const NonConstT>& copyFrom)
			: container(copyFrom.container) {
			std::cout << "Copy from handle\n";
			container->refCount++;
		}
		ResourceHandle<PossiblyConstT>& operator=(ResourceHandle<NonConstT> copyFrom){
			std::cout << "Copy by op=\n";
			swap(copyFrom);
			return *this;
		}
		ResourceHandle(ResourceHandle<NonConstT>&&) = delete;
		ResourceHandle(ResourceHandle<const NonConstT>&&) = delete;
		~ResourceHandle(){
			container->refCount--;
		}
		
	private:

		void swap(ResourceHandle<T>& copyFrom){
			std::swap(this->container, copyFrom.container);
		}
		
		ResourceContainer<T>* container = nullptr;
	};*/
}
