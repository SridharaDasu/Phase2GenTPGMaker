#ifndef Phase2ECALTPMaker_hh
#define Phase2ECALTPMaker_hh

/*

  Makes ECAL Trigger Primitives for various particles using Pythia::Particle and simple smearing
  Appends crystal TPs to the input

*/

#include <stdint.h>
#include <vector>

using namespace std;

#include "Pythia8/Pythia.h"

using namespace Pythia8; 

#include "Phase2CrystalTP.hh"

class Phase2ECALTPMaker {

public:

  Phase2ECALTPMaker(Rndm *r) {rndmPtr = r;}

  double process(Particle &particle, vector<Phase2CrystalTP> &crystals);

private:

  Phase2ECALTPMaker();
  Phase2ECALTPMaker(const Phase2ECALTPMaker&);
  const Phase2ECALTPMaker& operator=(const Phase2ECALTPMaker&);

  double getEnergyDeposit(Particle &particle);

  Rndm *rndmPtr;

};

#endif
