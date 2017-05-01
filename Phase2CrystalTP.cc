#include "Phase2CrystalTP.hh"

ostream& operator<<(ostream& os, const Phase2CrystalTP& c) {
  if(c.cET > 0)
    os << hex 
       << showbase
       << internal
       << setfill('0')
       << setw(6)
       << c.cEta << " "
       << c.cPhi << " "
       << c.cET << endl
       << setfill(' ')
       << dec;
  return os;
}
