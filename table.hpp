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

class Table {
 public:
  Table(string filename, char delim = ',');
  void evaluate();
  void print(ostream& out = cout);

 private:
  vector<vector<string>> raw_data;
  vector<vector<pair<int64_t, bool>>> processed_data;
  unordered_map<string, int64_t> columns;
  unordered_map<int64_t, int64_t> rows;

  void read_columns_names(const string& line);
  int64_t evaluate_cell(const pair<string, int64_t>& address);
};
