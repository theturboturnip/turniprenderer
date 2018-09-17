#include <iostream>

void testResourceManager();
void testScene();

int main(){

	std::cout << "\n||| Testing ResourceManager |||\n";
	testResourceManager();
	std::cout << "||| Done Testing ResourceManager |||\n";

	std::cout << "\n||| Testing Scene |||\n";
	testScene();
	std::cout << "||| Done Testing Scene |||\n";
			
	return 0;
}
