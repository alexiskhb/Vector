#pragma once

#include <iterator>
#include <algorithm>
#include <limits>
#include <memory>
#include <iostream>
#include <exception>
#include "allocator.h"


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
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

private:
	pointer m_memory_begin;
	pointer m_end;
	pointer m_memory_end;
	allocator_type m_allocator;
	static const size_type allocate_multiplier = 8;

public:
	Vector() : m_memory_begin(nullptr), m_end(nullptr), m_memory_end(nullptr) {
		init_allocate_and_set_size(allocate_multiplier);
		m_end = m_memory_begin;
	}

	Vector(size_type a_size) {
		init_allocate_and_set_size(a_size);
		construct(m_memory_begin, m_end, T());
	}

	// Fill constructor
	Vector(size_type a_size, const_reference a_value, const allocator_type& alloc = allocator_type()) : m_allocator(alloc) {
		init_allocate_and_set_size(a_size);
		construct(m_memory_begin, m_end, a_value);
	}

	Vector(std::initializer_list<value_type> il, const allocator_type& alloc = allocator_type()) : m_allocator(alloc) {
		init_allocate_and_set_size(il.size());
		for(const_iterator i = begin(), v = il.begin(); i < end(); i++, v++) {
			construct(i, *v);
		}
	}

	// Range constructor.
	// It conflicts with fill constructor, so we should
	// check if parameters are actually iterators
	template <class InputIterator>
	Vector(InputIterator a_first, InputIterator a_last, const allocator_type& alloc = allocator_type()) : m_allocator(alloc) {
		toggle<is_iterator<InputIterator>::value> is_range_constructor;
		construct_range_not_fill(is_range_constructor, a_first, a_last);
	}

	Vector& operator=(const Vector& other) {
		destroy(begin(), end());
		if (capacity() < other.size()) {
			this->allocate(allocate_multiplier*other.size());
		}
		m_end = m_memory_begin;
		for(const_reference v: other) {
			push_back(v);
		}
		return *this;
	}

	~Vector() {
		destroy(begin(), end());
		m_allocator.deallocate(begin(), capacity());
	}

// Iterators

	iterator begin() {
		return m_memory_begin;
	}

	const_iterator begin() const {
		return m_memory_begin;
	}

	iterator end() {
		return m_end;
	}

	const_iterator end() const {
		return m_end;
	}

	reverse_iterator rbegin() {
		return reverse_iterator(end());
	}

	const_reverse_iterator rbegin() const {
		return const_reverse_iterator(end());
	}

	reverse_iterator rend() {
		return reverse_iterator(begin());
	}

	const_reverse_iterator rend() const {
		return const_reverse_iterator(begin());
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
		std::copy(old_begin, old_begin + old_size, new_begin);
		destroy(old_begin, old_begin + old_size);
		m_allocator.deallocate(old_begin, old_capacity);
		m_memory_begin = new_begin;
		m_end = new_begin + old_size;
		m_memory_end = m_memory_begin + a_size;
	}

	void resize(size_type a_size) {
		if (a_size <= capacity()) {
			destroy(begin() + a_size, end());
			m_end = m_memory_begin + a_size;
			return;
		}
		iterator old_begin = begin();
		iterator old_end = end();
		size_type old_size = size();
		size_type old_capacity = capacity();
		this->allocate(allocate_multiplier*a_size);
		m_end = m_memory_begin + a_size;
		std::copy(old_begin, old_end, m_memory_begin);
		this->construct(m_memory_begin + old_size, m_end, T());
		this->destroy(old_begin, old_end);
		m_allocator.deallocate(old_begin, old_capacity);
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

	reference at(size_type a_index) {
		if (a_index >= size()) {
			throw std::out_of_range("custom vector out of range");
		}
		return *(begin() + a_index);
	}

	const_reference at(size_type a_index) const{
		if (a_index >= size()) {
			throw std::out_of_range("custom vector out of range");
		}
		return *(begin() + a_index);
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
	iterator erase(iterator a_first, iterator a_last) {
		// should I destroy them?
		// destroy(a_first, a_last);
		std::move(a_last, end(), a_first);
		destroy(end()-(a_last-a_first), end());
		m_end -= a_last - a_first;
	}

	iterator erase(iterator a_position) {
		erase(a_position, a_position + 1);
	}
	
	void pop_back() {
		erase(end() - 1);
	}

private:
	template<class I>
	struct is_iterator {
		template <class U> static char check(typename std::iterator_traits<U>::pointer* x);
		template <class U> static long check(U* x);

		static const bool value = sizeof(check<I>(nullptr)) == sizeof(char);
	};

	template<bool B>
	struct toggle {};

	// Range constructor
	template<class U>
	void construct_range_not_fill(toggle<true>, U a_first, U a_last) {
		init_allocate_and_set_size(allocate_multiplier);
		m_end = m_memory_begin;
		for(U i = a_first; i != a_last; i++) {
			push_back(*i);
		}
	}

	// Fill constructor
	template<class U>
	void construct_range_not_fill(toggle<false>, U a_size, U a_value) {
		init_allocate_and_set_size(a_size);
		construct(m_memory_begin, m_end, a_value);
	}

	pointer allocate(size_type a_size) {
		m_memory_begin = m_allocator.allocate(a_size);
		m_memory_end   = m_memory_begin + a_size;
		return m_memory_begin;
	}

	void construct(const_iterator a_position, const_reference a_value) {
		m_allocator.construct(a_position, a_value);
	}

	// There is no check if a_last < a_first,
	// and we know that iterator is random access one
	// so we use '<' in 'for' condition
	void construct(const_iterator a_first, const_iterator a_last, const_reference a_value) {
		for(const_iterator i = a_first; i < a_last; i++) {
			m_allocator.construct(i, a_value);
		}
	}

	void destroy(const_iterator a_first, const_iterator a_last) {
		for(const_iterator i = a_first; i < a_last; i++) {
			m_allocator.destroy(i);
		}
	}

	iterator rshift(iterator a_position, size_type a_count) {
		size_type old_size = size();
		if (old_size + a_count <= capacity()) {
			if (a_position < m_end) {
				std::move(a_position, end(), a_position + a_count);
				destroy(a_position, std::min(end(), a_position + a_count));	
			}
		} else {
			size_type index = a_position - begin();
			iterator old_begin = begin();
			iterator old_end = end();
			size_type old_capacity = capacity();
			iterator new_begin = this->allocate(allocate_multiplier*(old_size + a_count));
			a_position = new_begin + index;
			if (index != 0) {
				std::copy(old_begin, old_begin + index, new_begin);
			}
			if (old_size > index) {
				std::copy(old_begin + index, old_begin + old_size, a_position);
			}
			destroy(old_begin, old_end);
			m_allocator.deallocate(old_begin, old_capacity);
		}
		m_end = begin() + old_size + a_count;
		return a_position;
	}

	void init_allocate_and_set_size(size_type a_size) {
		m_memory_begin = m_end = m_memory_end = nullptr;
		this->allocate(allocate_multiplier*a_size);
		m_end = m_memory_begin + a_size;
		m_memory_end = m_memory_begin + allocate_multiplier*a_size;
	}
};

