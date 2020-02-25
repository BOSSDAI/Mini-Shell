#include "command.h"

extern std::map<std::string, std::string> alias_map;

/*
 * check_variable_name will check if the variable name is valid (return true) or
 * not (return false) the varible name should only contains letters numbers or
 * "_"
 */
bool check_variable_name(std::string &name) {
  bool condition;
  for (std::string::iterator it = name.begin(); it < name.end(); it++) {
    condition = (isalpha(*it) || isalnum(*it) || *it == '_');
    if (!condition) {
      std::cout << "Invalid variable name" << std::endl;
      return false;
    }
  }
  return true;
}

/*
 * set_alias will put the variable name-value pair in to the alias_map
 */
int Command::set_alias(void) {
  if (check_variable_name(arguments_vector[1])) {
    alias_map[arguments_vector[1]] = arguments_vector[2];
  }
  return 0;
}

/*
 * decode_alias will get the variable name after "$", find the value and change
 * it or print a massage to tell user that the variable are not found
 */

int Command ::decode_alias(void) {
  const std::string user_input = command_line;
  std::string actual_command;
  size_t i = 0;
  while (i < user_input.length()) {
    if (user_input[i] != '$') {
      actual_command.push_back(user_input[i]);
      i++;
    } else {
      // get varible name(s)
      std::size_t start = i + 1;
      std::size_t out_len = 0;

      while (start + out_len < user_input.length() &&
             user_input[start + out_len] != ' ' &&
             user_input[start + out_len] != '$') {
        out_len++;
      }

      size_t in_len = out_len;
      while (in_len >= 0) {
        if (in_len == 0) {
          std::cout << "No variable found" << std::endl;
          return 1;
        }
        std::string var_name = user_input.substr(start, in_len);
        std::string var_value = "";
        // find value in the alias_map
        std::map<std::string, std::string>::iterator it =
            alias_map.find(var_name);
        if (it == alias_map.end()) {
          in_len--;
        } else {
          var_value = it->second;
          actual_command.append(var_value);
          break;
        }
      }
      i = start + in_len;
    }
  }
  command_line = actual_command;
  return 0;
}

/*
 * export_variable will sent the particular variable to environment
 */
int Command ::export_variable(void) {
  std::map<std::string, std::string>::iterator it =
      alias_map.find(arguments_vector[1]);
  if (it == alias_map.end()) {
    std::cout << "No variable named" << arguments_vector[1] << std::endl;
    return 1;
  }
  setenv(arguments_array[1], it->second.c_str(), 1);
  return 0;
}
