
#include "lazy_vector.h"

// The type to be tested on lazy_vector
// TestType allocates memory to test if lazy_vector calls its destructors properly
class TestType {
public:
  TestType() {
    x = new int();
  }
  TestType(const TestType& t) {
    x = new int(*t.x);
  }

  ~TestType() {
    delete x;
  }

  TestType& operator=(const TestType& t) {
    if (t.x != 0) delete x;
    x = new int(*t.x);
  }
  int* x;
};

#define BOOST_TEST_MODULE lazy_vector_tests
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE(constructor_default) {
  lazy_vector<TestType> vec;
  BOOST_CHECK_EQUAL(vec.size(), 0);
}

BOOST_AUTO_TEST_CASE(constructor_1) {
  size_t n = 42;
  lazy_vector<TestType> vec(n);
  BOOST_CHECK_EQUAL(vec.size(), n);
}

BOOST_AUTO_TEST_CASE(constructor_2) {
  int n = 42;
  int val = 84;
  std::vector<int> vec(n, val);
  // check if same as val on first and last
  BOOST_CHECK_EQUAL(vec[0], val); 
  BOOST_CHECK_EQUAL(vec[n-1], val);
}

BOOST_AUTO_TEST_CASE(constructor_3) {
  int n = 42;
  std::initializer_list<int> init_list { 1, 5, 9, 14 };
  std::vector<int> vec = init_list;
  
  BOOST_CHECK_EQUAL(vec.size(), init_list.size());
  int i = 0;
  for (const auto itr: init_list) {
    BOOST_CHECK_EQUAL(vec[i], itr);
    ++i;
  }
}

BOOST_AUTO_TEST_CASE(constructor_cpy) {
  std::initializer_list<int> init_list{ 13, 16, 19 };
  lazy_vector<int>* vec1 = new lazy_vector<int>(init_list);
  lazy_vector<int> vec2(*vec1); // construct vec2 as a copy of vec1

  BOOST_CHECK_EQUAL(vec1->size(), vec2.size());
  BOOST_CHECK_EQUAL(vec1->capacity(), vec2.capacity());

  // check if vec2 can stand on its own when vec1 is gone from memory
  delete vec1;

  int i = 0;
  for (const auto itr : init_list) {
    BOOST_CHECK_EQUAL(vec2[i], itr);
    ++i;
  }
}

BOOST_AUTO_TEST_CASE(constructor_move) {
  std::initializer_list<int> init_list{ 13, 16, 19 };
  lazy_vector<int>* vec1 = new lazy_vector<int>(init_list);
  lazy_vector<int> vec2(std::move(*vec1));

  BOOST_CHECK_EQUAL(vec2.size(), init_list.size());
  BOOST_CHECK_EQUAL(vec1->size(), 0);

  // check if vec2 can stand on its own when vec1 is gone from memory
  delete vec1;

  int i = 0;
  for (const auto itr : init_list) {
    BOOST_CHECK_EQUAL(vec2[i], itr);
    ++i;
  }
}

BOOST_AUTO_TEST_CASE(assignment_move) {
  std::initializer_list<int> init_list{ 13, 16, 19 };
  lazy_vector<int>* vec1 = new lazy_vector<int>(init_list);
  lazy_vector<int> vec2;

  vec2 = std::move(*vec1);

  BOOST_CHECK_EQUAL(vec2.size(), init_list.size());
  BOOST_CHECK_EQUAL(vec1->size(), 0);

  // check if vec2 can stand on its own when vec1 is gone from memory
  delete vec1;

  int i = 0;
  for (const auto itr : init_list) {
    BOOST_CHECK_EQUAL(vec2[i], itr);
    ++i;
  }
}

BOOST_AUTO_TEST_CASE(assignment_operator) {
  std::initializer_list<int> init_list{ 13, 16, 19 };
  lazy_vector<int>* vec1 = new lazy_vector<int>(init_list);
  lazy_vector<int> vec2; // default constructor temporarily
  vec2 = *vec1; // assign vec1 to vec2

  BOOST_CHECK_EQUAL(vec1->size(), vec2.size());
  BOOST_CHECK_EQUAL(vec1->capacity(), vec2.capacity());

  delete vec1;

  // check if vec2 can stand on its own when vec1 is gone from memory
  int i = 0;
  for (const auto itr : init_list) {
    BOOST_CHECK_EQUAL(vec2[i], itr);
    ++i;
  }
}

