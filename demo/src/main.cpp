#include "turniprenderer/resource_manager.h"

#include <iostream>
#include <assert.h>

int main(){

	TurnipRenderer::ResourceManager<bool, float, int> resourceManager;

	std::cout << "\n\nInt Tests: Copy constructor\n";
	{
		TurnipRenderer::ResourceHandle<const int> intResource = resourceManager.addResource(1);
		std::cout << "Int Resource has " << intResource.refCount() << " refs after creation\n";
		{
			std::cout << "Creating copy...";
			TurnipRenderer::ResourceHandle<const int> intResource2 = intResource;
			std::cout << std::endl;
			std::cout << "Int Resource with value " << *intResource << " has " << intResource.refCount() << " refs after copy\n";
		}
		std::cout << "Int Resource has " << intResource.refCount() << " refs after destruction of copy\n";
	}
	assert(resourceManager.totalResources() == 1);
	resourceManager.destroyAllUnused();
	assert(resourceManager.totalResources() == 0);

	std::cout << "\n\nFloat Tests: References\n";
	{
		auto floatResource = resourceManager.addResource(1.0f);
		std::cout << "Float resource has " << floatResource.refCount() << " refs after creation\n";
		{
			auto& floatResourceRef = floatResource;
			assert(floatResourceRef.refCount() == 1 && "Taking a ref of the floatResource added to the refcount!");
		}
		assert(floatResource.refCount() == 1 && "Destroying a ref of the floatResource decremented the refCount"); 
	}
	resourceManager.destroyAllUnused();
	assert(resourceManager.totalResources() == 0);

	std::cout << "\n\nBool Tests: equals\n";
	{
		auto boolResource = resourceManager.addResource(false);
		std::cout << "Bool Resource 1 has " << boolResource.refCount() << " refs after creation\n";
		auto test = boolResource;
		std::cout << "Bool Resource 1 was copied into a new object, now has " << test.refCount() << " refs\n";
		auto boolResource2 = resourceManager.addResource(false);
		std::cout << "Bool Resource 2 has " << boolResource2.refCount() << " refs after creation\n";
		test = boolResource2;
		std::cout << "The object that previously had a copy to Bool Resource 1 now has a copy of Bool Resource 2\n";
		std::cout << "Bool 1 Refcount: " << boolResource.refCount() << " Bool 2 Refcount: " << boolResource2.refCount() << "\n";
	}
	resourceManager.destroyAllUnused();
	assert(resourceManager.totalResources() == 0);
			
	return 0;
}
