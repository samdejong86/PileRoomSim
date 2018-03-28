//
/// \file electromagnetic/PileRoomSim/src/EventAction.cc
/// \brief Implementation of the EventAction class
//
// $Id: EventAction.cc 75839 2013-11-06 17:27:26Z gcosmo $
//
#include <iostream>
#include <sys/time.h>

#include "EventAction.hh"
#include "Analysis.hh"

#include "G4Event.hh"
#include "G4UnitsTable.hh"
#include "DetectorConstruction.hh"
#include "G4SystemOfUnits.hh"

#include "formattingStrings.hh"
#include "xmlParser.h"

EventAction::EventAction(const DetectorConstruction* detectorConstruction, bool save)
:G4UserEventAction(),
 TotalEnergyDeposit(0.),
 fDetConstruction(detectorConstruction),
 saveAll(save),
 nNeutrons(0),
 GraphiteTotal(0),
 eventDuration(0),
 M(0),
 timeStdev(0)
{
  //fillGeoNtuple();
  //diffusionRadii.resize(10);
  for(int i=0; i<100; i++){
    diffusionRadii.push_back(300.+700.*(double)i/100.);
  }

  
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

  gettimeofday(&timeMark,NULL);
  startTime = (double)timeMark.tv_sec + (double)timeMark.tv_usec/1000000.;

  neutronHit=false;    //no neutron hit
  isNeutronHitVec.clear();
  isNeutronHitVec.resize(nChannels);
  leftGrap=false;     //hasn't left graphite
  leftConcrete=false; //hasn't left concrete
  ePostGraphite=-1;


  nCrossedRadii.clear();
  nCrossedRadii.resize(diffusionRadii.size());

  
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
    cout<<bold2<<red2;
    G4cout<<"Event number: "<<nevent<<G4endl;
    cout<<noFormat2;
  }else if(eve->GetEventID()%10000==0){
    cout<<bold2<<blue2;
    G4cout<<"Event number: "<<nevent<<G4endl;
    cout<<noFormat2;
  }else if (eve->GetEventID()%1000==0){
    cout<<bold2;
    G4cout<<"Event number: "<<nevent<<G4endl;
    cout<<noFormat2;
  }else if (eve->GetEventID()%100==0){ 
    G4cout<<"Event number: "<<nevent<<G4endl;
  }

}

//at the end of event
void EventAction::EndOfEventAction( const G4Event* eve)
{    

  if(leftGrap) GraphiteTotal++;

  if(neutronHit||saveAll){
    
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

    if(neutronHit){
      cout<<bold2<<green2;
      G4cout<<"Neutron Hit!"<<G4endl;
      cout<<noFormat2;
      nNeutrons++;
    }    


    //fill primitive ntuple branches
    analysisManager->FillNtupleDColumn(dataNtuple, 0, ePostGraphite/eV);
    analysisManager->FillNtupleDColumn(dataNtuple, 1, input_energy/MeV);
    analysisManager->FillNtupleDColumn(dataNtuple, 2, TotalEnergyDeposit);
    if(leftConcrete) analysisManager->FillNtupleIColumn(dataNtuple, 3, 1);
    else analysisManager->FillNtupleIColumn(dataNtuple, 3, 0);
    
    analysisManager->FillNtupleDColumn(dataNtuple, 4, tubeX);
    analysisManager->FillNtupleDColumn(dataNtuple, 5, tubeY);
    analysisManager->FillNtupleDColumn(dataNtuple, 6, tubeZ);

    analysisManager->FillNtupleIColumn(dataNtuple, 7, ParticleID);
    
    for(int i=0; i<nobj; i++){
      if(leftObj[i]==1) analysisManager->FillNtupleIColumn(dataNtuple, 8+i, 1);
      else analysisManager->FillNtupleIColumn(dataNtuple, 8+i, 0);
    }
    
    for(int i=0; i<nChannels; i++) 
      if(isNeutronHitVec[i]) 
	neutronHitVec.push_back(i);
    
    
    //vector branches are filled automatically
    
    analysisManager->AddNtupleRow(dataNtuple);

  }


  gettimeofday(&timeMark,NULL);
  endTime = (double)timeMark.tv_sec + (double)timeMark.tv_usec/1000000.;
  

  double thisEventTime=endTime-startTime;

  double delta = thisEventTime-eventDuration;
  eventDuration += delta/(eve->GetEventID()+1);
  double delta2 = thisEventTime-eventDuration;
  M=delta*delta2;
  

}

