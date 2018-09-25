#include "turniprenderer/context.h"
#include "turniprenderer/scene.h"
#include "turniprenderer/entity.h"

#include <iostream>

void printScene(TurnipRenderer::Scene& scene){
	std::cout << "Order of objects is: ";
	for (auto* sceneObject : scene.heirarchy){
		std::cout << sceneObject->name << ' ';
	}
	std::cout << "\n";
}

void testScene(){
	TurnipRenderer::Context context("Test");
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
	assert(A->getChildren().size() == 1);
	assert((TurnipRenderer::Entity*)A->getChildren()[0] == C);
	assert(A->getSiblingIndex() == 0);
	assert(B->getChildren().size() == 2);
	assert((TurnipRenderer::Entity*)B->getChildren()[1] == D);
	assert((TurnipRenderer::Entity*)B->getChildren()[0] == E);
	assert(B->heirarchyEnd() == D->heirarchyEnd());
	assert(B->heirarchyEnd() != scene.heirarchy.end());
	assert((TurnipRenderer::Entity*)B->heirarchyEnd());
	assert(B->heirarchyEnd() == scene.heirarchy.end());
	assert(B->getSiblingIndex() == 1);
	
	scene.reparentObject(*B, *A);
	std::cout << "Reparented B to the end of A\n";
	std::cout << "Order of objects should be: ROOT A C B E D\n";
	assert(A->getChildren().size() == 2);
	assert((TurnipRenderer::Entity*)A->getChildren()[0] == C);
	assert((TurnipRenderer::Entity*)A->getChildren()[1] == B);
	assert(A->getSiblingIndex() == 0);
	assert(A->heirarchyEnd() == B->heirarchyEnd());
	assert(B->getChildren().size() == 2);
	assert((TurnipRenderer::Entity*)B->getChildren()[1] == D);
	assert((TurnipRenderer::Entity*)B->getChildren()[0] == E);
	assert(D->heirarchyEnd() == D);
	assert((TurnipRenderer::Entity*)B->heirarchyEnd() == D);
	assert(B->heirarchyEnd() == D->heirarchyEnd());
	assert(B->heirarchyEnd() != scene.heirarchy.end());
	assert((TurnipRenderer::Entity*)B->heirarchyEnd());
	assert(B->heirarchyEnd() == scene.heirarchy.end());
	assert(scene.heirarchy.size() == 6);
	assert(B->getSiblingIndex() == 1);
	printScene(scene);

	scene.reparentObject(*B, *A, 0);
	std::cout << "Reparented B to the start of A\n";
	std::cout << "Order of objects should be: ROOT A B E D C\n";
	assert(A->getChildren().size() == 2);
	assert((TurnipRenderer::Entity*)A->getChildren()[0] == B);
	assert((TurnipRenderer::Entity*)A->getChildren()[1] == C);
	assert(A->getSiblingIndex() == 0);
	assert(A->heirarchyEnd() == C->heirarchyEnd());
	assert(B->getChildren().size() == 2);
	assert((TurnipRenderer::Entity*)B->getChildren()[1] == D);
	assert((TurnipRenderer::Entity*)B->getChildren()[0] == E);
	assert(D->heirarchyEnd() == D);
	assert((TurnipRenderer::Entity*)B->heirarchyEnd() == D);
	assert(B->heirarchyEnd() == D->heirarchyEnd());
	assert(B->heirarchyEnd() != scene.heirarchy.end());
	assert((TurnipRenderer::Entity*)B->heirarchyEnd());
	assert(C->heirarchyEnd() == scene.heirarchy.end());
	assert(scene.heirarchy.size() == 6);
	assert(B->getSiblingIndex() == 0);
	printScene(scene);
}
