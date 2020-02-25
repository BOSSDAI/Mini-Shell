#ifndef _COMMOND_H_
#define _COMMOND_H_
#include <unistd.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>
#define COMMAND_PATH "/bin/"

extern char ** environ;

class Command {
 private:
  std::string command_line;
  std::vector<std::string> arguments_vector;  // in stack
  char ** arguments_array;                    // in heap
 public:
  // test Makefile
  void get_command(void) const { std::cout << command_line << std::endl; }
  // constructor an rule of three, implemented in basic.cpp
  Command();
  Command(const Command & rhs);
  Command & operator=(const Command & rhs);
  ~Command();

  // operations, implemented in execute.cpp
  int print_prompt(void) const;
  int read_command(void);
  int parse_command(void);
  int execute_command(void);

  // about alias, implemented in alias.cpp
  int set_alias(void);
  int decode_alias(void);
  int export_variable(void);
};

#endif
