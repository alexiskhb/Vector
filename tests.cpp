#include <vector>
#include <gtest/gtest.h>
#include <vector>
#include "vector.h"
#include "allocator.h"
#include <cstdlib>
#include <memory>

class Class {
public:
	int value;
	Class()               : value(11)      {}
	Class(int a)          : value(a)       {}
	Class(int a, int b)   : value(a + b)   {}
	Class(const Class& b) : value(b.value) {}
	~Class() {}
};

class VectorTest        : public ::testing::Test {};
class AllocatorTest     : public ::testing::Test {};
class TestIterators     : public VectorTest {};
class TestCapacity      : public VectorTest {};
class TestElementAccess : public VectorTest {};
class TestModifiers     : public VectorTest {};

typedef Class TType;
typedef Vector<TType>::iterator TIterator;
typedef Vector<TType> TVector;

TEST_F(TestIterators, BEGIN_END) {
	const int SIZE = 25;
	TVector Vec(SIZE);
	TIterator begin = Vec.begin();
	TIterator end   = Vec.end();
	ASSERT_EQ(begin + SIZE, end);
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
