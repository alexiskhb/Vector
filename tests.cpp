#include <vector>
#include <gtest/gtest.h>
#include <vector>
#include "vector.h"
#include "allocator.h"


class Base {
public:
	Base() {
	}

	Base(int a) : val(a) {
	}

	Base(const Base& b) {
		val = b.val;
	}

	~Base() {
	}

	int val;

	int bar() {
	}
};


void foo() {
	std::vector<Base, Allocator<Base>> vector;
	vector.push_back(4);
	std::cout << vector[0].val << std::endl;
}

int main(int argc, char** argv) {
	foo();
	// ::testing::InitGoogleTest(&argc, argv);
	// return RUN_ALL_TESTS();
	return 0;
}