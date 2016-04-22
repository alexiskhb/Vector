#pragma once

#include <iterator>
#include <algorithm>
#include <limits>
#include <memory>
#include <cstring>
#include "allocator.h"
#include <iostream>



template<class T, class A = Allocator<T>>
class Vector {
public:
	typedef A allocator_type;
	typedef typename A::value_type      value_type; 
	typedef typename A::reference       reference;
	typedef typename A::const_reference const_reference;
	typedef typename A::size_type       size_type;
	typedef typename A::difference_type difference_type;
	typedef typename A::pointer         pointer;
	typedef typename A::const_pointer   const_pointer;
	typedef pointer       iterator;
	typedef const_pointer const_iterator;

	Vector() : m_begin(nullptr), m_end(nullptr), m_memory_end(nullptr) {
		
	}

	iterator begin() {
		return m_begin;
	}

	const_iterator begin() const {
		return m_begin;
	}

	iterator end() {
		return m_end;
	}

	const_iterator end() const {
		return m_end;
	}

	reference operator[](size_type a_index) {
		return *(m_begin + a_index);
	}

	const_reference operator[](size_type a_index) const {
		return *(m_begin + a_index);
	}

	size_type size() const {
		return m_end - m_begin;
	}

	size_type capacity() const {
		return m_memory_end - m_begin;
	}

	bool empty() const {
		return size() == 0;
	}

	void reserve(size_type a_size) {
		if (capacity() >= a_size) {
			return;
		}
		size_type old_size = size();
		pointer old_begin = begin();
		pointer new_begin = allocate(a_size);
		copy(new_begin, old_begin, old_size);
		destroy(old_begin, old_begin + old_size);
		m_allocator.deallocate(old_begin, old_size);
	}

	iterator insert(iterator a_position, const T& a_value) {
		if (size() + 1 > capacity()) {
			size_type index = a_position - begin();
			iterator new_begin = allocate(2*capacity());
			a_position = new_begin + index;
		}
		move(a_position + 1, a_position, m_end - a_position);
		if (a_position != end()) {
			destroy(a_position, a_position + 1);
		}
		construct(a_position, a_value);
		m_end += 1;
	}

	void push_back(const T& a_value) {
		insert(end(), a_value);
	}

	void push_front(const T& a_value) {
		insert(begin(), a_value);
	}

	// [first, last}
	iterator erase(const_iterator a_first, const_iterator a_last) {
		copy(a_first, a_last, end() - a_last);
		destroy(a_last, end());
		m_end -= a_last - a_first;
	}

	iterator erase(const_iterator a_position) {
		erase(a_position, a_position + 1);
	}
	
private:
	pointer m_begin;
	pointer m_end;
	pointer m_memory_end;
	allocator_type m_allocator;

	pointer allocate(size_type a_size) {
		size_type old_size = size();
		pointer new_begin = m_allocator.allocate(a_size);
		m_begin = new_begin;
		m_end = m_begin + old_size;
		m_memory_end = m_begin + a_size;
		return new_begin;
	}

	void construct(const_iterator a_position, const T& a_value) {
		m_allocator.construct(a_position, a_value);
	}

	void destroy(const_iterator a_first, const_iterator a_last) {
		for(const_iterator i = a_first; i != a_last; i++) {
			m_allocator.destroy(i);
			std::cout << i << std::endl;			
		}
	}

	void copy(pointer a_to, pointer a_from, size_type a_size) {
		std::memcpy(a_to, a_from, sizeof(value_type)*a_size);
	}

	void move(pointer a_to, pointer a_from, size_type a_size) {
		std::memmove(a_to, a_from, sizeof(value_type)*a_size);
	}
};
