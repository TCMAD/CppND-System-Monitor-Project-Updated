#include <dirent.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <algorithm>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

/*
*   Calcule la memoire utiliser
*/
float MemoryUtilization() {
  string info, value;
  string line;
  float MemUsed;
  vector<string> values;
  std::ifstream memfile("/proc/meminfo");
  if (memfile.is_open()) {
    for (std::string line; std::getline(memfile, line);) {
      std::istringstream lineInfo(line);
      lineInfo >> info >> value;
      values.push_back(value);
    }
    float MemTotal,Memfree;
    MemTotal = std::stof(values[0]); // stof => Convert into float
    Memfree = std::stof(values[1]);
    MemUsed = (MemTotal - Memfree);
    std::cout << MemUsed/MemTotal<< std::endl;
  }
  return MemUsed;
}

long UpTime(){
  /**
   * @return temps de fonctionnement du système (y compris en suspend) contenue dans le fichier /proc/uptime
   */ 
  string line, uptime;
  std::ifstream file("/proc/uptime"); // lis le fichier
  if(file.is_open()) {
    std::getline(file,line); // Obtient la ligne du fichier ouvert
    std::istringstream stream(line); // Diffuse le contenue de la ligne dans la variable stream
    stream >> uptime; // Prend la première valeur de la ligne
    std::cout << uptime << std::endl; 
  }
  return std::stol(uptime); // stol => Convert into long
}

int TotalProcesses() {
  string line,info,value;
  int TP;
  std::ifstream file("/proc/stat"); // lis le fichier
  if(file.is_open()) {
    for(line; std::getline(file,line);){ // Permet de lire chaque ligne du fichier
      std::istringstream stream(line); // Diffuse
      stream >> info >> value; 
      if(info=="processes"){
        std::cout << value << std::endl;
        TP = std::stoi(value); // stoi => Convert into int
      }
    }
  }
  return TP;
}

int RunningProcesses(){
  string line,info,value;
  int RP;
  std::ifstream file("/proc/stat"); // lis le fichier
  if(file.is_open()) {
    for(line; std::getline(file,line);){ // Permet de lire chaque ligne du fichier
      std::istringstream stream(line); // Diffuse
      stream >> info >> value; 
      if(info=="procs_running"){
        std::cout << value << std::endl;
        RP = std::stoi(value); // stoi => Convert into int
      }
    }
  }
  return RP;
}

vector<string> CpuUtilization() {
  /** @src https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux */
  string info, user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  string line;
  std::ifstream file("/proc/stat");
  if(file.is_open()){
    std::getline(file,line);
    std::istringstream stream(line);
    stream >> info >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
  }
  std::cout << line << std::endl;
  std::cout << info << ' ' << user << ' '  <<  nice << ' ' <<  system << ' ' << idle << ' ' << iowait << ' ' <<  irq << ' ' << softirq << ' ' << std::endl;
  return {user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice}; 
  }

string Uid(int pid) {
  string line, info, value;
  string Uid;
  std::ifstream file("/proc/"+ to_string(pid) +"/status");
  if (file.is_open()) {
    for (line; std::getline(file, line);) {
      std::istringstream stream(line);  // Diffuse
      stream >> info >> value; 
      if (info == "Uid:") {
        //std::cout << line << std::endl;
        Uid = value;
      }
    }
  }
  return Uid;
}

string User(int pid) {
  string line, info, key, x;
  string user;
  std::ifstream file("/etc/passwd");

  if (file.is_open()) {
    while (std::getline(file, line)) {
      // Remplace : par espace vide
      std::replace(line.begin(), line.end(), ':', ' ');
      // Stream
      std::istringstream stream(line);
      stream >> info >> x >> key;
      if (key == Uid(pid)) {
        std::cout << line << std::endl;
        user = info;
        std::cout << user << std::endl;
      }
    }
  }
  return user;
}

string Ram(int pid) { 
  string line, info, value, unit;
  string Ram;
  std::ifstream file("/proc/"+ to_string(pid) +"/status");
  
  if(file.is_open()){
    for(line; std::getline(file, line);){
      std::istringstream stream(line);
      stream >> info >> value >> unit;
      if (info == "VmSize:"){
        Ram = value + ' ' + unit;
        std::cout << Ram << std::endl;
      }
    }
  }
  return Ram;
}

string Command(int pid) { 
  string cmdline;
  std::ifstream file("/proc/" + to_string(pid) + "/cmdline");

  if(file.is_open()){
    std::getline(file,cmdline);
  }
  std::cout << cmdline << std::endl;
  return cmdline;
}

float ProcessCpuUtilization(int pid) { 
    long uptime = UpTime();
    vector<string> data;
    string line,value;
    long utime, stime, cutime, cstime, starttime;
    int Hertz = sysconf(_SC_CLK_TCK);
    // Obtenir les infos du fichier /proc/[PID]/stat
    std::ifstream file("/proc/" + to_string(pid) + "/stat");
    if(file.is_open()){
        std::getline(file,line);
        std::istringstream stream(line);
        //std::cout << line << std::endl; 
        while (stream >> value){
          data.push_back(value);
        }
        //std::cout << data[13] << std::endl;
        utime = std::stol(data[13]);
        stime = std::stol(data[14]);
        cutime = std::stol(data[15]);
        starttime = std::stol(data[21]);
        std::cout << utime << " " << stime << std::endl;
        
    }
    return 0; 
}

long LinuxParserUpTime(int pid) {
  string line, value;
  long clockTicks; // starttime
  int Hertz = sysconf(_SC_CLK_TCK);
  // Obtenir les infos du fichier /proc/[PID]/stat
  std::ifstream file("/proc/" + to_string(pid) +
                     "/stat");
  if (file.is_open()) {
    std::getline(file, line);
    std::istringstream stream(line);
    for (int i = 0; i < 22; i++) {
      stream >> value;
    }
    clockTicks = std::stol(value);
    std::cout << clockTicks << std::endl;
  }
  return clockTicks/sysconf(_SC_CLK_TCK);
}
int main() { 
    MemoryUtilization();
  }