#include "Phase2TrackTP.hh"

const double rInvScale = 0.01;

Phase2TrackTP::Phase2TrackTP(Particle &particle, Rndm *rndmPtrIn) :
  rndmPtr(rndmPtrIn),
  rInv(0),
  phi(0),
  d0(0),
  t(0),
  z0(0),
  chisq(0),
  consistency(0),
  hitMask(0),
  charge(false)
{
  double sigma = pow((0.15 * particle.pT() / 1000) * (0.15 * particle.pT() / 1000) + (0.005 * 0.005), 0.5);
  double smear = (1. + rndmPtr->gauss() * sigma);
  double pt = particle.pT() * smear;
  rInv = ((1 / pt) / rInvScale);
  if(rInv > 0x7FFF) rInv = 0x7FFF;
}  

ostream& operator<<(ostream& os, const Phase2TrackTP& t) {
  os << hex 
     << showbase
     << internal
     << setfill('0')
     << setw(6)
     << t.rInv << " " 
     << setw(7)
     << t.phi << " "
     << setw(6)
     << t.d0 << " "
     << t.t << " "
     << t.z0 << " "
     << t.chisq << " "
     << t.consistency << " "
     << t.hitMask << " "
     << setw(3)
     << t.charge << endl
     << setfill(' ')
     << dec;
  return os;
}
