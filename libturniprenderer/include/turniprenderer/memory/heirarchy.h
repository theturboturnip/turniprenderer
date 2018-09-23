#pragma once

#include <assert.h>

namespace TurnipRenderer {

	// NOT THREAD SAFE
	// TODO: Clean up NodeRef and NodeData. NodeRef probably shouldn't be used externally, and there should be a better way to integrate NodeData into the class.
	template<class Node>
	class Heirarchy {
	protected:

		// The only object without a parent. All objects are children of this.
		// This is a unique_ptr so that the Node can be defined AFTER Heirarchy<Node> is defined
		std::unique_ptr<Node> root;
		// TODO: Use a better method of allocation. It should be easy to remove from,
		// and any SceneObject* taken at any point in an object's lifetime should remain valid until that lifetime ends.
		std::vector<std::unique_ptr<Node>> storage;
		// TODO: Use a better method of allocation. Linked list is fine because we want constant-time insertion,
		// and it will only be forward/backward accessed. The linked list elements should stay within the same page if possible
		std::list<Node*> heirarchy;
	public:
		Heirarchy(std::unique_ptr<Node> root);

		using ConstRawNodeRef = typename decltype(heirarchy)::const_iterator;
		using RawNodeRef = typename decltype(heirarchy)::iterator;
		struct NodeRef {
			NodeRef() = default;
			NodeRef(RawNodeRef internal) : internal(internal){}

			bool operator == (ConstRawNodeRef rawRef) const {
				return internal == rawRef;
			}
			bool operator != (ConstRawNodeRef rawRef) const {
				return !this->operator==(rawRef);
			}
			NodeRef& operator = (RawNodeRef rawRef) {
				internal = rawRef;
				return *this;
			}
			
			Node& operator *(){
				return **internal;
			}
			Node* operator ->(){
				return *internal;
			}
			NodeRef& operator ++(){
				internal++;
				return *this;
			}
			/*NodeRef operator ++() const {
				return NodeRef(RawNodeRef(internal)++);
				}*/
			NodeRef operator ++(int dummy) const {
				return (*this) + 1;
			}
			NodeRef operator +(int i) const {
				// Assume i == 1 for now
				assert(i == 1);

				auto newInternal = internal;
				return NodeRef(++newInternal);
			}
			explicit operator Node* (){
				return *internal;
			}
			operator RawNodeRef(){
				return internal;
			}
			operator ConstRawNodeRef(){
				return internal;
			}
		private:
			RawNodeRef internal;
		};
		
		class NodeData {
			friend class Heirarchy<Node>;
		public:	
			inline NodeRef begin() const {
				return me;
			}
			inline NodeRef end() const {
				return cachedEndMinus1 + 1;
			}
			//private:
			Heirarchy<Node>* heirarchy = nullptr;
			NodeRef me;
			NodeRef parent;
			std::vector<NodeRef> children;
			size_t siblingIndex;

			// We don't cache the actual end, we cache it's value -1, which will always be consistent even if the node is reparented.
			NodeRef cachedEndMinus1;
			void updateCachedEnd(bool updateParent = true);
		};

		NodeRef addNode(std::unique_ptr<Node> nodeStoragePtr, Node* newParentPtr = nullptr, int relSiblingIndex = -1);
		void unparentNode(Node& node, int relSiblingIndex = -1);
		void reparentNode(Node& node, Node& newParentNode, int relSiblingIndex = -1);		
	private:
		// These don't need to be in the .impl file, because they only deal in NodeRefs
		
		inline size_t getRealSiblingIndex(NodeRef newParent, int relSiblingIndex){
			const int newSiblingCount = static_cast<int>(newParent->nodeData.children.size());
			if (newSiblingCount == 0) return 0;
			
			if (relSiblingIndex > newSiblingCount) relSiblingIndex %= newSiblingCount;
			while (relSiblingIndex < 0) relSiblingIndex += newSiblingCount + 1;
			
			return static_cast<size_t>(relSiblingIndex);
		}
		
		inline NodeRef nodePositionFromParentAndSiblingIndex(NodeRef parent, size_t siblingIndex){
			if (siblingIndex > 0)
				return parent->nodeData.children[siblingIndex - 1];
			return parent;
		}
		
		// Utility function to update a node when a sibling of a given index is changed (deleted or inserted)
		inline void updateNodeParent(NodeRef parent, size_t siblingIndex){
			auto& siblings = parent->nodeData.children;
			if (siblingIndex >= siblings.size() - 1)
				parent->nodeData.updateCachedEnd(); // This will update the parent's parent's end, and continue up the stack
			// Update new siblings that have changed
			for (size_t i = siblingIndex; i < siblings.size(); i++) {
				siblings[i]->nodeData.siblingIndex = i;
			}
		}
		
		// Utility function to use when reparenting nodes
		inline void moveNodeAndChildren(NodeRef toMove, NodeRef newLocation){
			if (newLocation + 1 == toMove->nodeData.begin()) return;
			heirarchy.splice(newLocation + 1, // This is the "element BEFORE which the content will be inserted", the newLocation is the element AFTER which the content should be inserted
							 heirarchy, // A std::list splicing itself is defined to work
							 toMove->nodeData.begin(), toMove->nodeData.end());
		}
	};
}
