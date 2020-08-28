#include "processor.h"

#include <string>
#include <vector>

#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  /** @src
   * https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
   */

  // Mise à jour des valeurs précédentes
  prevuser = user;
  prevnice = nice;
  prevsystem = system;
  previdle = idle;
  previowait = iowait;
  previrq = irq;
  prevsoftirq = softirq;
  prevsteal = steal;

  // Mise à jour des valeurs actuelles
  std::vector<std::string> CpuInfo = LinuxParser::CpuUtilization();

  user = std::stoi(CpuInfo[0]);  // stoi => Convert string into int
  nice = std::stoi(CpuInfo[1]);
  system = std::stoi(CpuInfo[2]);
  idle = std::stoi(CpuInfo[3]);
  iowait = std::stoi(CpuInfo[4]);
  irq = std::stoi(CpuInfo[5]);
  softirq = std::stoi(CpuInfo[6]);
  steal = std::stoi(CpuInfo[7]);
  guest = std::stoi(CpuInfo[8]);
  guestnice = std::stoi(CpuInfo[9]);

  // Incertain du calcul
  PrevIdle = previdle + previowait;
  Idle = idle + iowait;

  PrevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal;
  NonIdle = user + nice + system + irq + softirq + steal;

  PrevTotal = PrevIdle + PrevNonIdle;
  Total = Idle + NonIdle;

  // Différence entre valeur totale actuelle - valeur totale précédente
  totald = Total - PrevTotal;
  idled = Idle - PrevIdle;

  // Pourcentage CPU
  return float((totald - idled)/totald);
}