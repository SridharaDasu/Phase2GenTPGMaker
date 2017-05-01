#ifndef Phase2HCALTPMaker_hh
#define Phase2HCALTPMaker_hh

/*

  Makes HCAL Trigger Primitives for various particles using Pythia::Particle and simple smearing
  Appends HCAL Tower TPs to the input

*/

#include <stdint.h>
#include <vector>

using namespace std;

#include "Pythia8/Pythia.h"

using namespace Pythia8; 

#include "Phase2HTowerTP.hh"

class Phase2HCALTPMaker {

public:

  Phase2HCALTPMaker(Rndm *r) {rndmPtr = r;}

  double process(Particle &particle, vector<Phase2HTowerTP> &hTowers, double ecalDeposit);

private:

  Phase2HCALTPMaker();
  Phase2HCALTPMaker(const Phase2HCALTPMaker&);
  const Phase2HCALTPMaker& operator=(const Phase2HCALTPMaker&);

  double getEnergyDeposit(Particle &particle, double ecalDeposit);

  Rndm *rndmPtr;

};

#endif
