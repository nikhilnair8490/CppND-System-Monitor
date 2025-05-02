#include <iostream>
#include <string>

#include "linux_parser.h"
#include "ncurses_display.h"
#include "system.h"

int main() {
  System system;
  // std::vector<std::string> out = LinuxParser::CpuUtilization();
  // std::cout << LinuxParser::UpTime(1) << std::endl;
  NCursesDisplay::Display(system);
}