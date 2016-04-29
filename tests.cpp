#include <vector>
#include <gtest/gtest.h>
#include <memory>
#include <random>
#include <chrono>
#include <algorithm>
#include "vector.h"
#include "allocator.h"
#include "sort.h"

class Class {
public:
	static const int default_value = 11;
	static int count;
	int value;
	Class():   value(Class::default_value) {++Class::count;}
	Class(int a)          : value(a)       {++Class::count;}
	Class(const Class& b) : value(b.value) {++Class::count;}
	Class(Class&& b) {
		value = b.value;
		++Class::count;
		b.value = 0;
	}
	int get_value() const {return value;}
	bool operator==(const Class& b) const {return value == b.get_value();}
	Class& operator=(const Class& b) {value = b.value; return *this;}
	Class operator*(const Class& b) {return Class(value * b.value);}
	Class operator*(int a) {return Class(value * a);}
	bool operator>(const Class& b) {return value > b.get_value();}
	bool operator<(const Class& b) {return value < b.get_value();}
	~Class() {--Class::count;}
};

int Class::count = 0;

class VectorTest        : public ::testing::Test {};
class AllocatorTest     : public ::testing::Test {};
class TestBasic         : public VectorTest {};
class TestIterators     : public VectorTest {};
class TestCapacity      : public VectorTest {};
class TestElementAccess : public VectorTest {};
class TestModifiers     : public VectorTest {};

typedef int TType;
typedef Vector<TType, Allocator<TType>> Result;
typedef std::vector<TType, Allocator<TType>> Expect;
const int SIZE = 350;
std::random_device rd;






void compare_vectors(const Expect& expect, const Result& result) {
	ASSERT_EQ(expect.size(), result.size());
	for(typename Expect::size_type i; i < expect.size(); i++) {
		ASSERT_EQ(expect.at(i), result.at(i));
	}
}

void compare_vectors_br(const Expect& expect, const Result& result) {
	ASSERT_EQ(expect.size(), result.size());
	for(typename Expect::size_type i; i < expect.size(); i++) {
		ASSERT_EQ(expect[i], result[i]);
	}
}

void compare_vectors(const Result& result_1, const Result& result_2) {
	ASSERT_EQ(result_1.size(), result_2.size());
	ASSERT_NE(&result_1, &result_2);
	for(typename Result::size_type i; i < result_1.size(); i++) {
		ASSERT_EQ(result_1.at(i), result_2.at(i));
	}
}

void compare_vectors(const Expect& expect_1, const Expect& expect_2) {
	ASSERT_EQ(expect_1.size(), expect_2.size());
	ASSERT_NE(&expect_1, &expect_2);
	for(typename Expect::size_type i; i < expect_1.size(); i++) {
		ASSERT_EQ(expect_1.at(i), expect_2.at(i));
	}
}

void asc_ordered_fill(Result& result) {
	for(typename Result::size_type i = 0; i < result.size(); i++) {
		result[i] = i;
	}
}

void random_fill(Expect& expect) {
	for(typename Expect::size_type i = 0; i < expect.size(); i++) {
		expect[i] = rd();
	}
}

void random_fill(Result& result) {
	for(typename Result::size_type i = 0; i < result.size(); i++) {
		result[i] = rd();
	}
}






TEST_F(TestIterators, BEGIN_END) {
	Class::count = 0;
	Result result(SIZE, 0);
	random_fill(result);
	ASSERT_EQ(result.begin() + SIZE, result.end());
	ASSERT_EQ(*(result.begin() + SIZE), *result.end());
	result.clear();
	ASSERT_EQ(0, Class::count);
}

TEST_F(TestIterators, REVERSE_ITERATOR) {
	Class::count = 0;
	Result result(SIZE, 0);
	ASSERT_EQ(result.rbegin() + SIZE, result.rend());
	result.clear();
	ASSERT_EQ(0, Class::count);
}






