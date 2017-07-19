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


EventAction::EventAction(const DetectorConstruction* detectorConstruction, bool save)
:G4UserEventAction(),
 TotalEnergyDeposit(0.),
 fDetConstruction(detectorConstruction),
 saveAll(save),
 nNeutrons(0),
 GraphiteTotal(0)
{ 
}

EventAction::~EventAction()
{ 
}

void EventAction::Initialize(){
  nChannels=fDetConstruction->GetNChannels();  //get number of channels
  nobj = fDetConstruction->GetNMiscObjects();  //get number of objects

  //get location of tubes
  tubeLoc.resize(nChannels);   
  for(int i=0; i<nChannels; i++){  
    tubeLoc[i] = fDetConstruction->GetHe3Posn(i); 
  }

}

//at the start of each event...
void EventAction::BeginOfEventAction( const G4Event* eve)
{ 
  int nevent = eve->GetEventID();

  neutronHit=false;    //no neutron hit
  isNeutronHitVec.clear();
  isNeutronHitVec.resize(nChannels);
  leftGrap=false;     //hasn't left graphite
  leftConcrete=false; //hasn't left concrete
  ePostGraphite=-1;
  
  leftObj.clear();      //hasn't left objects
  leftObj.resize(nobj); 
  
  
  //clear some vectors
  edepInHe3.clear();
  PIDinHe3.clear();
  neutronHitVec.clear();
  tubeX=0;//.clear();
  tubeY=0;//.clear();
  tubeZ=0;//.clear();
  
  
  TotalEnergyDeposit = 0; //.resize(nChannels);
  
  
  if(nevent%100000==0){
    cout<<bold<<red<<"Event number: "<<nevent<<noFormat<<"\n";
  }else if(eve->GetEventID()%10000==0){
    cout<<bold<<green<<"Event number: "<<nevent<<noFormat<<"\n";
  }else if (eve->GetEventID()%1000==0){
    cout<<bold<<"Event number: "<<nevent<<noFormat<<"\n";
  }else if (eve->GetEventID()%100==0){ 
    cout<<"Event number: "<<nevent<<"\n";
  }

}

//at the end of event
void EventAction::EndOfEventAction( const G4Event*)
{    

  if(leftGrap) GraphiteTotal++;

  if(neutronHit||saveAll){
    
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

    if(neutronHit){
      cout<<bold<<green<<"Neutron Hit!"<<noFormat<<endl;
      nNeutrons++;
    }    


    //fill primitive ntuple branches
    analysisManager->FillNtupleDColumn(0, ePostGraphite/eV);
    analysisManager->FillNtupleDColumn(1, input_energy/MeV);
    analysisManager->FillNtupleDColumn(2, TotalEnergyDeposit);
    if(leftConcrete) analysisManager->FillNtupleIColumn(3, 1);
    else analysisManager->FillNtupleIColumn(3, 0);
    
    analysisManager->FillNtupleDColumn(4, tubeX);
    analysisManager->FillNtupleDColumn(5, tubeY);
    analysisManager->FillNtupleDColumn(6, tubeZ);

    
    for(int i=0; i<nobj; i++){
      if(leftObj[i]==1) analysisManager->FillNtupleIColumn(7+i, 1);
      else analysisManager->FillNtupleIColumn(7+i, 0);
    }
    
    for(int i=0; i<nChannels; i++) 
      if(isNeutronHitVec[i]) 
	neutronHitVec.push_back(i);
    
    
    //vector branches are filled automatically
    
    analysisManager->AddNtupleRow();
  }
  

}



