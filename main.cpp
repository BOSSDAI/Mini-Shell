#include "main.h"

// use a global variable alias_map to store the maps of variables set by user.
std::map<std::string, std::string> alias_map;
int main(void) {
  rep_loop();
  return EXIT_SUCCESS;
}
