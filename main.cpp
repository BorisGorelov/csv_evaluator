#include "table.hpp"

int main(int argc, char** argv) {
  CSV_Table table(argv[1]);
  table.evaluate();
  table.print();
}
// g++ -o csv_evaluator *.cpp -std=c++17 -Wall
