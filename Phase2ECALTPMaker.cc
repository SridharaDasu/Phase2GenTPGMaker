#include "Phase2ECALTPMaker.hh"
#include "Phase2CrystalTP.hh"

static double etScale = 0.25;

// 2 (MeV/gm-cm^2) * 25 X0 * 7.389 (gm-cm^2) / 1000 (GeV / MeV)
const double minIAverage = 0.369;
// Hadronic interaction probability in ECAL
const double hadrIntProb = 0.6;
// Average fraction of energy deposited in ECAL for interacting hadrons for low ET < 3 GeV
const double em0Fraction = 0.95;
// Average fraction of energy deposited in ECAL for interacting hadrons for 3 < ET > 5 GeV
const double em3Fraction = 0.50;
// Average fraction of energy deposited in ECAL for interacting hadrons for high ET > 5 GeV
const double em5Fraction = 0.25;

double Phase2ECALTPMaker::process(Particle &particle, vector<Phase2CrystalTP> &crystals) {

  double etDeposit = 0;

  if(abs(particle.eta()) < maxAbsBarrelECALEta) {
    // CrystalTP position
    int eta = (particle.eta() - (crystalSize / 2.)) / crystalSize;
    if(eta < -nCrystalsInHalfEta || eta > nCrystalsInHalfEta) {
      cerr << "E: eta = " << eta << "; particle.eta() = " << particle.eta() << endl;
    }
    else eta = eta + nCrystalsInHalfEta;
    int phi = (particle.phi() - (crystalSize / 2.)) / crystalSize;
    if(phi < -nCrystalsInHalfPhi || phi > nCrystalsInHalfPhi) {
      cerr << "E: phi = " << phi << "; particle.phi() = " << particle.phi() << endl;
    }
    else phi = phi + nCrystalsInHalfPhi;

    etDeposit = getEnergyDeposit(particle);
    // Core crystal takes 90% of the energy deposit
    double etCore = etDeposit * 0.9 * (1 + 0.1 * rndmPtr->gauss());
    // Peripheral crystals take 10% of energy, but distributed in 3(eta) x 5(phi)
    double etPeriphery = (etDeposit - etCore) / (3*5);

    uint16_t et = (etCore / etScale);
    crystals.push_back(Phase2CrystalTP(eta, phi, et, 0));

    for(int dPhi = -2; dPhi < 3; dPhi++) {
      for(int dEta = -1; dEta < 2; dEta++) {
	if(dPhi !=0 && dEta != 0) {
	  uint16_t pPhi = (phi + dPhi);
	  uint16_t pEta = (eta + dEta);
	  if(pPhi < nCrystalsInPhi && pEta < nCrystalsInEta) {
	    et = (etPeriphery * (1 + 0.1 * rndmPtr->gauss())) / etScale;
	    crystals.push_back(Phase2CrystalTP(pEta, pPhi, et, 0));
	  }
	}
      }
    }

  }

  return etDeposit;

}

double Phase2ECALTPMaker::getEnergyDeposit(Particle &particle) {
  double etDeposit = 0;
  if((abs(particle.id()) == 11 || particle.id() == 22)) { // EM CrystalTP
      double sqrte = pow(particle.e(), 0.5);
      double rsqr = ((0.027 / sqrte) * (0.027 / sqrte)) + 0.005 * 0.005 + (0.150 / particle.e()) * (0.150 / particle.e());
      double sigma = pow((particle.e()*rsqr), 0.5);
      double smear = (1. + rndmPtr->gauss() * sigma);
      etDeposit = particle.pT()*smear;
    }
    // Minimimum ionization for muons and 30% of charged pions, kaons and protons
    else if(abs(particle.id()) == 13 || ((rndmPtr->flat() < 0.4) && (
	    abs(particle.id()) == 211 ||
	    abs(particle.id()) == 321 ||
	    abs(particle.id()) == 2212))) {
      etDeposit = (1. + rndmPtr->gauss()) * minIAverage;
      if(etDeposit > particle.pT()) etDeposit = particle.pT();
    }
    else { // Interacting hadrons lose a fraction of their energy
      if(particle.pT() < 3) etDeposit = em0Fraction * particle.pT();
      else if(particle.pT() < 5) etDeposit = em3Fraction * particle.pT();
      else etDeposit = em5Fraction * particle.pT();
      double sqrte = pow(particle.e(), 0.5);
      double rsqr = ((0.027 / sqrte) * (0.027 / sqrte)) + 0.005 * 0.005 + (0.150 / particle.e()) * (0.150 / particle.e());
      double sigma = pow((particle.e()*rsqr), 0.5);
      double smear = (1. + rndmPtr->gauss() * sigma);
      etDeposit = etDeposit * smear;
    }
  return etDeposit;
}
