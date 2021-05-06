#include "table.hpp"

void CSV_Table::read_columns_names(const string& line) {
  string column_name = "";
  int64_t columns_number = 0;
  for (char i : line) {
    if (i != ',')
      column_name += i;
    else
      if (!column_name.empty()) {
        // if columns.count(row) != 0 throw invalid_arg
        columns.insert({std::move(column_name), columns_number++});
        column_name = "";
      }
  }
  if (!column_name.empty())
    columns.insert({std::move(column_name), columns_number});
}

void CSV_Table::read_line(const string& line, int64_t row_number) {
  size_t first_comma = line.find_first_of(',');
  if (first_comma == line.npos || first_comma == 0)
    throw std::invalid_argument("row number has been missed");

  int64_t initial_number = std::stoll(line.substr(0, first_comma));
  rows.insert({initial_number, row_number});
  rows_to_print.push_back(initial_number);

  string cell = "";
  for (char i : string{line.begin() + first_comma + 1, line.end()}) {
    if (i != ',') {
      cell += i;
    } else if (!cell.empty()) {
      data.back().push_back(Cell{std::move(cell), 0, 0, 0});
      cell = "";
    }
  }
  if (!cell.empty())
    data.back().push_back(Cell{std::move(cell), 0, 0, 0});
}

CSV_Table::CSV_Table(string filename, char delim) {
  ifstream file(filename);
  if (!file.is_open())
    throw std::invalid_argument("file has not been found\n");

  if (file.peek() != ',')
    throw std::invalid_argument("comma was expected as first character");

  string line = "", cell = "";
  getline(file, line);
  columns_to_print = line;
  read_columns_names(line);

  int64_t row_number = 0;
  while (getline(file, line)) {
    data.push_back({});
    read_line(line, row_number++);
  }

  if (rows.size() != data.size())
    throw std::invalid_argument("row adress has been missed\n");

  for (const auto& row : data)
    if (row.size() != columns.size())
      throw std::invalid_argument("empty cell has been found\n");
}

pair<int64_t, int64_t> CSV_Table::get_cell_address(const string& raw_address) {
  size_t first_digit = raw_address.find_first_of("0123456789");
  string column = raw_address.substr(0, first_digit);
  int64_t row = stoll(raw_address.substr(first_digit));
  return {rows[row], columns[column]};
}

int64_t apply_operation(int64_t first, char operation, int64_t second) {
  if (operation == '/' && second == 0)
    throw std::logic_error("zero division\n");
  if (operation == '+')
    return first + second;
  if (operation == '-')
    return first - second;
  if (operation == '*')
    return first * second;
  return first / second;
}

int64_t CSV_Table::Cell::evaluate_operand(
  const string& raw_string,
  CSV_Table& table,
  size_t operation_position
  ) {
  int64_t result;
  if (isdigit(raw_string.at(0))) {
    result = std::stoll(raw_string);
  } else {
    pair<int64_t, int64_t> address =
      table.get_cell_address(raw_string);
    result = evaluate(table, address);
  }
  return result;
}

int64_t CSV_Table::Cell::evaluate(
  CSV_Table& table,
  pair<int64_t, int64_t> address
  ) {
  Cell& cell = table.data[address.first][address.second];
  if (cell.visited == true && cell.is_processed == false)
    throw std::logic_error("cyclic reference has been found\n");

  if (cell.is_processed)
    return cell.processed;

  cell.visited = true;
  string& raw_string = cell.raw;
  int64_t result;

  if (raw_string[0] == '=') {
    size_t operation_position = raw_string.find_first_of("/*-+");
    if (operation_position == raw_string.npos)
      throw std::invalid_argument("operation has not been found");

    char operation = raw_string[operation_position];
    int64_t left = evaluate_operand(
      raw_string.substr(1, operation_position - 1),
      table,
      operation_position);
    int64_t right = evaluate_operand(
      raw_string.substr(operation_position + 1),
      table,
      operation_position);

    result = apply_operation(
      left,
      operation,
      right);
  } else {
    result = std::stoll(raw_string);
    if (std::to_string(result) != raw_string)
      throw std::invalid_argument("cell is consist of invalid characters\n");
  }

  cell.processed = result;
  cell.is_processed = true;

  return result;
}

void CSV_Table::evaluate() {
  for (int64_t i = 0; i < data.size(); ++i)
    for (int64_t j = 0; j < data[i].size(); ++j)
      if (!data[i][j].visited)
        data[i][j].evaluate(*this, {i, j});
}

void CSV_Table::print(ostream& out) {
  out << columns_to_print << '\n';
  for (int64_t i = 0; i < data.size(); ++i) {
    out << rows_to_print[i];
    for (int64_t j = 0; j < data[i].size(); ++j)
      out << ',' << data[i][j].processed;
    out << '\n';
  }
}
