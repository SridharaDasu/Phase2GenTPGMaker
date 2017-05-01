#include "Phase2HTowerTP.hh"

ostream& operator<<(ostream& os, const Phase2HTowerTP& c) {
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
