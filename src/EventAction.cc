//
/// \file electromagnetic/PileRoomSim/src/EventAction.cc
/// \brief Implementation of the EventAction class
//
// $Id: EventAction.cc 75839 2013-11-06 17:27:26Z gcosmo $
//
#include <iostream>
#include "EventAction.hh"
#include "Analysis.hh"

#include "G4Event.hh"
#include "G4UnitsTable.hh"
#include "DetectorConstruction.hh"
#include "G4SystemOfUnits.hh"


EventAction::EventAction(const DetectorConstruction* detectorConstruction)
:G4UserEventAction(),
 TotalEnergyDeposit(0.),
 fDetConstruction(detectorConstruction)
{ 

}

EventAction::~EventAction()
{ 
}

void EventAction::Initialize(){
  nChannels=fDetConstruction->GetNChannels();

  //tubeX.resize(nChannels);
  //tubeY.resize(nChannels);
  //tubeZ.resize(nChannels);
  tubeLoc.resize(nChannels);
  
  for(int i=0; i<nChannels; i++){  
    tubeLoc[i] = fDetConstruction->GetHe3Posn(i); 
    //tubeX[i] = tupePos.x();
    //tubeY[i] = tupePos.y();
    //tubeZ[i] = tupePos.z();
  }

}

void EventAction::BeginOfEventAction( const G4Event* eve)
{ 


 neutronHit=false;
 isNeutronHitVec.clear();
 isNeutronHitVec.resize(nChannels);
 leftGraphite=false;
 leftConcrete=false;
 ePostGraphite=-1;

 edepInHe3.clear();
 PIDinHe3.clear();
 he3Ch.clear();
 channelVec.clear();
 TotalEnergyDeposit.clear();
 neutronHitVec.clear();
 tubeX.clear();
 tubeY.clear();
 tubeZ.clear();
 

 he3Ch.resize(nChannels);
 TotalEnergyDeposit.resize(nChannels);

 nevent++;
 if(nevent%100==0) G4cout<<"Event number: "<<nevent<<"\n";


}

void EventAction::EndOfEventAction( const G4Event*)
{    
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  if(neutronHit) G4cout<<"Neutron Hit!\n";
  
  //fill primitive ntuple branches
  analysisManager->FillNtupleDColumn(0, ePostGraphite/eV);
  analysisManager->FillNtupleDColumn(1, input_energy/MeV);
  if(neutronHit) analysisManager->FillNtupleIColumn(2, 1);
  else analysisManager->FillNtupleIColumn(2, 0);

  if(leftConcrete) analysisManager->FillNtupleIColumn(3, 1);
  else analysisManager->FillNtupleIColumn(3, 0);
  
  for(int i=0; i<nChannels; i++) 
    if(isNeutronHitVec[i]) 
      neutronHitVec.push_back(i);
  

  //vector branches are filled automatically
  
  analysisManager->AddNtupleRow();
  
  

}