TEST_F(TestBasic, DEFAULT_CONSTRUCTOR) {
	Class::count = 0;
	Expect expect;
	Result result;
	compare_vectors(expect, result);
	ASSERT_EQ(0, Class::count);
}

TEST_F(TestBasic, SIZE_CONSTRUCTOR_1) {
	Class::count = 0;
	Expect expect(0);
	Result result(0);
	compare_vectors(expect, result);
	ASSERT_EQ(0, Class::count);
}

TEST_F(TestBasic, SIZE_CONSTRUCTOR_2) {
	Class::count = 0;
	Expect expect(10000);
	Result result(10000);
	compare_vectors(expect, result);
	result.clear();
	expect.clear();
	ASSERT_EQ(0, Class::count);
}

TEST_F(TestBasic, FILL_CONSTRUCTOR_1) {
	Class::count = 0;
	Expect expect(10, 11);
	Result result(10, 11);
	compare_vectors(expect, result);
	result.clear();
	expect.clear();
	ASSERT_EQ(0, Class::count);
}

TEST_F(TestBasic, FILL_CONSTRUCTOR_2) {
	Class::count = 0;
	Expect expect(0, 0);
	Result result(0, 0);
	compare_vectors(expect, result);
	result.clear();
	expect.clear();
	ASSERT_EQ(0, Class::count);
}

TEST_F(TestBasic, FILL_CONSTRUCTOR_3) {
	Class::count = 0;
	Expect expect(1, 1);
	Result result(1, 1);
	compare_vectors(expect, result);
	result.clear();
	expect.clear();
	ASSERT_EQ(0, Class::count);
}

TEST_F(TestBasic, FILL_CONSTRUCTOR_4) {
	Class::count = 0;
	Expect expect(10000, -1);
	Result result(10000, -1);
	compare_vectors(expect, result);
	result.clear();
	expect.clear();
	ASSERT_EQ(0, Class::count);
}

TEST_F(TestBasic, INITIALIZER_LIST_CONSTRUCTOR_1) {
	Class::count = 0;
	Expect expect = {1, 4, 2, 923, 453, 4, 674, 78, 5, 58, 324, 534, 55345, 54};
	Result result = {1, 4, 2, 923, 453, 4, 674, 78, 5, 58, 324, 534, 55345, 54};
	compare_vectors(expect, result);
	result.clear();
	expect.clear();
	ASSERT_EQ(0, Class::count);
}

TEST_F(TestBasic, INITIALIZER_LIST_CONSTRUCTOR_2) {
	Class::count = 0;
	Expect expect = {1};
	Result result = {1};
	compare_vectors(expect, result);
	result.clear();
	expect.clear();
	ASSERT_EQ(0, Class::count);
}

TEST_F(TestBasic, RANGE_CONSTRUCTOR_1) {
	Class::count = 0;
	Expect expect(1000, 0);
	random_fill(expect);
	Result result(expect.begin(), expect.end());
	compare_vectors(expect, result);
	result.clear();
	expect.clear();
	ASSERT_EQ(0, Class::count);
}

TEST_F(TestBasic, RANGE_CONSTRUCTOR_2) {
	Class::count = 0;
	Expect expect;
	random_fill(expect);
	Result result(expect.begin(), expect.end());
	compare_vectors(expect, result);
	result.clear();
	expect.clear();
	ASSERT_EQ(0, Class::count);
}

TEST_F(TestBasic, ASSIGN_1) {
	Class::count = 0;
	Result result_1(1000, 0);
	random_fill(result_1);
	Result result_2;
	result_2 = result_1;
	compare_vectors(result_1, result_2);
	result_1.clear();
	result_2.clear();
	ASSERT_EQ(0, Class::count);
}

TEST_F(TestBasic, ASSIGN_2) {
	Class::count = 0;
	Result result_1;
	random_fill(result_1);
	Result result_2;
	result_2 = result_1;
	compare_vectors(result_1, result_2);
	result_1.clear();
	result_2.clear();
	ASSERT_EQ(0, Class::count);
}

