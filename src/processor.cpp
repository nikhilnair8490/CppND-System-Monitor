#include "processor.h"

#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  std::vector<std::string> cpuUtlization_ss = LinuxParser::CpuUtilization();
  // the first element is the total cpu utlization
  return std::stof(cpuUtlization_ss[0]);
}