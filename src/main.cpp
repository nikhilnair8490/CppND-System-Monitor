#include <iostream>

#include "linux_parser.h"
#include "ncurses_display.h"
#include "system.h"

int main() {
  System system;
  std::cout << LinuxParser::UpTime(434) << std::endl;
  // NCursesDisplay::Display(system);
}