TEST_F(TestBasic, ASSIGN_3) {
	Class::count = 0;
	Result result_1(0, 0);
	random_fill(result_1);
	Result result_2(1000, 0);
	random_fill(result_2);
	result_2 = result_1;
	compare_vectors(result_1, result_2);
	result_1.clear();
	result_2.clear();
	ASSERT_EQ(0, Class::count);
}

TEST_F(TestBasic, ASSIGN_4) {
	Class::count = 0;
	Result result_1(1000, 0);
	random_fill(result_1);
	Result result_2(1, 0);
	random_fill(result_2);
	result_2 = result_1;
	compare_vectors(result_1, result_2);
	result_1.clear();
	result_2.clear();
	ASSERT_EQ(0, Class::count);
}






TEST_F(TestCapacity, SIZE_1) {
	Class::count = 0;
	Result result(0);
	ASSERT_EQ(0, result.size());
	result.clear();
	ASSERT_EQ(0, result.size());
	ASSERT_EQ(0, Class::count);
}

TEST_F(TestCapacity, SIZE_2) {
	Class::count = 0;
	Result result(10000);
	ASSERT_EQ(10000, result.size());
	result.clear();
	ASSERT_EQ(0, result.size());
	ASSERT_EQ(0, Class::count);
}

TEST_F(TestCapacity, EMPTY_1) {
	Class::count = 0;
	Result result;
	ASSERT_TRUE(result.empty());
}

TEST_F(TestCapacity, EMPTY_2) {
	Class::count = 0;
	Result result{1, 2, 3, 4, 5};
	result.clear();
	ASSERT_TRUE(result.empty());
}

TEST_F(TestCapacity, RESERVE) {
	Class::count = 0;
	Expect expect;
	Result result;
	expect.reserve(2000);
	result.reserve(2000);
	ASSERT_EQ(2000, result.capacity());
	ASSERT_EQ(expect.capacity(), result.capacity());
	result.clear();
	ASSERT_EQ(0, Class::count);
}

TEST_F(TestCapacity, RESIZE) {
	Class::count = 0;
	Expect expect;
	Result result;
	expect.resize(100);
	result.resize(100);
	compare_vectors(expect, result);
	expect.resize(5000);
	result.resize(5000);
	compare_vectors(expect, result);
	expect.resize(50);
	result.resize(50);
	compare_vectors(expect, result);
	expect.clear();
	result.clear();
	ASSERT_EQ(0, Class::count);
}






TEST_F(TestElementAccess, FRONT_1) {
	Class::count = 0;
	Expect expect(1000, 0);
	random_fill(expect);
	Result result(expect.begin(), expect.end());
	ASSERT_EQ(expect.front(), result.front());
	expect.clear();
	result.clear();
	ASSERT_EQ(0, Class::count);
}

TEST_F(TestElementAccess, FRONT_2) {
	Class::count = 0;
	Expect expect(1);
	random_fill(expect);
	Result result(expect.begin(), expect.end());
	ASSERT_EQ(expect.front(), result.front());
	expect.clear();
	result.clear();
	ASSERT_EQ(0, Class::count);
}

TEST_F(TestElementAccess, FRONT_3) {
	Class::count = 0;
	Expect expect(1);
	random_fill(expect);
	Result result(expect.begin(), expect.end());
	auto value = expect.front();
	expect.front() = value*2;
	result.front() = value*2;
	compare_vectors(expect, result);
	expect.clear();
	result.clear();
	ASSERT_EQ(1, Class::count);
}

TEST_F(TestElementAccess, BACK_1) {
	Class::count = 0;
	Expect expect(1000, 0);
	random_fill(expect);
	Result result(expect.begin(), expect.end());
	ASSERT_EQ(expect.back(), result.back());
	expect.clear();
	result.clear();
	ASSERT_EQ(0, Class::count);
}

TEST_F(TestElementAccess, BACK_2) {
	Class::count = 0;
	Expect expect(1);
	random_fill(expect);
	Result result(expect.begin(), expect.end());
	ASSERT_EQ(expect.back(), result.back());
	expect.clear();
	result.clear();
	ASSERT_EQ(0, Class::count);
}

