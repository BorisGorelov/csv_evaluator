#include "table.hpp"

int main(int argc, char** argv) {
  Table table(argv[0]);
  table.evaluate();
  table.print();
  return 0;
}