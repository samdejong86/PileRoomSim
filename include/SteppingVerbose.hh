//
/// \file radioactivedecay/rdecay01/include/SteppingVerbose.hh
/// \brief Definition of the SteppingVerbose class
//
//
// $Id: SteppingVerbose.hh 70529 2013-05-31 16:52:42Z gcosmo $
// 

#ifndef SteppingVerbose_h
#define SteppingVerbose_h 1

#include "G4SteppingVerbose.hh"


class SteppingVerbose : public G4SteppingVerbose {

public:   

  SteppingVerbose();
 ~SteppingVerbose();
 
  virtual void TrackingStarted();
  virtual void StepInfo();
};


#endif
