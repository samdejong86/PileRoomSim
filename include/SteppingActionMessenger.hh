
#ifndef SteppingActionMessenger_h
#define SteppingActionMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class G4UIdirectory;
class G4UIcmdWithADouble;
class SteppingAction;
class SteppingActionMessenger: public G4UImessenger
{
public:
  
  SteppingActionMessenger(SteppingAction* );
  ~SteppingActionMessenger();
    
  virtual void SetNewValue(G4UIcommand*, G4String);
    
private:
  
  SteppingAction* fSteppingAction;
    
  G4UIdirectory*         fActDir;    
  G4UIcmdWithADouble*    fFACTORCmd;
};


#endif

