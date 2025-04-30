#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;
namespace fs = std::filesystem;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
// vector<int> LinuxParser::Pids() {
//   vector<int> pids;
//   DIR* directory = opendir(kProcDirectory.c_str());
//   struct dirent* file;
//   while ((file = readdir(directory)) != nullptr) {
//     // Is this a directory?
//     if (file->d_type == DT_DIR) {
//       // Is every character of the name a digit?
//       string filename(file->d_name);
//       if (std::all_of(filename.begin(), filename.end(), isdigit)) {
//         int pid = stoi(filename);
//         pids.push_back(pid);
//       }
//     }
//   }
//   closedir(directory);
//   return pids;
// }

// Linux parser using filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  fs::path directory(kProcDirectory);
  // Iterate through the directory
  if (fs::exists(directory)) {
    for (const auto& file : fs::directory_iterator(directory)) {
      if (fs::is_directory(file)) {
        string dirName = file.path().filename().string();
        if (std::all_of(dirName.begin(), dirName.end(), isdigit)) {
          int pid = stoi(dirName);
          pids.push_back(pid);
        }
      }
    }
  }
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  // used = total - free - (Buffers + Cached + SReclaimable - Shmem)
  float memUsed = 0.0f;
  float memTotal = 0.0f;
  float memFree = 0.0f;
  float memBuff = 0.0f;
  float memCache = 0.0f;
  float memSreclaim = 0.0f;
  float memShmem = 0.0f;
  float memUsed_pct = 0.0f;
  string line;
  string key;
  string value;
  string kB;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value >> kB) {
        if (key == "MemTotal") {
          memTotal = stof(value);
        } else if (key == "MemFree") {
          memFree = stof(value);
        } else if (key == "Buffers") {
          memBuff = stof(value);
        } else if (key == "Cached") {
          memCache = stof(value);
        } else if (key == "SReclaimable") {
          memSreclaim = stof(value);
        } else if (key == "Shmem") {
          memShmem = stof(value);
        }
      }
    }
  }
  memUsed = memTotal - memFree - (memBuff + memCache + memSreclaim - memShmem);
  if (memTotal > 0.0f) {
    memUsed_pct = memUsed / memTotal;
  }
  return memUsed_pct;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string upTime_s;
  string idleTimeCores;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> upTime_s >> idleTimeCores;
  }
  return stol(upTime_s);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  // Total number of jiffies is sum of all 10 columns in proc/stat first line
  string label;
  string line;
  long value = 0, total_jiffies = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);

    // Ignore the 'cpu' label
    linestream >> label;

    while (linestream >> value) {
      total_jiffies += value;
    }
  }

  return total_jiffies;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  // Active jiffies of a process is utime + stime
  string line;
  long value = 0, utime = 0, stime = 0;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    // Find where the process name ends
    size_t pos = line.find(')');
    if (pos != string::npos) {
      // Shift two pos ahead
      std::istringstream linestream(line.substr(pos + 2));
      std::string dummy;
      linestream >> dummy;  // Read and discard the "S" (state)
      int i = 1;
      // user time utime is in 11th pos and system time stime is in 12th.
      while (linestream >> value) {
        if (i == 11) {
          utime = value;
        }
        if (i == 12) {
          stime = value;
          break;
        }
        ++i;
      }
    }
  }

  return (utime + stime);
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  // Active jiffies of system is user + nice + system + irq + softirq + steal
  string token;
  string line;
  long value = 0, activeJiffies = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    // Ignore the 'cpu' label
    linestream >> token;
    int i = 1;
    // Include: fields 1, 2, 3, 6, 7, 8 after cpu label
    while (linestream >> value) {
      switch (i) {
        case 1:
        case 2:
        case 3:
        case 6:
        case 7:
        case 8:
          activeJiffies += value;
          break;
        default:
          break;
      }
      i++;
      if (i >= 9) {
        break;
      }
    }
  }

  return activeJiffies;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  // Idle jiffies of system is idle + iowait
  string token;
  string line;
  long value = 0, idleJiffies = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    // Ignore the 'cpu' label
    linestream >> token;
    int i = 1;
    // Include: fields 4,5 after cpu label
    while (linestream >> value) {
      switch (i) {
        case 4:
        case 5:
          idleJiffies += value;
          break;
        default:
          break;
      }
      i++;
      if (i >= 6) {
        break;
      }
    }
  }

  return idleJiffies;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  // CPU utilization is ratio of Active Jiffies Diff / Total Jiffies Diff
  // Previous jiffie values
  static long prevActvJiffie = 0;
  static long prevTotalJiffie = 0;
  float cpuUtilization = 0.0f;

  // Current cpu jiffies value
  long currActvJiffie = LinuxParser::ActiveJiffies();
  long currTotalJiffie = LinuxParser::Jiffies();

  long deltaTotal = (currTotalJiffie - prevTotalJiffie);
  // Change in jiffies
  if (deltaTotal > 0) {
    cpuUtilization = float(currActvJiffie - prevActvJiffie) / deltaTotal;
  }

  // Update prev jiffie
  prevActvJiffie = currActvJiffie;
  prevTotalJiffie = currTotalJiffie;

  vector<string> output;
  output.push_back(std::to_string(cpuUtilization));

  return output;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  // Total processes is in proc/stat file under the processes key
  string key;
  string line;
  int totalProcesses = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      // Get the key/label
      linestream >> key;
      if (key == "processes") {
        // Get the value at that key
        linestream >> totalProcesses;
        break;
      }
    }
  }
  return totalProcesses;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  // Active/running processes is in proc/stat file under the procs_running key
  string key;
  string line;
  int runProcesses = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      // Get the key/label
      linestream >> key;
      if (key == "procs_running") {
        // Get the value at that key
        linestream >> runProcesses;
        break;
      }
    }
  }
  return runProcesses;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  // Command associated by a process is in processes cmdline file
  string cmdLine = "";
  string line;
  std::ifstream fileStream(kProcDirectory + std::to_string(pid) +
                           kCmdlineFilename);
  if (fileStream.is_open()) {
    std::getline(fileStream, line);
    std::istringstream linestream(line);
    linestream >> cmdLine;
  }
  return cmdLine;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  // Memory (RAM) by a process is in processes stat file @ VmRSS
  string memUsed = "";
  string line;
  string key, value, kB;
  std::ifstream fileStream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (fileStream.is_open()) {
    while (getline(fileStream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value >> kB;
      if (key == "VmRSS:") {
        memUsed = value;
        break;
      }
    }
  }
  return memUsed;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  // Uid for a process is in processes stat file @ UiD: 1st value is real user
  // id
  string Uid = "";
  string line;
  string key, value;
  std::ifstream fileStream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (fileStream.is_open()) {
    while (getline(fileStream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:") {
        Uid = value;
        break;
      }
    }
  }
  return Uid;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  // Map Uid in /etc/passwd file to get the username of the person
  string line;
  string userName, x, Uid_;
  string Uid = LinuxParser::Uid(pid);
  std::ifstream fileStream(kPasswordPath);
  if (fileStream.is_open()) {
    while (getline(fileStream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> userName >> x >> Uid_;
      if (Uid_ == Uid) {
        // Got username for the given PID
        return userName;
      }
    }
  }

  return "";
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  // Uptime of a process is in process stat file
  string line;
  long value = 0, upTime = 0, upTime_sec = 0;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);

    // Find where the process name ends
    size_t pos = line.find(')');
    if (pos != string::npos) {
      std::istringstream linestream(line.substr(pos + 2));
      std::string dummy;
      linestream >> dummy;  // Read and discard the "S" (state)
      int i = 1;
      // uptime in jiffies is at 19th position ater process name
      while (linestream >> value) {
        if (i == 19) {
          upTime = value;
          break;
        }
        ++i;
      }
    }
  }

  // Current system up time
  long systemUpTime = LinuxParser::UpTime();
  // Proces up time in secs
  upTime_sec = systemUpTime - (upTime / sysconf(_SC_CLK_TCK));

  return upTime_sec;
}