BOOST_AUTO_TEST_CASE(destructor_calls) {
  // See if lazy_vector calls destructor on the type parameter correctly
  // Test for memory leaks on TestType
  lazy_vector<TestType> vec;

  for (int i = 0; i < 25467; ++i) {
    vec.push_back(TestType());
  }
  for (int i = 0; i < 3254; ++i) {
    vec.pop_back();
  }
  for (int i = 0; i < 2099; ++i) {
    vec.push_back(TestType());
  }
  for (int i = 0; i < 19827; ++i) {
    vec.pop_back();
  }
}

BOOST_AUTO_TEST_CASE(destructor_calls_primitives) {
  // See if lazy_vector calls destructor on the type parameter correctly
  // Test for memory leaks on TestType
  lazy_vector<int> vec;
  for (int i = 0; i < 217; ++i) {
    vec.push_back(0);
  }
  for (int i = 0; i < 150; ++i) {
    vec.pop_back();
  }
  for (int i = 0; i < 199; ++i) {
    vec.push_back(0);
  }
  for (int i = 0; i < 164; ++i) {
    vec.pop_back();
  }
}

BOOST_AUTO_TEST_CASE(iterator_begin) {
  lazy_vector<int> vec = { 1, 5, 9 };
  BOOST_CHECK_EQUAL(vec[0], *vec.begin());
}

BOOST_AUTO_TEST_CASE(iterator_end) {
  lazy_vector<int> vec = { 1, 5, 9 };
  BOOST_CHECK_EQUAL(vec[vec.size()-1], *(vec.end()-1));
}


BOOST_AUTO_TEST_CASE(empty) {
  lazy_vector<int> vec;
  BOOST_CHECK(vec.empty());
  vec.push_back(0);
  BOOST_CHECK(!vec.empty());
}

BOOST_AUTO_TEST_CASE(size) {
  size_t n = 116;
  lazy_vector<int> vec(n);
  BOOST_CHECK_EQUAL(n, vec.size());
}

BOOST_AUTO_TEST_CASE(resize) {
  lazy_vector<int> vec(116);
  size_t new_size = 76;
  vec.resize(76, 0);
  BOOST_CHECK_EQUAL(new_size, vec.size());
}

BOOST_AUTO_TEST_CASE(capacity) {
  //not much to test - should at least be greater/equal to size
  lazy_vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  BOOST_CHECK(vec.size() <= vec.capacity());
}
BOOST_AUTO_TEST_CASE(reserve) {
  lazy_vector<int> vec;
  size_t n = 1000;
  vec.reserve(n);
  BOOST_CHECK(n <= vec.capacity());
}

BOOST_AUTO_TEST_CASE(at) {
  int third_value = 19;
  lazy_vector<int> vec = { 1, 11, third_value, 25, 43 };

  BOOST_CHECK_EQUAL(third_value, vec[2]);
}

BOOST_AUTO_TEST_CASE(rnd_access_operator) {
  int third_value = 19;
  lazy_vector<int> vec = { 1, 11, third_value, 25, 43 };
  
  BOOST_CHECK_EQUAL(third_value, vec[2]);
}

BOOST_AUTO_TEST_CASE(front_element) {
  int first_value = 1;
  lazy_vector<int> vec = { first_value, 11, 19, 25, 43 };
  BOOST_CHECK_EQUAL(first_value, vec.front());
}

BOOST_AUTO_TEST_CASE(back_element) {
  int last_value = 43;
  lazy_vector<int> vec = { 1, 11, 19, 25, last_value };
  BOOST_CHECK_EQUAL(last_value, vec.back());
}

BOOST_AUTO_TEST_CASE(swap) {
  int original_vec1_val = 1;
  int original_vec2_val = 2;
  lazy_vector<int> vec1 = { original_vec1_val };
  lazy_vector<int> vec2 = { original_vec2_val };
  
  lazy_vector<int>::swap(vec1, vec2);
  // values should be swapped
  BOOST_CHECK_EQUAL(original_vec1_val, vec2[0]);
  BOOST_CHECK_EQUAL(original_vec2_val, vec1[0]);
}

BOOST_AUTO_TEST_CASE(clear) {
  lazy_vector<TestType> vec;

  for (int i = 0; i < 20; ++i) {
    vec.push_back(TestType());
  }
  vec.clear();
  BOOST_CHECK_EQUAL(vec.size(), 0);
}

