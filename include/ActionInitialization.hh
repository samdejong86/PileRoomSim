//
// $Id: ActionInitialization.hh 76340 2013-11-08 15:21:09Z gcosmo $
//
/// \file ActionInitialization.hh
/// \brief Definition of the ActionInitialization class

#ifndef ActionInitialization_h
#define ActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

class G4VSteppingVerbose;
class DetectorConstruction;

/// Action initialization class.
///

class ActionInitialization : public G4VUserActionInitialization
{
  public:
    ActionInitialization(DetectorConstruction*);
    
    virtual ~ActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;
    
    virtual G4VSteppingVerbose* InitializeSteppingVerbose() const;
    
  private:
    DetectorConstruction* fDetConstruction;    

};

#endif

    
