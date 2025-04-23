#include <iostream>
#include "ncurses_display.h"
#include "system.h"
#include "linux_parser.h"

int main() {
  System system;
  //std::cout << LinuxParser::UpTime() << std::endl;
  NCursesDisplay::Display(system);
}