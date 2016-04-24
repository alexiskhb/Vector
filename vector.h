#pragma once

#include <iterator>
#include <algorithm>
#include <limits>
#include <memory>
#include <cstring>
#include "allocator.h"
#include <iostream>



template<class T, class A = std::allocator<T>>
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

	Vector(size_type a_size) {
		m_begin = m_end = m_memory_end = nullptr;
		init_allocate_and_resize(a_size);
		construct(m_begin, m_end, T());
	}

	Vector(std::initializer_list<value_type> il, const allocator_type& alloc = allocator_type()) {
		m_begin = m_end = m_memory_end = nullptr;
		init_allocate_and_resize(il.size());
		for(const_iterator i = begin(), v = il.begin(); i < end(); i++, v++) {
			construct(i, *v);
		}
	}

	Vector& operator=(const Vector& other) {

	}

	~Vector() {
		destroy(begin(), end());
		m_allocator.deallocate(begin(), capacity());
	}

// Iterators

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

// Capacity

	size_type size() const {
		return end() - begin();
	}

	size_type capacity() const {
		return m_memory_end - begin();
	}

	bool empty() const {
		return size() == 0;
	}

	void reserve(size_type a_size) {
		if (capacity() >= a_size) {
			return;
		}
		size_type old_size = size();
		size_type old_capacity = capacity();
		pointer old_begin = begin();
		pointer new_begin = allocate(a_size);
		copy(new_begin, old_begin, old_size);
		destroy(old_begin, old_begin + old_size);
		m_allocator.deallocate(old_begin, old_capacity);
		m_begin = new_begin;
		m_end = new_begin + old_size;
		m_memory_end = m_begin + a_size;
	}

	void resize(size_type a_size) {

	}

// Element access

	reference front() {
		return *begin();
	}

	const_reference front() const {
		return *begin();
	}

	reference back() {
		return *(end() - 1);
	}

	const_reference back() const {
		return *(end() - 1);
	}

	reference operator[](size_type a_index) {
		return *(begin() + a_index);
	}

	const_reference operator[](size_type a_index) const {
		return *(begin() + a_index);
	}

// Modifiers

	iterator insert(iterator a_position, const T& a_value) {
		a_position = rshift(a_position, 1);
		construct(a_position, a_value);
		return a_position;
	}

	template <class... Args>
	iterator emplace(iterator a_position, Args&&... args) {
		return insert(a_position, T(args...));
	}

	void push_back(const T& a_value) {
		insert(end(), a_value);
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
	
	void pop_back() {
		erase(end() - 1);
	}

private:
	pointer m_begin;
	pointer m_end;
	pointer m_memory_end;
	allocator_type m_allocator;

	pointer allocate(size_type a_size) {
		pointer new_begin = m_allocator.allocate(a_size);
		return new_begin;
	}

	void construct(const_iterator a_position, const T& a_value) {
		m_allocator.construct(a_position, a_value);
	}

	void construct(const_iterator a_first, const_iterator a_last, const T& a_value) {
		for(const_iterator i = a_first; i < a_last; i++) {
			m_allocator.construct(i, a_value);
		}
	}

	void destroy(const_iterator a_first, const_iterator a_last) {
		for(const_iterator i = a_first; i < a_last; i++) {
			m_allocator.destroy(i);
		}
	}

	void copy(pointer a_to, pointer a_from, size_type a_size) {
		std::memcpy(a_to, a_from, sizeof(value_type)*a_size);
	}

	void move(pointer a_to, pointer a_from, size_type a_size) {
		std::memmove(a_to, a_from, sizeof(value_type)*a_size);
	}

	iterator rshift(iterator a_position, size_type a_count) {
		size_type index = a_position - begin();
		size_type old_size = size();
		if (old_size + a_count <= capacity()) {
			move(a_position + a_count, a_position, end() - a_position);
			destroy(a_position, a_position + a_count - (a_position + a_count == end() + 1 ? 1 : 0));
		}
		else {
			iterator old_begin = begin();
			iterator old_end = end();
			size_type old_capacity = capacity();
			iterator new_begin = allocate(2*(old_size + a_count));
			a_position = new_begin + index;
			copy(new_begin, old_begin, index);
			copy(a_position, old_begin + index, old_size - index);
			destroy(old_begin, old_end);
			m_allocator.deallocate(old_begin, old_capacity);
		}
		m_end = begin() + old_size + a_count;
		return a_position;
	}

	void init_allocate_and_resize(size_type a_size) {
		m_begin = allocate(2*a_size);
		m_end = m_begin + a_size;
		m_memory_end = m_begin + 2*a_size;
	}
};
