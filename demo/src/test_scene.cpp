#include "turniprenderer/context.h"
#include "turniprenderer/scene.h"
#include "turniprenderer/scene_object.h"

#include <iostream>

void printScene(TurnipRenderer::Scene& scene){
	std::cout << "Order of objects is: ";
	for (auto* sceneObject : scene.heirarchy){
		std::cout << sceneObject->name << ' ';
	}
	std::cout << "\n";
}

void testScene(){
	TurnipRenderer::Context context;
	TurnipRenderer::Scene scene(context);

	std::cout << "Adding A\n";
	auto* A = scene.addObjectToEndOfRoot("A", glm::vec3(0));
	std::cout << "Adding B\n";
	auto* B = scene.addObjectToEndOfRoot("B", glm::vec3(0));
	std::cout << "Adding C\n";
	auto* C = scene.addObjectToEndOfObject(*A, "C", glm::vec3(0));
	std::cout << "Adding D\n";
	auto* D = scene.addObjectToEndOfObject(*B, "D", glm::vec3(0));
	std::cout << "Adding E\n";
	auto* E = scene.addObjectToIndexOfObject(*B, 0, "E", glm::vec3(0));

	std::cout << "Order of objects should be: ROOT A C B E D\n";
	printScene(scene);
	assert(A->nodeData.children.size() == 1);
	assert((TurnipRenderer::SceneObject*)A->nodeData.children[0] == C);
	assert(A->nodeData.siblingIndex == 0);
	assert(B->nodeData.children.size() == 2);
	assert((TurnipRenderer::SceneObject*)B->nodeData.children[1] == D);
	assert((TurnipRenderer::SceneObject*)B->nodeData.children[0] == E);
	assert(B->nodeData.cachedEndMinus1 == D->nodeData.cachedEndMinus1);
	assert(B->nodeData.cachedEndMinus1 != scene.heirarchy.end());
	assert((TurnipRenderer::SceneObject*)B->nodeData.cachedEndMinus1);
	assert((B->nodeData.cachedEndMinus1 + 1) == scene.heirarchy.end());
	assert(B->nodeData.siblingIndex == 1);
	
	scene.reparentObject(*B, *A);
	std::cout << "Reparented B to the end of A\n";
	std::cout << "Order of objects should be: ROOT A C B E D\n";
	assert(A->nodeData.children.size() == 2);
	assert((TurnipRenderer::SceneObject*)A->nodeData.children[0] == C);
	assert((TurnipRenderer::SceneObject*)A->nodeData.children[1] == B);
	assert(A->nodeData.siblingIndex == 0);
	assert(A->nodeData.cachedEndMinus1 == B->nodeData.cachedEndMinus1);
	assert(B->nodeData.children.size() == 2);
	assert((TurnipRenderer::SceneObject*)B->nodeData.children[1] == D);
	assert((TurnipRenderer::SceneObject*)B->nodeData.children[0] == E);
	assert(D->nodeData.cachedEndMinus1 == D->nodeData.me);
	assert((TurnipRenderer::SceneObject*)D->nodeData.me == D);
	assert((TurnipRenderer::SceneObject*)B->nodeData.cachedEndMinus1 == D);
	assert(B->nodeData.cachedEndMinus1 == D->nodeData.cachedEndMinus1);
	assert(B->nodeData.cachedEndMinus1 != scene.heirarchy.end());
	assert((TurnipRenderer::SceneObject*)B->nodeData.cachedEndMinus1);
	assert((B->nodeData.cachedEndMinus1 + 1) == scene.heirarchy.end());
	assert(scene.heirarchy.size() == 6);
	assert(B->nodeData.siblingIndex == 1);
	printScene(scene);

	scene.reparentObject(*B, *A, 0);
	std::cout << "Reparented B to the start of A\n";
	std::cout << "Order of objects should be: ROOT A B E D C\n";
	assert(A->nodeData.children.size() == 2);
	assert((TurnipRenderer::SceneObject*)A->nodeData.children[0] == B);
	assert((TurnipRenderer::SceneObject*)A->nodeData.children[1] == C);
	assert(A->nodeData.siblingIndex == 0);
	assert(A->nodeData.cachedEndMinus1 == C->nodeData.cachedEndMinus1);
	assert(B->nodeData.children.size() == 2);
	assert((TurnipRenderer::SceneObject*)B->nodeData.children[1] == D);
	assert((TurnipRenderer::SceneObject*)B->nodeData.children[0] == E);
	assert(D->nodeData.cachedEndMinus1 == D->nodeData.me);
	assert((TurnipRenderer::SceneObject*)D->nodeData.me == D);
	assert((TurnipRenderer::SceneObject*)B->nodeData.cachedEndMinus1 == D);
	assert(B->nodeData.cachedEndMinus1 == D->nodeData.cachedEndMinus1);
	assert(B->nodeData.cachedEndMinus1 != scene.heirarchy.end());
	assert((TurnipRenderer::SceneObject*)B->nodeData.cachedEndMinus1);
	assert((C->nodeData.cachedEndMinus1 + 1) == scene.heirarchy.end());
	assert(scene.heirarchy.size() == 6);
	assert(B->nodeData.siblingIndex == 0);
	printScene(scene);
}
