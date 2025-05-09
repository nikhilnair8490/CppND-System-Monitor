#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { return cpuUtil_; }

void Process::UpdateCpuUtilization() {
  cpuUtil_ = LinuxParser::CpuUtilization(pid_);
}

// TODO: Return the command that generated this process
string Process::Command() { return cmd_; }

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// TODO: Return the user (name) that generated this process
string Process::User() { return user_; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
  return (cpuUtil_ > a.cpuUtil_);
}

// Constructor to initialize static attributes of a process
Process::Process(int pid) {
  pid_ = pid;
  cmd_ = LinuxParser::Command(pid);
  user_ = LinuxParser::User(pid);
  UpdateCpuUtilization();
}