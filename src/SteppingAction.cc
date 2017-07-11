//
/// \file electromagnetic/PileRoomSim/src/SteppingAction.cc
/// \brief Implementation of the SteppingAction class
//
//
// $Id: SteppingAction.cc 67268 2013-02-13 11:38:40Z ihrivnac $
//
// 
#include "SteppingAction.hh"
#include "EventAction.hh"
#include "G4SteppingManager.hh"
#include "G4RunManager.hh"
#include "DetectorConstruction.hh"
#include "Analysis.hh"
#include "SteppingActionMessenger.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4VProcess.hh"
#include <G4StepPoint.hh>
#include <G4Step.hh>

// 
SteppingAction::SteppingAction(const DetectorConstruction* detectorConstruction,EventAction* EvAct)
:G4UserSteppingAction(),fSteppingActionMessenger(0),fDetConstruction(detectorConstruction)
,fEventAction(EvAct)
{ 
  //       fSteppingActionMessenger = new SteppingActionMessenger(this);

  
  nch = fDetConstruction->GetNChannels();
  nobj = fDetConstruction->GetNMiscObjects();

}

SteppingAction::~SteppingAction()
{ 
  //delete fSteppingActionMessenger;  
}

void SteppingAction::UserSteppingAction(const G4Step* aStep)
{

  //G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();  
  G4double EdepStep = aStep->GetTotalEnergyDeposit();
  G4Track* a1Track = aStep->GetTrack();
  G4VPhysicalVolume* post_volume = aStep->GetPostStepPoint()->GetTouchableHandle()->GetVolume();
  G4VPhysicalVolume* pre_volume = aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
  int PDG=a1Track->GetDefinition()->GetPDGEncoding();

 
  if(pre_volume->GetName().contains("Rod") && !post_volume->GetName().contains("Rod")){
    fEventAction->leftGraphite(a1Track->GetKineticEnergy());
  }

  for(int i=0; i<nch; i++){
    if ( post_volume == fDetConstruction->Getphysicry(i) && EdepStep!=0) {
      if(PDG==2212||PDG==1000010030) fEventAction->He3Hit(PDG, EdepStep, i);
    }
  }

  for(int i=0; i<nobj; i++){
    if ( post_volume == fDetConstruction->GetphysiMisc(i) && EdepStep!=0) {
      fEventAction->leftObject(i);
    }

  }


  if(post_volume ==  fDetConstruction->GetphysiRoom()){
    fEventAction->leftWall();
  }

}


