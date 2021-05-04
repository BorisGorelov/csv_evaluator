#include "table.hpp"

void Table::read_columns_names(const string& line) {
  string column_name = "";
  int64_t columns_number = 0;
  for (char i : line) {
    if (i != ',')
      column_name += i;
    else
      if (!column_name.empty()) {
        // if columns.count(row) != 0 throw invalid_arg
        columns.insert({column_name, columns_number++});
        column_name = "";
      }
  }
}

pair<string, int64_t> get_cell_address(const string& line) {
  size_t first_digit = line.find_first_of("0123456789");
  string column = line.substr(0, first_digit);
  int64_t row = stoll(line.substr(first_digit));
  return {column, row};
}

Table::Table(string filename, char delim = ',') {
  ifstream file(filename);
  if (!file.is_open())
    throw std::invalid_argument("file has not been found\n");

  if (file.peek() != ',')
    throw std::invalid_argument("comma was expected as first character");

  string line = "", cell = "";
  getline(file, line);
  read_columns_names(line);

  bool is_row_number;
  int64_t row_number;
  int64_t row_counter = 0;

  while (getline(file, line)) {
    is_row_number = true;
    raw_data.push_back({});
    for (char i : line) {
      if (i != ',')
        cell += i;
      else {
        if (is_row_number) {
          row_number = std::stoll(cell);
          rows.insert({row_number, row_counter++});
          is_row_number = false;
        } else {
          raw_data.back().push_back(std::move(cell));
        }
        cell = "";
      }
    }
  }

  if (rows.size() != raw_data.size())
    throw std::invalid_argument("row adress has been missed\n");

  for (const auto& row : raw_data)  
    if (row.size() != columns.size())
      throw std::invalid_argument("empty cell has been found\n");

  processed_data.assign(raw_data.size(), vector<pair<int64_t, bool>>(raw_data[0].size(), {0, 0}));
}

int64_t Table::evaluate_cell(pair<string, int64_t> address) {
  if (processed_data[rows[address.second]][columns[address.first]].second == true)
    throw std::logic_error("circular reference has been found");
  processed_data[rows[address.second]][columns[address.first]].second = true;

  string cell = raw_data[rows[address.second]][columns[address.first]];
  if (cell[0] == '=') {
    size_t operation_position = cell.find_first_of("/*-+");
    if (operation_position == cell.npos)
      throw std::invalid_argument("operation has not been found");
    char operation = cell[operation_position];

    pair<string, int64_t> address1 = get_cell_address(cell.substr(1, operation_position - 1));
    pair<string, int64_t> address2 = get_cell_address(cell.substr(operation_position));

    if (operation == '+')
      return evaluate_cell
  }
}

void Table::evaluate() {
  for (const auto& row : raw_data) {
    for (const auto& cell : row) {
      
    }
  }
}

void print(ostream& out = cout) {

}
