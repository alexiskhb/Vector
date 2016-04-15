#pragma once

#include <limits>
#include <memory>

template<typename T>
class Allocator {
public:
	typedef T        value_type;
	typedef T*       pointer;
	typedef const T* const_pointer;
	typedef T&       reference;
	typedef const T& const_reference;
	typedef std::size_t    size_type;
	typedef std::ptrdiff_t difference_type;
	
	template<typename U> 
	struct rebind { 
		typedef Allocator<U> other; 
	};

	Allocator() throw() {
	}

	Allocator(const Allocator&) {
	}

	template<typename U>
	Allocator(const Allocator<U>&) {	
	}

	~Allocator() throw() {
	}

	pointer address(reference r) const {
		return &r;
	}

	const_pointer address(const_reference r) const {
		return &r;
	}

	pointer allocate(size_type n, const_pointer hint = 0) {
		return static_cast<pointer>(operator new(n * sizeof(T)));
	}

	void deallocate(pointer p, size_type n) {
		operator delete(p);
	}

	size_type max_size() const {
		return std::numeric_limits<size_type>::max() / sizeof(T);
	}

	template<typename U, typename... Args>
	void construct(U* p, Args&&... args) {
		new((void *)p) U(std::forward<Args>(args)...);
	}

	template<class U>
	void destroy(U* p) {
		p->~U();
	}
};

template <typename T1, typename T2>
bool operator==(const Allocator<T1>&, const Allocator<T2>&) throw() {
	return true;
}

template <typename T1, typename T2>
bool operator!=(const Allocator<T1>&, const Allocator<T2>&) throw() {
	return false;
}