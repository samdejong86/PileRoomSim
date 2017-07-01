
#include "SteppingActionMessenger.hh"

#include "SteppingAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAnInteger.hh"

SteppingActionMessenger::SteppingActionMessenger(SteppingAction * ra)
 : G4UImessenger(),
   fSteppingAction(ra),
   fActDir(0),    
   fFACTORCmd(0)
   {   
//   fActDir = new G4UIdirectory("/testem4/run/");
//   fActDir->SetGuidance("run commands");
      
  
//   fFACTORCmd = new G4UIcmdWithADouble("/testem4/steppingaction/factor", this);
//   fFACTORCmd->SetGuidance("Set FACTOR.");
//   fFACTORCmd->SetParameterName("factor", false);
//   fFACTORCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

SteppingActionMessenger::~SteppingActionMessenger()
{
  delete fFACTORCmd;
  delete fActDir;
}


void SteppingActionMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{ 

//  if( command == fFACTORCmd )
//    { fSteppingAction->SetFactor(fFACTORCmd->GetNewDoubleValue(newValue));}

}
