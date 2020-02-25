/*
 * This file implement some basic methods of Command class, include:
 *     constructor
 *     copy constructor
 *     destructor
 *     assignment operator
*/

#include <string.h>

#include "command.h"

// Two constructors
Command::Command() : command_line(""), arguments_vector(), arguments_array(NULL){};
Command::Command(const Command & rhs) :
    command_line(rhs.command_line),
    arguments_vector(rhs.arguments_vector) {
  size_t count = arguments_vector.size();
  char ** temp = new char *[count];
  for (size_t i = 0; i < count; i++) {
    temp[i] = strdup(rhs.arguments_array[i]);
  }
  arguments_array = temp;
  temp = NULL;
};

// assignment operator
Command & Command::operator=(const Command & rhs) {
  if (this != &rhs) {
    command_line = rhs.command_line;
    arguments_vector = rhs.arguments_vector;
    size_t count = arguments_vector.size();
    char ** temp = new char *[count];
    for (size_t i = 0; i < count; i++) {
      temp[i] = strdup(rhs.arguments_array[i]);
    }
    delete[] arguments_array;
    arguments_array = temp;
    temp = NULL;
  }
  return *this;
};

// destructor
Command::~Command() {
  for (size_t i = 0; i < arguments_vector.size() + 1; i++) {
    free(arguments_array[i]);
  }
  delete[] arguments_array;
}
