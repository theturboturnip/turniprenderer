#include "debug_window.h"

#include "context.h"
#include "entity.h"

#include "private/external/imgui.h"

namespace TurnipRenderer {
	void Debug::DebugWindow::show(){
		Scene& scene = context.scene;
		
		if (ImGui::Begin("Scene Inspector", nullptr)){
			ImGui::BeginChild("Hierarchy", ImVec2(175, 0));
			{
				auto iter = ++scene.heirarchy.begin();
				while(iter != scene.heirarchy.end()){
					Entity* entity = *iter;
					assert(!entity->isRoot());

					const bool isLeaf = (entity->getChildren().size() == 0);
					
					const ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow
						| ImGuiTreeNodeFlags_OpenOnDoubleClick
						| ((selected == entity) ? ImGuiTreeNodeFlags_Selected : 0)
						| (isLeaf ? ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen : 0);
					
					const bool nodeIsOpen = ImGui::TreeNodeEx((void*)entity->getSiblingIndex(), nodeFlags, "%s", entity->name.c_str());

					if (ImGui::IsItemClicked())
						selected = (selected == entity) ? nullptr : entity;

					if (!nodeIsOpen || isLeaf){
						iter = entity->heirarchyEnd();
					}else{
						iter++;
					}
					if (!entity->getParent().isRoot()
						&& entity->getSiblingIndex() == entity->getParent().getChildren().size() - 1){
						ImGui::TreePop();
					}
				}
			}
			ImGui::EndChild();
			ImGui::SameLine();
			ImGui::BeginChild("Selected Object");
			if (selected){
#define XYZ_SPLIT(VEC) VEC.x, VEC.y, VEC.z
				ImGui::Text("Name: %s", selected->name.c_str());
				ImGui::Text("Position");
				ImGui::Indent();
				{
					ImGui::Text("Local: %02.02f %02.02f %02.02f", XYZ_SPLIT(selected->transform.localPosition()));
					ImGui::Text("World: %02.02f %02.02f %02.02f", 0.f,0.f,0.f);
				}
				ImGui::Unindent();
				ImGui::Text("Rotation");
				ImGui::Indent();
				{
					ImGui::Text("Local: %02.02f %02.02f %02.02f", XYZ_SPLIT(selected->transform.localEulerAnglesDegrees()));
					ImGui::Text("World: %02.02f %02.02f %02.02f", 0.f,0.f,0.f);
				}
				ImGui::Unindent();
				ImGui::Text("Scale");
				ImGui::Indent();
				{
					ImGui::Text("Local: %02.02f %02.02f %02.02f", XYZ_SPLIT(selected->transform.localScale()));
					ImGui::Text("World: N/A");
				}
				ImGui::Unindent();
			}else{
				ImGui::Text("No Object Selected!");
			}
			ImGui::EndChild();
#undef XYZ_SPLIT
		}
		ImGui::End();
	}
}
