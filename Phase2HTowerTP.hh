#ifndef Phase2HTowerTP_hh
#define Phase2HTowerTP_hh

/*

  ECAL Barrel HTower Trigger Primitives
     ET, eta, phi, quality (time)

*/

#include <stdint.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <math.h>

using namespace std;

const uint16_t nHTowersInHalfEta = 17;
const uint16_t nHTowersInEta = nHTowersInHalfEta * 2;
const uint16_t nHTowersInPhi = 72;
const uint16_t nHTowersInHalfPhi = nHTowersInPhi / 2;

const double hTowerSize = 2 * M_PI / nHTowersInPhi;
const double maxAbsBarrelHCALEta = hTowerSize * nHTowersInHalfEta;

class Phase2HTowerTP {

public:

  Phase2HTowerTP(uint16_t eta, uint16_t phi, uint16_t et, uint16_t q)
    : cEta(eta), cPhi(phi), cET(et), quality(q) {
    if(cEta > nHTowersInEta) cEta = 0xFFFF;
    if(cPhi > nHTowersInPhi) cPhi = 0xFFFF;
    if(cET > 0x3FF) et = 0x3FF;
  }

  friend ostream& operator<<(ostream& os, const Phase2HTowerTP& c);

private:

  Phase2HTowerTP();

  //Phase2HTowerTP(const Phase2HTowerTP&);
  //const Phase2HTowerTP& operator=(const Phase2HTowerTP&);

  uint16_t cEta;        // 6 bits (Barrel: 34 HTowers |eta|<maxAbsBarrelCaloEta)
  uint16_t cPhi;        // 7 bits (Barrel: 72 HTowers in phi)
  uint16_t cET;         // 10 bits (Barrel: Integerized to linear scale based on LSB choice)
  uint16_t quality;    // Timing information (Barrel: Currently unused)

};

#endif
