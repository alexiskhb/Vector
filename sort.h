#pragma once

#include <type_traits>
#include <iterator>
#include <algorithm>

namespace custom {
	template<class I, typename = void>
	struct is_random_access_iterator {
		static const bool value = false;
	};

	template<class I>
	struct is_random_access_iterator<I, typename std::enable_if< std::is_same<
		typename std::iterator_traits<I>::iterator_category, 
		std::random_access_iterator_tag
	>::value >::type> {
		static const bool value = true;
	};

	template<class iterator>
	typename std::enable_if<is_random_access_iterator<iterator>::value, void>::type 
	sort(iterator first, iterator last) {
		if (first >= last) {
			return;
		}
		iterator i = first;
		iterator j = last;
		iterator x = first + rand()%(last - first);
		do {
			while (*i < *x) ++i;		
			while (*j > *x) --j;		
			if (i <= j) {
				std::swap(*i, *j);
				++i;
				--j;
			}
		} while (i < j);
		custom::sort(first, j);
		custom::sort(i, last);
	}
}
