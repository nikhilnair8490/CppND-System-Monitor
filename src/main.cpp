#include <iostream>

#include "linux_parser.h"
#include "ncurses_display.h"
#include "system.h"

int main() {
  System system;
  // std::cout << Format::ElapsedTime(36501) << std::endl;
  NCursesDisplay::Display(system);
}