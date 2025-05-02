#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;
/*You need to complete the mentioned TODOs in order to satisfy the rubric
criteria "The student will be able to extract and display basic data about the
system."

You need to properly format the uptime. Refer to the comments mentioned in
format. cpp for formatting the uptime.*/

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  // get the current list of processes IDs
  vector<int> pids = LinuxParser::Pids();
  std::vector<Process> updated_processes = {};
  // Create list of updated processes for this cycle
  for (int pid : pids) {
    Process newProcess(pid);
    newProcess.UpdateCpuUtilization();
    updated_processes.push_back(newProcess);
  }
  // Sort the process
  std::sort(updated_processes.begin(), updated_processes.end());
  processes_ = updated_processes;
  return processes_;
}

// TODO: Return the system's kernel identifier (string)
std::string System::Kernel() { return kernel_; }

// TODO: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// TODO: Return the operating system name
std::string System::OperatingSystem() { return operSys_; }

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// TODO: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// TODO: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }

// Constructor to intialize static attributes of system
System::System() {
  Processor processor;
  cpu_ = processor;
  operSys_ = LinuxParser::OperatingSystem();
  kernel_ = LinuxParser::Kernel();
};