TEST_F(TestElementAccess, BACK_3) {
	Class::count = 0;
	Expect expect(1);
	random_fill(expect);
	Result result(expect.begin(), expect.end());
	auto value = expect.back();
	expect.back() = value*2;
	result.back() = value*2;
	compare_vectors(expect, result);
	expect.clear();
	result.clear();
	ASSERT_EQ(1, Class::count);
}

TEST_F(TestElementAccess, AT_1) {
	Class::count = 0;
	Expect expect(1000, 0);
	random_fill(expect);
	Result result(expect.begin(), expect.end());
	compare_vectors(expect, result);
	expect.clear();
	result.clear();
	ASSERT_EQ(0, Class::count);
}

TEST_F(TestElementAccess, AT_2) {
	Class::count = 0;
	Expect expect(1000, 0);
	random_fill(expect);
	Result result(expect.begin(), expect.end());
	int index = rd()%expect.size();
	auto value = expect.at(index);
	expect.at(index) = value*2;
	result.at(index) = value*2;
	compare_vectors(expect, result);
	expect.clear();
	result.clear();
	ASSERT_EQ(1, Class::count);
}

TEST_F(TestElementAccess, BRACES_1) {
	Class::count = 0;
	Expect expect(1000, 0);
	random_fill(expect);
	Result result(expect.begin(), expect.end());
	compare_vectors_br(expect, result);
	expect.clear();
	result.clear();
	ASSERT_EQ(0, Class::count);
}

TEST_F(TestElementAccess, BRACES_2) {
	Class::count = 0;
	Expect expect(1000, 0);
	random_fill(expect);
	Result result(expect.begin(), expect.end());
	int index = rd()%expect.size();
	auto value = expect.at(index);
	expect[index] = value*2;
	result[index] = value*2;
	compare_vectors(expect, result);
	expect.clear();
	result.clear();
	ASSERT_EQ(1, Class::count);
}






TEST_F(TestModifiers, INSERT) {
	Class::count = 0;
	Expect expect;
	Result result;
	for(int i = 0; i < 1000; i++) {
		TType value(rd());
		size_t index = i == 0 ? 0 : rd()%expect.size();
		auto ie = expect.insert(expect.begin() + index, value);
		auto ir = result.insert(result.begin() + index, value);
		ASSERT_EQ(expect.end() - ie, result.end() - ir);
	}
	compare_vectors(expect, result);
	expect.clear();
	result.clear();
	ASSERT_EQ(0, Class::count);
}

TEST_F(TestModifiers, CLEAR) {
	Class::count = 0;
	Result result(1000);
	random_fill(result);
	result.clear();
	ASSERT_EQ(0, result.size());
	ASSERT_EQ(0, Class::count);
}

TEST_F(TestModifiers, EMPLACE) {
	Class::count = 0;
	Expect expect;
	Result result;
	for(int i = 0; i < 1000; i++) {
		int value = rd();
		size_t index = i == 0 ? 0 : rd()%expect.size();
		auto ie = expect.emplace(expect.begin() + index, value);
		auto ir = result.emplace(result.begin() + index, value);
		ASSERT_EQ(expect.end() - ie, result.end() - ir);
	}
	compare_vectors(expect, result);
	expect.clear();
	result.clear();
	ASSERT_EQ(0, Class::count);
}

TEST_F(TestModifiers, PUSH_BACK) {
	Class::count = 0;
	Expect expect;
	Result result;
	for(int i = 0; i < 1000; i++) {
		int value = rd();
		expect.push_back(value);
		result.push_back(value);
	}
	compare_vectors(expect, result);
	expect.clear();
	result.clear();
	ASSERT_EQ(0, Class::count);
}

