#ifndef Phase2TrackTP_hh
#define Phase2TrackTP_hh

#include <stdint.h>

#include <iostream>
#include <iomanip>
using namespace std;

#include "Pythia8/Pythia.h"
using namespace Pythia8; 

class Phase2TrackTP {
public:

  Phase2TrackTP(Particle &particle, Rndm *r);
  friend ostream& operator<<(ostream& os, const Phase2TrackTP& t);

private:

  Phase2TrackTP();

  //Phase2TrackTP(const Phase2TrackTP&);
  //const Phase2TrackTP& operator=(const Phase2TrackTP&);

  Rndm *rndmPtr;

  uint32_t rInv;       // 15 bits
  uint32_t phi;        // 17 bits
  uint16_t d0;         // 10 bits
  uint16_t t;          // 12 bits
  uint16_t z0;         // 12 bits
  uint16_t chisq;      // 10 bits
  uint16_t consistency;// 5 bits
  uint16_t hitMask;    // 11 bits
  bool charge;

};

#endif
