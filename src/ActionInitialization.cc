//
// $Id: ActionInitialization.cc 76340 2013-11-08 15:21:09Z gcosmo $
//
/// \file ActionInitialization.cc
/// \brief Implementation of the ActionInitialization class

#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "SteppingVerbose.hh"
#include "DetectorConstruction.hh"


ActionInitialization::ActionInitialization(DetectorConstruction* detConstruction)
 : G4VUserActionInitialization(),
    fDetConstruction(detConstruction)
{}

ActionInitialization::~ActionInitialization()
{}


void ActionInitialization::BuildForMaster() const
{
 SetUserAction(new RunAction);
}



void ActionInitialization::Build() const
{
  

  //  SetUserAction(new PrimaryGeneratorAction);
  

  EventAction* eventAction = new EventAction(fDetConstruction);
  SetUserAction(new PrimaryGeneratorAction(eventAction));
  SetUserAction(eventAction);
  SetUserAction(new SteppingAction(fDetConstruction,eventAction));
  SetUserAction(new RunAction);
  
  
  
  /*
  SetUserAction(new RunAction);
  EventAction* eventAction = new EventAction;
  
  SetUserAction(new PrimaryGeneratorAction(eventAction));
  SetUserAction(eventAction);
  SetUserAction(new SteppingAction(fDetConstruction,eventAction));

  */
}  


G4VSteppingVerbose* ActionInitialization::InitializeSteppingVerbose() const
{
  return new SteppingVerbose();
}  

