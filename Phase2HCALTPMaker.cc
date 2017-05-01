#include "Phase2HCALTPMaker.hh"
#include "Phase2HTowerTP.hh"

static double etScale = 0.25;

const double minIAverage = 1.0;

double Phase2HCALTPMaker::process(Particle &particle, vector<Phase2HTowerTP> &hTowers, double ecalDeposit) {

  double hcalDeposit = 0;
  if(abs(particle.eta()) < maxAbsBarrelHCALEta) {

    // HTowerTP position
    int eta = (particle.eta() - (hTowerSize / 2.)) / hTowerSize;
    if(eta < -nHTowersInHalfEta || eta > nHTowersInHalfEta) {
      cerr << "H: eta = " << eta << "; particle.eta() = " << particle.eta() << endl;
    }
    else eta = eta + nHTowersInHalfEta;
    int phi = (particle.phi() - (hTowerSize / 2.)) / hTowerSize;
    if(phi < -nHTowersInHalfPhi || phi > nHTowersInHalfPhi) {
      cerr << "H: phi = " << phi << "; particle.phi() = " << particle.phi() << endl;
    }
    else phi = phi + nHTowersInHalfPhi;

    hcalDeposit = getEnergyDeposit(particle, ecalDeposit);

    // Core hTower takes 50% of the energy deposit
    double etCore = hcalDeposit * 0.5 * (1 + 0.1 * rndmPtr->gauss());
    // Peripheral hTowers take the remaining, but distributed in 3(eta) x 3(phi)
    double etPeriphery = (hcalDeposit - etCore) / (3*3);

    uint16_t et = (etCore / etScale);
    hTowers.push_back(Phase2HTowerTP(eta, phi, et, 0));

    for(int dPhi = -1; dPhi < 2; dPhi++) {
      for(int dEta = -1; dEta < 2; dEta++) {
	if(dPhi !=0 && dEta != 0) {
	  uint16_t pPhi = (phi + dPhi);
	  uint16_t pEta = (eta + dEta);
	  if(pPhi < nHTowersInPhi && pEta < nHTowersInEta) {
	    et = (etPeriphery * (1 + 0.1 * rndmPtr->gauss())) / etScale;
	    hTowers.push_back(Phase2HTowerTP(pEta, pPhi, et, 0));
	  }
	}
      }
    }

  }

  return (particle.pT() - hcalDeposit);

}

double Phase2HCALTPMaker::getEnergyDeposit(Particle &particle, double ecalDeposit) {

  double hcalDeposit = 0;
  double etRemaining = particle.pT() - ecalDeposit;

  // Minimimum ionization for muons
  if(abs(particle.id()) == 13) {
    hcalDeposit = (1. + rndmPtr->gauss()) * minIAverage;
    if(hcalDeposit > etRemaining) hcalDeposit = etRemaining;
  }
  else { // All other particles lose their remaining energy in the HCAL
    double sqrte = pow(particle.e(), 0.5);
    double rsqr = ((1.15 / sqrte) * (1.15 / sqrte)) + 0.055 * 0.055;
    double sigma = pow((particle.e()*rsqr), 0.5);
    double smear = (1. + rndmPtr->gauss() * sigma);
    hcalDeposit = etRemaining * smear;
  }

  return hcalDeposit;

}
