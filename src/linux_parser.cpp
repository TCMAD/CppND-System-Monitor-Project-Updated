#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

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
        int pid = std::stoi(filename);
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
   * @see Calcul mémoire
   * https://stackoverflow.com/questions/41224738/how-to-calculate-system-memory-usage-from-proc-meminfo-like-htop/41251290#41251290
   */
  string info, value;
  string line;
  float MemUsed;
  vector<string> values;
  std::ifstream memfile(kProcDirectory +
                        kMeminfoFilename);  // Obtenir le fichier meminfo
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
    float MemTotal, Memfree;
    MemTotal = std::stof(values[0]);
    Memfree = std::stof(values[1]);
    MemUsed = (MemTotal - Memfree)/MemTotal;

  }
  return MemUsed;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  /**
   * @return temps de fonctionnement du système (y compris en suspend) contenue
   * dans le fichier /proc/uptime
   */
  string line, uptime;
  std::ifstream file(kProcDirectory + kUptimeFilename);  // lis le fichier
  if (file.is_open()) {
    std::getline(file, line);  // Obtient la ligne du fichier ouvert
    std::istringstream stream(
        line);  // Diffuse le contenue de la ligne dans la variable stream
    stream >> uptime;  // Prend la première valeur de la ligne
  }
  return std::stol(uptime);  // stol => Convert into long
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return ActiveJiffies() + IdleJiffies(); }

// TODO: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  /* CPU time spent in user mode*/
  long utime;
  /* CPU time spent in kernel mode*/
  long stime;
  /* Waited for Children CPU time spent in user mode*/
  long cutime;
  /* Waited for Children CPU time spent in kernel mode*/
  long cstime;
  std::string line{};
  std::vector<std::string> Jiffies{};

  /*Get the file from the path */
  std::ifstream file_name(kProcDirectory + std::to_string(pid) + kStatFilename);

  if (file_name.is_open()) {
    /* get the line from the file */
    std::getline(file_name, line);
    /*Stream the line into a buffer iss*/
    std::istringstream iss(line);
    /* Reads successive elements from buffer iss */
    std::istream_iterator<string> iter(iss);
    /* end of biffer */
    std::istream_iterator<string> eos;
    /* Copy the buffer elements iss to vector jiffies using iterator */
    std::copy(iter, eos, std::back_inserter(Jiffies));
    /* Now sort the string vector */
    std::sort(Jiffies.begin(), Jiffies.end());

    /* Get the active jiffies from the vector string for the process */
    utime = std::stol(Jiffies[13]);
    stime = std::stol(Jiffies[14]);
    cutime = std::stol(Jiffies[15]);
    cstime = std::stol(Jiffies[16]);
  }
  return (utime + stime + cutime + cstime);
}


// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
    /* Get the total time spent by the CPU */
    vector<string> cpu_time = CpuUtilization();
    long active_jiffies{};

    /* Get the active time spent by the cpu */
    active_jiffies = (std::stol(cpu_time[CPUStates::kUser_]) +
                    std::stol(cpu_time[CPUStates::kNice_]) +
                    std::stol(cpu_time[CPUStates::kSystem_]) +
                    std::stol(cpu_time[CPUStates::kIRQ_]) +
                    std::stol(cpu_time[CPUStates::kSoftIRQ_]) +
                    std::stol(cpu_time[CPUStates::kSteal_]) +
                    std::stol(cpu_time[CPUStates::kGuest_]) +
                    std::stol(cpu_time[CPUStates::kGuestNice_]));

    return active_jiffies;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
    /*Get the total time spent by the CPU */
    vector<string> cpu_time = CpuUtilization();

    /* Get the time spent idle by the cpu */
    long idle_jiffies = (std::stol(cpu_time[CPUStates::kIdle_]) +
                        std::stol(cpu_time[CPUStates::kIOwait_]));
    return idle_jiffies;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  /** @src
   * https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
   */
  string info, user, nice, system, idle, iowait, irq, softirq, steal, guest,
      guest_nice;
  string line;
  std::ifstream file(kProcDirectory + kStatFilename);
  if (file.is_open()) {
    std::getline(file, line);
    std::istringstream stream(line);
    stream >> info >> user >> nice >> system >> idle >> iowait >> irq >> softirq >>
        steal >> guest >> guest_nice;
  }
  return {user, nice,    system, idle,  iowait,
          irq,  softirq, steal,  guest, guest_nice};
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line, info, value;
  int TP;
  std::ifstream file(kProcDirectory + kStatFilename);  // lis le fichier
  if (file.is_open()) {
    for (line;
         std::getline(file, line);) {  // Permet de lire chaque ligne du fichier
      std::istringstream stream(line);  // Diffuse
      stream >> info >> value;
      if (info == "processes") {
        TP = std::stoi(value);  // stoi => Convert into int
      }
    }
  }
  return TP;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line, info, value;
  int RP;
  std::ifstream file(kProcDirectory + kStatFilename);  // lis le fichier
  if (file.is_open()) {
    for (line;
         std::getline(file, line);) {  // Permet de lire chaque ligne du fichier
      std::istringstream stream(line);  // Diffuse
      stream >> info >> value;
      if (info == "procs_running") {
        RP = std::stoi(value);  // stoi => Convert into int
      }
    }
  }
  return RP;
}

// TODO: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string cmdline;
  std::ifstream file(kProcDirectory + to_string(pid) + kCmdlineFilename);

  if (file.is_open()) {
    std::getline(file, cmdline);
  }
  return cmdline;
}

// TODO: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line, info, value, unit;
  string Ram;
  std::ifstream file(kProcDirectory + to_string(pid) + kStatFilename);

  if (file.is_open()) {
    for (line; std::getline(file, line);) {
      std::istringstream stream(line);
      stream >> info >> value >> unit;
      if (info == "VmSize:") {
        Ram = value + ' ' + unit;
        break;
      }
    }
  }
  return Ram;
}

// TODO: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line, info, value;
  string Uid;
  std::ifstream file(kProcDirectory + to_string(pid) + kStatusFilename);
  if (file.is_open()) {
    for (line; std::getline(file, line);) {
      std::istringstream stream(line);  // Diffuse
      stream >> info >> value;
      if (info == "Uid:") {
        Uid = value;
        break;
      }
    }
  }
  return Uid;
}

// TODO: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line, info, key, x;
  string user;
  std::ifstream file(kPasswordPath);

  if (file.is_open()) {
    while (std::getline(file, line)) {
      // Remplace : par espace vide
      std::replace(line.begin(), line.end(), ':', ' ');
      // Stream
      std::istringstream stream(line);
      stream >> info >> x >> key;
      if (key == Uid(pid)) {
        user = info;
        break;
      }
    }
  }
  return user;
}

// TODO: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line, value;
  long clockTicks; // starttime
  int Hertz = sysconf(_SC_CLK_TCK);
  // Obtenir les infos du fichier /proc/[PID]/stat
  std::ifstream file(LinuxParser::kProcDirectory + to_string(pid) +
                     LinuxParser::kStatFilename);
  if (file.is_open()) {
    std::getline(file, line);
    std::istringstream stream(line);
    for (int i = 0; i < 22; i++) {
      stream >> value;
    }
    clockTicks = std::stol(value);
  }
  return clockTicks/Hertz;
}