#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

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
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  
  string line;
  string key, value, kB;  
  float TotalMemory, FreeMemory;
  
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
   
  if (stream.is_open()) {
    while (std::getline(stream, line)){
      	std::replace(line.begin(), line.end(), ':', ' ');
  		std::istringstream linestream(line);
    	while (linestream >> key >> value >> kB) {
        	if (key == "TotalMemory") { TotalMemory = std::stof(value);}
            if (key == "FreeMemory") {  FreeMemory = std::stof(value);}
        	}
    	}
  }
  return (TotalMemory - FreeMemory)/TotalMemory; 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  string line;
  string uptime;
  
  if (stream.is_open()) {
    std::getline(stream, line);
   	std::istringstream linestream(line);
    linestream >> uptime;
  }
  return std::stol(uptime);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  
  std::ifstream stream(kProcDirectory + kStatFilename);
  string line;
  string key;
  int s_idl;
  int s_activ;
  float s_util;
  vector<string> jiffies{};
  vector<int> elems;
  
  
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key;
    while (linestream >> key) {
      elems.emplace_back(std::stoi(key));
      }
    s_idl = elems[3] + elems[4];
    s_activ = elems[0] + elems[1] + elems[2] + elems[4] + elems[6] + elems[7];
    s_util = s_activ/(float)(s_idl + s_activ);
    jiffies.emplace_back(std::to_string(s_util));
  }
  return jiffies;
}

float LinuxParser::CpuUtilization(int pid) {
  string line;
  string column;
  vector<string> columns;
 
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  float s_util{0.0};
  
  if(stream.is_open()) {
    getline(stream, line);
    std::istringstream linestream(line);
    while(linestream.good()) {
      getline(linestream, column, ' ');
      columns.push_back(column);
    }
    //totalTime = utime + stime
    // with child processes totalTime += cutime + cstime
    int TotalProcTCK = stoi(columns[13]) + stoi(columns[14]) + stoi(columns[15]) + stoi(columns[16]);
    float TotalProcTime = TotalProcTCK / (float)sysconf(_SC_CLK_TCK);
    long TotalSecs = UpTime() - UpTime(pid);
    
    if(TotalSecs != 0)
      s_util = TotalProcTime/(float)TotalSecs;
  }
  return s_util;
}
// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {

  string line;
  string key;
  string value;  
  std::ifstream stream(kProcDirectory + kStatFilename);
  int processes;
  
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "processes") {
          linestream >> processes;
          break;
        }
      }
    }
  }
  return processes;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  
  string line;
  string key;
  std::ifstream stream(kProcDirectory + kStatFilename);
  int R_processes = 0;  
  
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "procs_running") {
          linestream >> R_processes;
          break;
        }
      }
    }
  }
  return R_processes;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  
  string command;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
 
  if (stream.is_open()) {
    std::getline(stream, command);
  }
  return command;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  
  string line;
  string key;
  long ram;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "RAMSize:") {
        linestream >> ram;
        break;
      }
    }
  }
  return std::to_string(ram/ 1024);
}
  

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  
  string uid;
  string line;
  string key;
  
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  
  if(stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "Uid:") {
          linestream >> uid;
          break;
        }
      }
    }
  }
  return uid;  
}
  

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  
  string user;
  string passwd;
  string uid;
  string line;
  std::ifstream stream(kPasswordPath);
  
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> passwd >> uid) {
        if (uid == LinuxParser::Uid(pid)){
          break;;
        }
      }
    }
  }
  return user; 
}
// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  
  string value;
  string line;
  vector<string> states;
  
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      states.emplace_back(value);
    }
  }
  return LinuxParser::UpTime() - std::stol(states[21])/sysconf(_SC_CLK_TCK);
}