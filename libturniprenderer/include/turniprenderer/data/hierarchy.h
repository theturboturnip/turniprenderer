#pragma once

#include <assert.h>

#include "helpers/pointer_iterator.h"

#include <list>
#include <vector>
#include <memory>

namespace TurnipRenderer {
	// NOT THREAD SAFE
	template<class NodeContent>
	class Hierarchy {
		// Forward Definitions
	public:
		class Node;
	protected:
		// TODO: Use a better method of allocation. Linked list is fine because we want constant-time insertion,
		// and it will only be forward/backward accessed. The linked list elements should stay within the same page if possible
		using OrderedStorage = std::list<Node*>;
		// TODO: Use a better method of allocation. It should be easy to remove from,
		// and any Node* taken at any point in an object's lifetime should remain valid until that lifetime ends. This doesn't have cache locality, objects are not guaranteed to be close together
		using UnorderedStorage = std::vector<std::unique_ptr<Node*>>;
		using ThisType = Hierarchy<NodeContent>;
		using OrderedIterator = Helpers::PointerIteratorAdaptor<OrderedStorage>;
		using OrderedIteratorToConst = Helpers::PointerIteratorAdaptor<OrderedStorage, const Node>;

	public:
		template<class... Args>
		Hierarchy(Args&&... args)
			: orderedNodes(std::initializer_list(&root)),
			  root(orderedNodes.front(), orderedNodes.front(), 0, std::forward<Args>(args)...){
		}

		template<class... Args>
		Node& addNode(Node& newParent, int relSiblingIndex, Args&&...);
		void unparentNode(Node& node, int relSiblingIndex = -1);
		void reparentNode(Node& node, Node& newParentNode, int relSiblingIndex = -1);		

		// NOTE: This class has a slightly different interpretation of const-correctness to normal.
		// A const Node& can only output const Node*s or similar, so that a program with a const Node& can only read the current scene state not modify it.
		class Node {
		public:
			template<class... Args>
			Node(OrderedIterator me, OrderedIterator parent, size_t siblingIndex, Args&&... args)
				: content(std::make_unique<NodeContent>(std::forward<Args>(args)...)),
				  me(me), parent(parent), children(), siblingIndex(siblingIndex){
				updateCachedEnd(false);
				content->initialize(); // TODO: Is this necessary anymore?
			}
			
			std::unique_ptr<NodeContent> content;

			template<class IteratorType>
			struct HierarchyIter {
				inline IteratorType begin() const {
					return owner.heirarchyBegin();
				}
				inline IteratorType end() const {
					return owner.heirarchyEnd();
				}
			private:
				friend class Node;
				HierarchyIter(Node& owner) : owner(owner) {}
				Node& owner;
			};
			inline auto subentities() const {
				return HierarchyIter<OrderedIteratorToConst>(*this);
			}
			inline auto subentities() {
				return HierarchyIter<OrderedIterator>(*this);
			}

			inline bool isRoot() const {
				return me == parent;
			}

			inline const Node& getParent() const {
				return *parent;
			}
			inline Node& getParent() {
				return *parent;
			}
			inline const Node& getChild(size_t index) const {
				return *children[index];
			}
			inline Node& getChild(size_t index) {
				return *children[index];
			}
			inline auto totalChildren() const {
				return children.size();
			}
			inline auto getSiblingIndex() const {
				return siblingIndex;
			}
			
		protected:
			friend ThisType;

			inline OrderedIteratorToConst heirarchyBegin() const {
				return OrderedIteratorToConst(me);
			}
			inline OrderedIterator heirarchyBegin() {
				return me;
			}
			inline OrderedIteratorToConst heirarchyEnd() const {
				return OrderedIteratorToConst(cachedEndMinus1.plusOne());
			}
			inline OrderedIterator heirarchyEnd() {
				return cachedEndMinus1.plusOne();
			}
			
			OrderedIterator me;
			OrderedIterator parent;
			std::vector<OrderedIterator> children;
			size_t siblingIndex;
			
			// We don't cache the actual end, we cache it's value -1, which will always be consistent even if the node is reparented.
			OrderedIterator cachedEndMinus1;
			void updateCachedEnd(bool updateParent = true);
		};

	protected:
		UnorderedStorage unorderedNodes;
		OrderedStorage orderedNodes;

		// The only object without a parent. All objects are children of this.
		// This must be declared after orderedNodes because it is constructed from data in orderedNodes
		Node root;
	private:
		size_t getRealSiblingIndex(OrderedIterator newParent, int relSiblingIndex);
		OrderedIterator nodePositionFromParentAndSiblingIndex(OrderedIterator parent, size_t siblingIndex);
		void updateNodeParent(OrderedIterator parent, size_t siblingIndex);
		void moveNodeAndChildren(OrderedIterator toMove, OrderedIterator newLocation);
	};
}
#include "hierarchy.impl"
