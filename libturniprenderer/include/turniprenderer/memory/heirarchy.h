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

			bool operator == (RawNodeRef rawRef) const {
				return internal == rawRef;
			}
   			bool operator != (RawNodeRef rawRef) const {
				return !this->operator==(rawRef);
			}
			bool operator == (const Node* ptr) const {
				return *internal == ptr;
			}
			bool operator != (const Node* ptr) const {
				return !this->operator==(ptr);
			}
			bool operator == (const NodeRef& other) const {
				return internal == other.internal;
			}
			bool operator != (const NodeRef& other) const {
				return !this->operator==(other);
			}
			
			NodeRef& operator = (RawNodeRef rawRef) {
				internal = rawRef;
				return *this;
			}
			
			Node& operator *() const {
				return **internal;
			}
			Node* operator ->() const {
				return *internal;
			}
			NodeRef& operator ++(){
				internal++;
				return *this;
			}
			NodeRef operator ++(int dummy) const {
				return plusOne();
			}
			NodeRef& operator --(){
				internal--;
				return *this;
			}
			NodeRef operator --(int dummy) const {
				auto newInternal = internal;
				return NodeRef(--newInternal);
			}
			explicit operator Node* () const {
				return *internal;
			}
			operator RawNodeRef() const {
				return internal;
			}
			operator ConstRawNodeRef() const {
				return internal;
			}
		protected:
			friend class Heirarchy<Node>;
			NodeRef plusOne() const {
				auto newInternal = internal;
				return NodeRef(++newInternal);
			}
		private:
			RawNodeRef internal;
		};
		
		class NodeBase {
		public:
			struct HeirarchyIter {
				inline NodeRef begin() const {
					return parent.heirarchyBegin();
				}
				inline NodeRef end() const {
					return parent.heirarchyEnd();
				}
			private:
				friend class NodeBase;
				HeirarchyIter(const NodeBase& parent) : parent(parent) {}
				const NodeBase& parent;
			};
			inline HeirarchyIter subentities() const {
				return HeirarchyIter(*this);
			}
			
			inline NodeRef heirarchyBegin() const {
				return me;
			}
			inline NodeRef heirarchyEnd() const {
				return cachedEndMinus1.plusOne();
			}

			inline bool isRoot() const {
				return me == parent;
			}

			inline Node& getParent() const {
				return *parent;
			}
			inline const auto& getChildren() const {
				return children;
			}
			inline const auto getSiblingIndex() const {
				return siblingIndex;
			}

			virtual ~NodeBase() = default;
			
		protected:
			friend class Heirarchy<Node>;

			virtual void initialize() = 0;
			virtual void onReparent(){};

			inline NodeBase& getAsNodeBase(){
				return *this;
			}
			
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
			const int newSiblingCount = static_cast<int>(newParent->children.size());
			if (newSiblingCount == 0) return 0;
			
			if (relSiblingIndex > newSiblingCount) relSiblingIndex %= newSiblingCount;
			while (relSiblingIndex < 0) relSiblingIndex += newSiblingCount + 1;
			
			return static_cast<size_t>(relSiblingIndex);
		}
		
		inline NodeRef nodePositionFromParentAndSiblingIndex(NodeRef parent, size_t siblingIndex){
			if (siblingIndex > 0)
				return parent->children[siblingIndex - 1]->getAsNodeBase().heirarchyEnd();
			return parent.plusOne();
		}
		
		// Utility function to update a node when a sibling of a given index is changed (deleted or inserted)
		inline void updateNodeParent(NodeRef parent, size_t siblingIndex){
			auto& siblings = parent->getAsNodeBase().children;
			if (siblingIndex >= siblings.size() - 1)
				parent->getAsNodeBase().updateCachedEnd(); // This will update the parent's parent's end, and continue up the stack
			// Update new siblings that have changed
			for (size_t i = siblingIndex; i < siblings.size(); i++) {
				siblings[i]->getAsNodeBase().siblingIndex = i;
			}
		}
		
		// Utility function to use when reparenting nodes
		inline void moveNodeAndChildren(NodeRef toMove, NodeRef newLocation){
			if (newLocation == toMove->getAsNodeBase().heirarchyBegin()) return;
			heirarchy.splice(newLocation,
							 heirarchy, // A std::list splicing itself is defined to work
							 toMove->getAsNodeBase().heirarchyBegin(), toMove->getAsNodeBase().heirarchyEnd());
		}
	};
}

#include "heirarchy.impl"
