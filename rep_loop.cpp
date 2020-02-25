#include "rep_loop.h"
/*
 * The workflow of the bash can be regarded as an infinite loop of 
 *     Read command inputed by user
 *     execute the command
 *     print the result
*/
void rep_loop(void) {
  for (;;) {
    Command command;
    command.print_prompt();   // print a prompt before read command from user
    command.read_command();   // read command from cin
    command.decode_alias();   // change the command to its regular way
    command.parse_command();  // set up the vector and the array of arguments correctly
    if (command.execute_command() != 0) {
      break;
    }
  }
}
