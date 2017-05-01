#ifndef Phase2CrystalTP_hh
#define Phase2CrystalTP_hh

/*

  ECAL Barrel Crystal Trigger Primitives
     ET, eta, phi, quality (time)

*/

#include <stdint.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <math.h>

using namespace std;

const uint16_t nCrystalsInEta = 17 * 5 * 2;
const uint16_t nCrystalsInPhi = 360;
const uint16_t nCrystalsInHalfEta = nCrystalsInEta / 2;
const uint16_t nCrystalsInHalfPhi = nCrystalsInPhi / 2;

const double crystalSize = 2 * M_PI / nCrystalsInPhi;
const double maxAbsBarrelECALEta = crystalSize * nCrystalsInHalfEta;

class Phase2CrystalTP {

public:

  Phase2CrystalTP(uint16_t eta, uint16_t phi, uint16_t et, uint16_t q)
    : cEta(eta), cPhi(phi), cET(et), quality(q) {
    if(cEta > nCrystalsInEta) cEta = 0xFFFF;
    if(cPhi > nCrystalsInPhi) cPhi = 0xFFFF;
    if(cET > 0x3FF) et = 0x3FF;
  }

  friend ostream& operator<<(ostream& os, const Phase2CrystalTP& c);

private:

  Phase2CrystalTP();

  //Phase2CrystalTP(const Phase2CrystalTP&);
  //const Phase2CrystalTP& operator=(const Phase2CrystalTP&);

  uint16_t cEta;        // 8 bits (Barrel: 170 crystals |eta|<maxAbsBarrelCaloEta)
  uint16_t cPhi;        // 9 bits (Barrel: 360 crystals in phi)
  uint16_t cET;         // 16 bits (Barrel: Integerized to linear scale based on LSB choice)
  uint16_t quality;    // Timing information (Barrel: Currently unused)

};

#endif
