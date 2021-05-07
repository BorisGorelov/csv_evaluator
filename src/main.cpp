#include "table.hpp"
#include "test_runner.h"

void Test1() {
  CSV_Table<int64_t> table("src/test/test1_default.csv");
  table.evaluate();
  std::ostringstream oss;
  table.print(oss);
  ASSERT_EQUAL(",A,B,Cell\n1,1,0,1\n2,2,6,0\n30,0,1,5\n", oss.str());
}

void Test2() {
  std::ostringstream oss;
  try {
    CSV_Table<int64_t> table("src/test/test2_cyclic.csv");
    table.evaluate();
  } catch (std::exception& e) {
    oss << e.what();
  }
  ASSERT_EQUAL("adress: 2, 1: cyclic reference has been found\n", oss.str());
}

void Test3() {
  CSV_Table<float> table("src/test/test3_float.csv");
  table.evaluate();
  std::ostringstream oss;
  table.print(oss);
  ASSERT_EQUAL(",A,B,Cell,asd\n1,1.200,0.000,1.000,15.500\n2,2.000,6.200,0.000,14.500\n30,0.000,10.000,5.000,13.500\n", oss.str());
}

void Test4() {
  std::ostringstream oss;
  try {
    CSV_Table<int64_t> table("src/test/test4_bad_cell.csv");
    table.evaluate();
  } catch (std::exception& e) {
    oss << e.what();
  }
  ASSERT_EQUAL("adress: 1, 0: cell is consist of invalid characters\n", oss.str());
}

void Test5() {
  std::ostringstream oss;
  try {
    CSV_Table<int64_t> table("src/test/test5_double_cell_name.csv");
    table.evaluate();
  } catch (std::exception& e) {
    oss << e.what();
  }
  ASSERT_EQUAL("second column name 'A' has been found\n", oss.str());
}

int main(int argc, char** argv) {
  TestRunner tr;
  RUN_TEST(tr, Test1);
  RUN_TEST(tr, Test2);
  RUN_TEST(tr, Test3);
  RUN_TEST(tr, Test4);
  RUN_TEST(tr, Test5);

  try {
    CSV_Table<int64_t> table(argv[1]);
    table.evaluate();
    table.print();
  } catch (std::exception& e) {
    cerr << e.what() << endl;
  }
}
// g++ -o csv_evaluator src/main.cpp -std=c++17 -Wall -O3 -march=native -fsanitize=address