TEST_F(TestModifiers, ERASE) {
	Class::count = 0;
	Expect expect;
	Result result;
	for(int i = 0; i < 1000; i++) {
		int value = rd();
		expect.push_back(value);
		result.push_back(value);
	}
	for(int i = 0; i < 500; i++) {
		int index = rd()%expect.size();
		auto ie = expect.erase(expect.begin() + index);
		auto ir = result.erase(result.begin() + index);
		ASSERT_EQ(expect.end() - ie, result.end() - ir);
	}
	compare_vectors(expect, result);
	ASSERT_EQ(1000, Class::count);
	expect.clear();
	result.clear();
	ASSERT_EQ(0, Class::count);
}

TEST_F(TestModifiers, POP_BACK) {
	Class::count = 0;
	Expect expect;
	Result result;
	for(int i = 0; i < 1000; i++) {
		int value = rd();
		expect.push_back(value);
		result.push_back(value);
	}
	for(int i = 0; i < 500; i++) {
		expect.pop_back();
		result.pop_back();
	}
	compare_vectors(expect, result);
	ASSERT_EQ(1000, Class::count);
	expect.clear();
	result.clear();
	ASSERT_EQ(0, Class::count);
}




using std::cout;
using std::endl;

int main(int argc, char** argv) {
	// ::testing::InitGoogleTest(&argc, argv);
	// return RUN_ALL_TESTS();
	auto size_list = {100, 1000, 10000, 100000, 1000000, 3000000};
	auto start = std::chrono::system_clock::now();
	auto end = std::chrono::system_clock::now();
	for(auto size : size_list) {
		Expect sample(size);
		random_fill(sample);
		Expect expect(sample.begin(), sample.end());
		Result result(sample.begin(), sample.end());

		start = std::chrono::system_clock::now();
		custom::sort(expect.begin(), expect.end());
		end = std::chrono::system_clock::now();
		cout << 
			(end - start).count() << " (" <<
			std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() 
			<< "ms.) custom::sort; std::vector; random fill. N = " << size << "\n";

		start = std::chrono::system_clock::now();
		custom::sort(expect.rbegin(), expect.rend());
		end = std::chrono::system_clock::now();
		cout << 
			(end - start).count() << " (" <<
			std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() 
			<< "ms.) custom::sort; std::vector; reverse order. N = " << size << "\n";

		start = std::chrono::system_clock::now();		
		custom::sort(result.begin(), result.end());
		end = std::chrono::system_clock::now();
		cout << 
			(end - start).count() << " (" <<
			std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() 
			<< "ms.) custom::sort; Vector; same fill. N = " << size << "\n";

		start = std::chrono::system_clock::now();		
		custom::sort(result.rbegin(), result.rend());
		end = std::chrono::system_clock::now();
		cout << 
			(end - start).count() << " (" <<
			std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() 
			<< "ms.) custom::sort; Vector; reverse. N = " << size << "\n";

		cout << endl;

		expect = Expect(sample.begin(), sample.end());
		result = Result(sample.begin(), sample.end());

		start = std::chrono::system_clock::now();
		std::sort(expect.begin(), expect.end());
		end = std::chrono::system_clock::now();
		cout << 
			(end - start).count() << " (" <<
			std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() 
			<< "ms.) std::sort; std::vector; random fill. N = " << size << "\n";

		start = std::chrono::system_clock::now();
		std::sort(expect.rbegin(), expect.rend());
		end = std::chrono::system_clock::now();
		cout << 
			(end - start).count() << " (" <<
			std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() 
			<< "ms.) std::sort; std::vector; reverse order. N = " << size << "\n";

		start = std::chrono::system_clock::now();		
		std::sort(result.begin(), result.end());
		end = std::chrono::system_clock::now();
		cout << 
			(end - start).count() << " (" <<
			std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() 
			<< "ms.) std::sort; Vector; same fill. N = " << size << "\n";

		start = std::chrono::system_clock::now();		
		std::sort(result.rbegin(), result.rend());
		end = std::chrono::system_clock::now();
		cout << 
			(end - start).count() << " (" <<
			std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() 
			<< "ms.) std::sort; Vector; reverse order. N = " << size << "\n";

		cout << endl;
	}

	return 0;
}
