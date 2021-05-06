#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <utility>
#include <fstream>
#include <iostream>
#include <stdexcept>
// #include "cell.h"

using std::vector;
using std::string;
using std::unordered_map;
using std::pair;
using std::cout;
using std::ostream;
using std::ifstream;


class CSV_Table {
 public:
  CSV_Table(string filename, char delim = ',');
  void evaluate();
  void print(ostream& out = cout);

 private:
struct Cell {
  string raw;
  int64_t processed;
  bool visited;
  bool is_processed;
  int64_t evaluate(CSV_Table& table, pair<int64_t, int64_t> address);
  int64_t evaluate_operand(
  const string& raw_string,
  CSV_Table& table,
  size_t operation_position);
};
  vector<vector<Cell>> data;
  unordered_map<string, int64_t> columns;
  unordered_map<int64_t, int64_t> rows;
  string columns_to_print;
  vector<int64_t> rows_to_print;

 private:
  void read_columns_names(const string& line);
  void read_line(const string& line, int64_t row_number);
  pair<int64_t, int64_t> get_cell_address(const string& raw_address);

  friend class Cell;
};
// #include "table.inl"
