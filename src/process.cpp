#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"


using std::string;
using std::to_string;
using std::vector;

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 
    long uptime = LinuxParser::UpTime();
    vector<string> data;
    string line, value;
    long utime, stime, cutime, cstime, starttime;
    int Hertz = sysconf(_SC_CLK_TCK);
    // Obtenir les infos du fichier /proc/[PID]/stat
    std::ifstream file(LinuxParser::kProcDirectory + to_string(pid_) + LinuxParser::kStatFilename);
    if(file.is_open()){
        std::getline(file,line);
        std::istringstream stream(line);
        while (stream >> value){
          data.push_back(value);
        }
        utime = std::stol(data[13]);
        stime = std::stol(data[14]);
        cutime = std::stol(data[15]);
        cstime = std::stol(data[16]);
        starttime = std::stol(data[21]);
    }
    // Calcul : https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
    long total_time = utime + stime;
    total_time = total_time + cutime + cstime;
    float seconds = uptime - (starttime / Hertz);
    float cpu_usage = 100 * ((total_time / Hertz)/ seconds);
    cpu_ = cpu_usage;
    return cpu_usage; 
}

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
    return cpu_ < a.cpu_; 
}