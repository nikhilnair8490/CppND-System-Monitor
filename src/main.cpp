#include <iostream>
#include <string>

#include "linux_parser.h"
#include "ncurses_display.h"
#include "system.h"

int main() {
  System system;
  // std::vector<std::string> out = LinuxParser::CpuUtilization();
  // std::cout << Format::ElapsedTime(36501) << std::endl;
  NCursesDisplay::Display(system);
}