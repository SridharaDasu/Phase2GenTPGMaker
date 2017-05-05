/*

  This simple program generates various signal processes for various
  physics processes.  It includes the mixing of required level of
  pileup.  It then takes pythia output, smears stable particles and
  writes them out in Phase2 TPG format in a flat files.

*/

#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <math.h>

using namespace std;

#include "Pythia8/Pythia.h"

using namespace Pythia8; 

namespace Pythia8 {
  class Rndm;
  class Event;
  class Particle;
}

Rndm *rndmPtr;

#include "Phase2TrackTP.hh"

#include "Phase2ECALTPMaker.hh"
#include "Phase2HCALTPMaker.hh"
#include "Phase2CrystalTP.hh"
#include "Phase2HTowerTP.hh"

const double maxAbsTrackerEta = maxAbsBarrelECALEta;

int poisson(double mean, Rndm *rndmPtr) {
  static double oldMean = -1;
  static double g;
  if(mean != oldMean) {
    oldMean = mean;
    if(mean == 0) {
      g = 0;
    }
    else {
      g = exp(-mean);
    }
  }    
  double em = -1;
  double t = 1;
  do {
    em++;
    t *= rndmPtr->flat();
  } while(t > g);
  return em;
}

int main(int argc, char **argv) {

  // Generator. Process selection. LHC initialization. Histogram.

  Pythia pythia;

  if(argc >= 2) pythia.readFile(argv[1]);
  else {
    cerr << "Command syntax: " 
	 << argv[0] 
	 << " <command file name> [<numberOfEvents>] [<runNumber>==<randomNumberSeed>] [<meanPileUpLevel>]" 
	 << endl;
    exit(1);
  }

  // Get default parameters from the pythia cards file

  int   nEvents    = pythia.mode("Main:numberOfEvents");
  int     nList    = 1;//pythia.mode("Main:numberToList");
  int     nShow    = 1;//pythia.mode("Main:timesToShow");
  int    nAbort    = pythia.mode("Main:timesAllowErrors");
  bool   showCS    = false;//pythia.flag("Main:showChangedSettings");
  bool   showAS    = true;//pythia.flag("Main:showAllSettings");
  bool   showCPD   = false;//pythia.flag("Main:showChangedParticleData");
  bool   showAPD   = true;//pythia.flag("Main:showAllParticleData");

  // Overwrite with command line options, when appropriate

  if(argc >= 3) nEvents = atoi(argv[2]);
  int runNumber = 0;
  if(argc >= 4) runNumber = atoi(argv[3]);
  float meanPileupEventCount = 0;
  if(argc >= 5) meanPileupEventCount = atof(argv[4]);

  // Initialize pythia

  pythia.init();

  const int beamA = pythia.info.idA();
  const int beamB = pythia.info.idB();
  const double cmEnergy = pythia.info.eCM();

  // List settings.
  if (showCS) pythia.settings.listChanged();
  if (showAS) pythia.settings.listAll();

  // List particle data.  
  if (showCPD) pythia.particleData.listChanged();
  if (showAPD) pythia.particleData.listAll();

  rndmPtr = &pythia.rndm;
  rndmPtr->init(runNumber);

  // Pileup pythia for proton beams

  Pythia *pileupPythia;
  if(beamA == 2212 && beamB == 2212) {
    pileupPythia = new Pythia();
    // Use beamA = beamB = 2212 and cmEnergy = Hard Interaction cmEnergy
    pileupPythia->readString("SoftQCD:all = on");
    cout << "beam A, B, s" << beamA << ", " << beamB << ", " << cmEnergy << endl;
    pileupPythia->readString("Beams:idA = 2212");
    pileupPythia->readString("Beams:idB = 2212");
    std::stringstream ss;
    ss << "Beams:eCM = " << cmEnergy;
    pileupPythia->readString(ss.str());
    pileupPythia->init();
    pileupPythia->rndm.init(runNumber);
  }

  // Begin event loop
  for (int iEvent = 0, iAbort = 0; iEvent < nEvents; ) {

    // Generate event. Skip if error. Unless, errors are too many.
    if (!pythia.next()) {
      iAbort++;
      if(iAbort > nAbort) {
	cerr << "Too many aborted events = " << nAbort << endl;
	exit(1);
      }
      continue;
    }

    // List first few events, both hard process and complete events.
    if (iEvent == 0) { 
      pythia.info.list();
      pythia.process.list();
      pythia.event.list();
      cout << '\a'; // Put a bell before our output starts!
    }

    // Add pileup for proton beams

    if(beamA == 2212) {
      int pileupEventCount = 0;
      if(meanPileupEventCount > 0) pileupEventCount = poisson(meanPileupEventCount, rndmPtr);
      for (int puEvent = 0; puEvent < pileupEventCount; ) {
	if(!pileupPythia->next()) continue;
	double vx = rndmPtr->gauss()*2.; // Mean beam spread is 2 mm in x-y and 75 mm in z 
	double vy = rndmPtr->gauss()*2.;
	double vz = rndmPtr->gauss()*75.;
	for(int i = 0; i < pileupPythia->event.size(); i++) {
	  Particle& particle = pileupPythia->event[i];
	  particle.xProd(vx+particle.xProd());
	  particle.yProd(vy+particle.yProd());
	  particle.zProd(vz+particle.zProd());
	  if(particle.status() > 0) {
	    if(particle.isVisible()) {
	      if(particle.pT() > 0.5) {
		pythia.event.append(particle);
	      }
	    }
	  }
	}
	puEvent++;
      }
    }

    // Loop over all particles in the event
    // Save TPG information in suitable format
    // https://twiki.cern.ch/twiki/bin/view/CMS/L1TriggerPhase2InterfaceSpecifications

    vector<Phase2TrackTP> tracks;
    vector<Phase2TrackTP> muonTracks;
    
    Phase2ECALTPMaker ecalTPMaker(rndmPtr);
    Phase2HCALTPMaker hcalTPMaker(rndmPtr);

    vector<Phase2CrystalTP> crystals;
    vector<Phase2HTowerTP> hTowers;

    for (int i = 0; i < pythia.event.size(); ++i) {

      Particle& particle = pythia.event[i];
      
      // Consider only particles with good status
      if(particle.status() > 0) {

	// Consider only visible particles
	if(particle.isVisible()) {

	  // Ignore soft particles and those outside the detector
	  if(particle.pT() < 1.0 && abs(particle.eta()) > maxAbsBarrelECALEta) {
	  }

	  else {

	    if(abs(particle.charge()) != 0) {
	      // Tracker coverage is larger than ecal barrel so this doesn't matter
	      if(abs(particle.eta()) < maxAbsTrackerEta) {
		tracks.push_back(Phase2TrackTP(particle, rndmPtr));
	      }
	      // Muons greater than 3 GeV make muon tracks
	      if(abs(particle.id()) == 13 && particle.pT() > 3.) {
		muonTracks.push_back(Phase2TrackTP(particle, rndmPtr));
	      }
	    }
	  
	    // All particles deposit energy in the calorimeters
	    // Multiple crystals and hcal towers may light up
	    // Energy remaining after ECAL is deposited in the HCAL
	    double etRemaining = ecalTPMaker.process(particle, crystals);
	    if(etRemaining > 0) hcalTPMaker.process(particle, hTowers, etRemaining);
	    
	  }
	}

      }

    }

    // End of event loop. Statistics. Done.
    
    cout << "Event = " << iEvent 
	 << "; nCrystals = " << crystals.size() 
	 << "; nHTowers = " << hTowers.size() 
	 << "; nTracks = " << tracks.size() 
	 << "; nMuonTracks = " << muonTracks.size()
	 << endl;
    cout << "Non-zero Crystal TPs: " << endl;
    for(int i = 0; i < crystals.size(); i++) {
      cout << crystals[i];
    }
    cout << "Non-zero hTower TPs: " << endl;
    for(int i = 0; i < hTowers.size(); i++) {
      cout << hTowers[i];
    }
    cout << "Non-zero Track TPs: " << endl;
    for(int i = 0; i < tracks.size(); i++) {
      cout << tracks[i];
    }
    cout << "Non-zero Muon TPs: " << endl;
    for(int i = 0; i < muonTracks.size(); i++) {
      cout << muonTracks[i];
    }
    if(muonTracks.size() > 0) cout << endl;
    
    iEvent++;
    
  }
  
  // Save output

  pythia.stat();
  if(beamA == 2212) {
    pileupPythia->stat();
  }

  return 0;

}
