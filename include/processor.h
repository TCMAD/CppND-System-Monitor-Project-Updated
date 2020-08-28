#ifndef PROCESSORH
#define PROCESSORH

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
 // Valeurs actuelles
 int user, nice, system, idle{0}, iowait, irq, softirq, steal, guest, guestnice, nonIdle{0};
 // Valeurs précédentes
 int prevuser, prevnice, prevsystem, previdle, previowait, previrq, prevsoftirq, prevsteal;
 // Valeurs calculées
 int PrevTotal, PrevNonIdle, Idle, NonIdle, PrevIdle, Total;
 // Valeurs retournées
 int totald, idled, total, nonidle;
};

#endif