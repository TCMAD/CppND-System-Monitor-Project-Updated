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
  /**
   * @return Memoire utilisé
   * @see Calcul mémoire https://stackoverflow.com/questions/41224738/how-to-calculate-system-memory-usage-from-proc-meminfo-like-htop/41251290#41251290
   */
  string info, value;
  string line;
  float MemUsed;
  vector<string> values;
  std::ifstream memfile(kProcDirectory + kMeminfoFilename); // Obternir le fichier meminfo
  if (memfile.is_open()) {
    /** 
    * Obtenir les informations lignes par lignes
    *  Les valeurs sont enregistrer sur le vecteur "Values"
    */
    for (std::string line; std::getline(memfile, line);) {
      std::istringstream lineInfo(line);
      lineInfo >> info >> value;
      values.push_back(value);
    }
    float MemTotal,Memfree;
    MemTotal = std::stof(values[0]);
    Memfree = std::stof(values[1]);
    MemUsed = MemTotal - Memfree;
  }
  return MemUsed;
 }

// TODO: Read and return the system uptime
long LinuxParser::UpTime(){
  /**
   * @return temps de fonctionnement du système (y compris en suspend) contenue dans le fichier /proc/uptime
   */ 
  string line, uptime;
  std::ifstream file(kProcDirectory + kUptimeFilename); // lis le fichier
  if(file.is_open()) {
    std::getline(file,line); // Obtient la ligne du fichier ouvert
    std::istringstream stream(line); // Diffuse le contenue de la ligne dans la variable stream
    stream >> uptime; // Prend la première valeur de la ligne 
  }
  return std::stol(uptime); // stol => Convert into long
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
  /** @src https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux */
  string info, user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  string line;
  std::ifstream file(kProcDirectory + kStatFilename);
  if(file.is_open()){
    std::getline(file,line);
    std::istringstream stream(line);
    stream >> info >> user >> nice >> system >> idle >> irq >> softirq >> steal >> guest >> guest_nice;
  }
  return {user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice}; 
  }

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line,info,value;
  int TP;
  std::ifstream file(kProcDirectory + kStatFilename); // lis le fichier
  if(file.is_open()) {
    for(line; std::getline(file,line);){ // Permet de lire chaque ligne du fichier
      std::istringstream stream(line); // Diffuse
      stream >> info >> value; 
      if(info=="processes"){
        TP = std::stoi(value); // stoi => Convert into int
      }
    }
  }
  return TP;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line,info,value;
  int RP;
  std::ifstream file(kProcDirectory + kStatFilename); // lis le fichier
  if(file.is_open()) {
    for(line; std::getline(file,line);){ // Permet de lire chaque ligne du fichier
      std::istringstream stream(line); // Diffuse
      stream >> info >> value; 
      if(info=="procs_running"){
        RP = std::stoi(value); // stoi => Convert into int
      }
    }
  }
  return RP;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }