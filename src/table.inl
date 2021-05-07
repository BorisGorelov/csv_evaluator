#include "table.hpp"

template <class Data_type>
void CSV_Table<Data_type>::read_columns_names(string line) {
  string column_name = "";
  int64_t columns_number = 0;
  for (auto it = line.begin(); it != line.end(); ++it) {
    if (*it != ',') {
      column_name += *it;
    } else if (!column_name.empty()) {
      if (columns.count(column_name) != 0)
        throw std::invalid_argument("second column name '" + column_name + "' has been found\n");
      columns.insert({std::move(column_name), columns_number++});
      column_name.clear();
    }
  }
  if (!column_name.empty()) {
    if (columns.count(column_name) != 0)
      throw std::invalid_argument("second column name '" + column_name + "' has been found\n");
    columns.insert({std::move(column_name), columns_number++});
  }
}

template <class Data_type>
void CSV_Table<Data_type>::read_line(string line, size_t row_number) {
  size_t first_comma = line.find_first_of(',');
  if (first_comma == line.npos || first_comma == 0)
    throw std::out_of_range("row number has been missed (line: " +
                            std::to_string(row_number) + ")\n");

  string str_initial_numder = line.substr(0, first_comma);
  int64_t initial_number = std::stoll(str_initial_numder);

  if (std::to_string(initial_number) != str_initial_numder)
    throw std::invalid_argument("row number '" +
                                str_initial_numder +
                                "' is consist of invalid characters\n");

  if (rows.count(initial_number) != 0)
    throw std::invalid_argument("second row number '" +
                                std::to_string(initial_number) +
                                "' has been found\n");

  rows.insert({initial_number, row_number});
  rows_to_print.push_back(initial_number);

  string cell = "";
  for (auto it = line.begin() + first_comma + 1; it != line.end(); ++it) {
    if (*it != ',') {
      cell += *it;
    } else if (!cell.empty()) {
      data.back().push_back(Cell<Data_type>{std::move(cell), 0, 0, 0});
      cell.clear();
    }
  }
  if (!cell.empty())
    data.back().push_back(Cell<Data_type>{std::move(cell), 0, 0, 0});

}

template <class Data_type>
CSV_Table<Data_type>::CSV_Table(string filename, char delim) {
  std::ifstream file(filename);
  if (!file.is_open())
    throw std::invalid_argument("file has not been found\n");

  if (file.peek() != ',')
    throw std::invalid_argument("comma was expected as first character");

  string line = "", cell = "";
  getline(file, line);
  columns_to_print = line;
  read_columns_names(line);

  size_t row_number = 0;
  while (getline(file, line)) {
    data.push_back({});
    read_line(line, row_number++);
  }

  if (rows.size() != data.size())
    throw std::invalid_argument("row adress has been missed\n");

  for (const auto& [init_number, actual_number] : rows)
    if (data.at(actual_number).size() != columns.size())
      throw std::invalid_argument("line " + std::to_string(init_number) +
                                  ": missing value has been found\n");
}

template <class Data_type>
pair<size_t, size_t> CSV_Table<Data_type>::get_cell_address(const string& raw_address) {
  size_t first_digit = raw_address.find_first_of("0123456789");
  if (first_digit == 0 || first_digit == raw_address.npos)
    throw std::invalid_argument("bad cell address: " + raw_address);

  string column = raw_address.substr(0, first_digit);
  int64_t row = stoll(raw_address.substr(first_digit));
  return {rows.at(row), columns.at(column)};
}

template <class Data_type>
Data_type apply_operation(Data_type first, char operation, Data_type second) {
  if (operation == '/' && second == 0)
    throw std::runtime_error("zero division");
  if (operation == '+')
    return first + second;
  if (operation == '-')
    return first - second;
  if (operation == '*')
    return first * second;
  return first / second;
}

template <class Cell_type>
template <class Data_type>
Data_type CSV_Table<Cell_type>::Cell<Data_type>::evaluate_operand(
  const string& raw_string,
  CSV_Table& table,
  size_t operation_position
  ) {
  if (raw_string.empty())
    throw std::runtime_error("empty string as operation parameter\n");
  Data_type result;
  if (isdigit(raw_string.at(0))) {
    if (std::is_integral_v<Data_type>)
      result = std::stoll(raw_string);
    else
      result = std::stold(raw_string);
    if (std::is_integral_v<Data_type> && std::to_string(result) != raw_string)
      throw std::invalid_argument("invalid cell value: " + raw_string);
  } else {
    pair<size_t, size_t> address =
      table.get_cell_address(raw_string);
    result = evaluate(table, address);
  }
  return result;
}

template <class Cell_type>
template <class Data_type>
Data_type CSV_Table<Cell_type>::Cell<Data_type>::evaluate(
  CSV_Table& table,
  pair<size_t, size_t> address
  ) {
  Cell& cell = table.data.at(address.first).at(address.second);
  if (cell.visited == true && cell.is_processed == false)
    throw std::logic_error("cyclic reference has been found\n");

  if (cell.is_processed)
    return cell.processed;

  cell.visited = true;
  string& raw_string = cell.raw;
  Data_type result;

  try {
    if (raw_string.at(0) == '=') {
      size_t operation_position = raw_string.find_first_of("/*-+");
      if (operation_position == raw_string.npos)
        throw std::invalid_argument("operation has not been found");

      char operation = raw_string[operation_position];
        Data_type left = evaluate_operand(
          raw_string.substr(1, operation_position - 1),
          table,
          operation_position);
        Data_type right = evaluate_operand(
          raw_string.substr(operation_position + 1),
          table,
          operation_position);

        result = apply_operation<Data_type>(
          left,
          operation,
          right);
    } else {
      if (std::is_integral_v<Data_type>)
        result = std::stoll(raw_string);
      else
        result = std::stold(raw_string);
      if (std::is_integral_v<Data_type> && std::to_string(result) != raw_string)
        throw std::invalid_argument("cell is consist of invalid characters\n");
    }
  } catch (std::exception& e) {
    throw std::invalid_argument("adress: " +
                                std::to_string(address.first) + ", " +
                                std::to_string(address.second) + ": " +
                                e.what());
  }

  cell.processed = result;
  cell.is_processed = true;

  return result;
}

template <class Data_type>
void CSV_Table<Data_type>::evaluate() {
  for (size_t i = 0; i < data.size(); ++i)
    for (size_t j = 0; j < data[i].size(); ++j)
      if (!data[i][j].visited)
        data[i][j].evaluate(*this, {i, j});
}

template <class Data_type>
void CSV_Table<Data_type>::print(ostream& out) {
  out << columns_to_print << '\n';
  for (size_t i = 0; i < data.size(); ++i) {
    out << rows_to_print[i];
    for (size_t j = 0; j < data[i].size(); ++j)
      if (!std::is_integral_v<Data_type>)
        out << std::fixed << std::setprecision(3) << ',' << data[i][j].processed;
      else
        out << ',' << data[i][j].processed;
    out << '\n';
  }
}
