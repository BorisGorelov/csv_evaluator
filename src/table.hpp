#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <utility>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <exception>
#include <iomanip>

using std::vector;
using std::string;
using std::unordered_map;
using std::pair;
using std::cout;
using std::ostream;

template <class Cell_type>
class CSV_Table {
 public:
   static_assert(
    std::is_convertible_v<Cell_type, uint64_t> ||
    std::is_convertible_v<Cell_type, long double>,
    "Values could be only integers or double"
  );
  CSV_Table(string filename, char delim = ',');
  void evaluate();
  void print(ostream& out = cout);

 private:
  template <class Data_type>
  struct Cell {
    string raw;
    Data_type processed;
    bool visited;
    bool is_processed;
    Data_type evaluate(CSV_Table& table, pair<size_t, size_t> address);
    Data_type evaluate_operand(
      const string& raw_string,
      CSV_Table& table,
      size_t operation_position);
  };

  vector<vector<Cell<Cell_type>>> data;
  unordered_map<string, size_t> columns;
  unordered_map<int64_t, size_t> rows;
  string columns_to_print;
  vector<int64_t> rows_to_print;

 private:
  void read_columns_names(string line);
  void read_line(string line, size_t row_number);
  pair<size_t, size_t> get_cell_address(const string& raw_address);

  friend class Cell<Cell_type>;
};
#include "table.inl"
