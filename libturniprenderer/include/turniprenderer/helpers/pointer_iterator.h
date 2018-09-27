#pragma once

#include <type_traits>

namespace TurnipRenderer::Helpers {
	// This is a zero-const abstraction for use with containers of pointers.
	// It takes a raw iterator and converts it such that * and -> produce refs/pointers to the actual thing, not another pointer. It also 
	template<class ContainerType, class T = typename std::remove_pointer<typename ContainerType::value_type>::type>
	class PointerIteratorAdaptor {
	public:
		using RawIter = typename ContainerType::iterator;
		using RawConstIter = typename ContainerType::const_iterator;
	private:
		RawConstIter internal;

	public:
		PointerIteratorAdaptor(RawIter internal)
			: internal(internal) {}
		PointerIteratorAdaptor(RawConstIter internal)
			: internal(internal) {}
		
		inline operator RawConstIter () const {
			return internal;
		}
		inline explicit operator T* () const {
			return *internal;
		}

		inline const T& operator *() const {
			return **internal;
		}
		inline T& operator *() {
			return **internal;
		}
		inline const T* const & operator ->() const {
			return *internal;
		}
		inline T* const & operator ->() {
			return *internal;
		}
		inline auto plusOne(){
			auto newInternal = internal;
			return PointerIteratorAdaptor<ContainerType>(++newInternal);
		}
		inline auto& operator ++(){
			internal++;
			return *this;
		}
		inline auto operator ++(int dummy) const {
			return plusOne();
			}
		inline auto& operator --(){
			internal--;
			return *this;
		}
		inline auto operator --(int dummy) const {
			auto newInternal = internal;
			return NodeRef(--newInternal);
		}
		inline bool operator == (const PointerIteratorAdaptor& other) const {
			return internal == other.internal;
		}
		inline bool operator != (const PointerIteratorAdaptor& other) const {
			return !this->operator == (other);
		}
		inline bool operator == (const RawConstIter& otherInternal) const {
			return internal == otherInternal;
		}
		inline bool operator != (const RawConstIter& otherInternal) const {
			return !this->operator == (otherInternal);
		}
		PointerIteratorAdaptor& operator +(int) = delete;
		PointerIteratorAdaptor& operator -(int) = delete;
	};
}
