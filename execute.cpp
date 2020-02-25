/*
 * This file include some core methods and funtion of the whole project
 */

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include <cstring>
#include <iostream>
#include <sstream>

#include "command.h"

extern std::map<std::string, std::string> alias_map;

/*
 * print_prompt method will print a string before this program read command from cin
 * The string contains "ffosh", current working directory, and a "$" sign.
 * For example, if the program is working under
 *     ~/commandShell
 * this method should print
 * ffosh:~/commandShell $ 
 */
int Command::print_prompt(void) const {
  size_t sz = 1024;
  char current_path[1024];
  getcwd(current_path, sz);
  std::cout << "ffosh:" << current_path << " $";
  return 0;
}

/*
 * read_command method will read oneline command either typed by user or passed by redirection
 * the method will then pass this line to member variable command_line
 */
int Command::read_command(void) {
  if (std::cin.eof()) {
    std::cin.clear();
    exit(0);
  }
  std::getline(std::cin, command_line);
  return 0;
};

/*
 * parse_command method will use the command_line that we get by read_command method and finally 
 * create an array of arguments which can be the input of execute function in the next method
 * The method take following steps to do this:
 *     1. sperator arguments by whitespace or quotation mark and put the arguments in the argument_vector
 *     2. change all variables to its corresponding string
 *     3. use the vector we have to make the arguments_array 
 */
int Command::parse_command(void) {
  // exit if user type "exit"
  if (command_line == "exit") {
    exit(EXIT_SUCCESS);
  }

  // create arguments vector

  // separate arguments by white space, including the operation deal with quotation mark and "\"
  std::string::iterator it = command_line.begin();
  while (it < command_line.end()) {
    // Jump over whitespaces
    while (it < command_line.end() && *it == ' ') {
      it++;
    }
    if (it == command_line.end()) {
      break;
    }
    /* a whitespace doesn't followed by a quotation mark, 
       which means the next arguments should be separeted by two whitespaces    
     */
    if (*it != '"') {
      std::string argument;
      std::string::iterator argument_start = it;
      std::string::iterator argument_end = argument_start;
      while (argument_end < command_line.end() && *argument_end != ' ' &&
             *argument_end != '\0') {
        argument_end++;
      }
      argument = command_line.substr(argument_start - command_line.begin(),
                                     argument_end - argument_start);
      arguments_vector.push_back(argument);
      it = argument_end + 1;
    }
    /*
      a whitespace followed by a quotation mark, 
      which means that the next argument should be separeted by a pair of quotation marks
     */
    else {
      // finding the next quotatin mark
      std::string argument;
      std::string::iterator argument_start = it;
      std::string::iterator argument_end = argument_start + 1;
      while (argument_end <= command_line.end()) {
        if (argument_end == command_line.end()) {
          std::cout << "Matching failed" << std::endl;
          command_line.clear();
          return 1;
        }
        if (*argument_end != '"') {
          argument_end++;
        }
        // now *argument_end maybe the quotation mark we want to find
        // we still need to do some checking
        /* 1. If there is a space or the end of line after the quotation mark? 
	   If not, this quotation mark is not the quotation mark we want to find
	   If is:
	       Have this quotation been escaped?
	       If it have been escaped, this quatation is still not the quotation mark we want to find
	 */
        else if (
            *(argument_end + 1) != ' ' &&
            *(argument_end + 1) !=
                '\0') {  // If there is a space or the end of line after the quatation mark
          argument_end++;
        }
        else {  // 后面有空格或者结束的引号
          int escape_count = 0;
          std::string::iterator escape_flag = argument_end - 1;
          while (escape_flag > argument_start) {
            if (*escape_flag != '\\') {
              break;
            }
            else {
              escape_flag--;
              escape_count++;
            }
          }
          if ((escape_count % 2) == 0) {
            argument = command_line.substr(argument_start - command_line.begin() + 1,
                                           argument_end - argument_start - 1);
            arguments_vector.push_back(argument);
            it = argument_end + 1;
            break;
          }
          else {
            argument_end++;
          }
        }
      }
    }
  }

  // change all escaped characters to their original

  for (size_t i = 0; i < arguments_vector.size(); i++) {
    std::string::iterator j = arguments_vector[i].begin();
    while (j < arguments_vector[i].end()) {
      if (*j == '\"') {
        std::cout << "Wrong format" << std::endl;
        arguments_vector.clear();
      }

      if (*j == '\\') {
        if (*(j + 1) == '\\' || *(j + 1) == '\"') {
          arguments_vector[i].erase(j);
        }
      }

      j++;
    }
  }

  // use arguments vector to create arguments array
  size_t count = arguments_vector.size();
  arguments_array = new char *[count + 1];
  for (size_t i = 0; i < count; i++) {
    arguments_array[i] = strdup((char *)arguments_vector[i].c_str());
  }
  arguments_array[count] = NULL;
  return 0;
}

/*
 * execute_command function will execute the line as a shell do
 */
int Command::execute_command(void) {
  if (command_line.empty() || arguments_vector.empty()) {
    return 0;
  }
  /* cd, set and export can be regarded as build-in functions
   * they don't need a child process
  */
  if (arguments_vector[0] == "cd") {
    if (chdir(arguments_array[1]) != 0) {
      std::cerr << "cannot change directory: " << std::strerror(errno) << std::endl;
    }
    return 0;
  }
  // set_alias and export_variable are implemented in alias.cpp
  if (arguments_vector[0] == "set") {
    set_alias();
    return 0;
  }
  if (arguments_vector[0] == "export") {
    export_variable();
    return 0;
  }
  pid_t pid = fork();
  int status;
  if (pid == -1) {
    std::cerr << "Failed to fork" << std::endl;
  }
  if (pid == 0) {
    // execute the command in child process
    execvp(arguments_array[0], arguments_array);
    int error_number = errno;
    if (error_number == 2) {
      std::cout << "Command " << arguments_vector[0] << " not found" << std::endl;
    }
    else {
      std::cerr << "Execve command failed: " << std::strerror(error_number) << std::endl;
    }
    _exit(EXIT_FAILURE);  // exit child process
  }

  waitpid(pid, &status, 0);  // parent process wait child process

  /*
   * print information about the execution of the command 
   */
  if (WEXITSTATUS(status) == 0) {
    std::cout << "Program was successful" << std::endl;
  }
  else if (WEXITSTATUS(status) != 0) {
    std::cout << "Program failed with code " << WEXITSTATUS(status) << std::endl;
  }

  else if (WIFSIGNALED(status)) {
    std::cout << "Terminated by signal " << WTERMSIG(status) << std::endl;
  }

  return 0;
}
