#include <iostream>
#include "ncurses_display.h"
#include "system.h"
#include "linux_parser.h"

int main() {
  System system;
  std::cout << LinuxParser::User(1508) << std::endl;
  //NCursesDisplay::Display(system);
}