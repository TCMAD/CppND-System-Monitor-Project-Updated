#include <dirent.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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
    MemUsed = MemTotal - Memfree;
    std::cout << MemUsed << std::endl;
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
    std::cout << uptime; 
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
    stream >> info >> user >> nice >> system >> idle >> irq >> softirq >> steal >> guest >> guest_nice;
  }
  std::cout << info << ' ' << user << ' '  <<  nice << ' ' <<  system << ' ' << idle << ' ' <<  irq << ' ' << std::endl;
  return {user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice}; 
  }


int main() { CpuUtilization(); }