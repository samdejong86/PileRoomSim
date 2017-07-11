//
/// \file electromagnetic/PileRoomSim/src/RunAction.cc
/// \brief Implementation of the RunAction class
//
// $Id: RunAction.cc 79239 2014-02-20 16:00:06Z gcosmo $
// 

#include "RunAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"

#include "G4EmCalculator.hh"
#include "G4UnitsTable.hh"
#include <G4Electron.hh>
#include <G4Electron.hh>
#include <G4PionMinus.hh>
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "EventAction.hh"

#include "xmlParser.h"

RunAction::RunAction(std::string miscFile)
 : G4UserRunAction()
{
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetVerboseLevel(1);  
  analysisManager->SetFirstHistoId(1);
   

  //create ntuple
  analysisManager->CreateNtuple("PileRoomSim", "Simulation of Pile Room");
  
  //primitive branches
  analysisManager->CreateNtupleDColumn("Ekin_n_PostGraphite");
  analysisManager->CreateNtupleDColumn("Etot_n_initial");
  analysisManager->CreateNtupleIColumn("IsNeutronInHe3");
  analysisManager->CreateNtupleIColumn("leftWall");


  


 
  //need to link to the EventAction object to get the vectors that will be added to the ntuple
  const EventAction* constEventAction = static_cast<const EventAction*>(G4RunManager::GetRunManager()->GetUserEventAction());
  EventAction* eventAction = const_cast<EventAction*>(constEventAction);
  
  std::vector<XmlParser> miscObjects = XmlParser::getVector(miscFile);
  
  for(int i=0; i<(int)miscObjects.size(); i++){
    analysisManager->CreateNtupleIColumn("left"+miscObjects[i].getStringValue("Name"));
  }




  //vector branches
  analysisManager->CreateNtupleDColumn("EDEPinHe3", eventAction->getEDEPvec());
  analysisManager->CreateNtupleIColumn("PIDinHe3", eventAction->getPIDvec());
  analysisManager->CreateNtupleIColumn("channel", eventAction->getChannelVec());
  analysisManager->CreateNtupleIColumn("neutronHits", eventAction->getNeutronHits());

  //these always have 4 entries
  analysisManager->CreateNtupleIColumn("he3Hits", eventAction->getHitvec());
  analysisManager->CreateNtupleDColumn("TotalEnergyDeposited", eventAction->getTotalEdep());
  analysisManager->CreateNtupleDColumn("he3TubeXPos", eventAction->getTubeX());
  analysisManager->CreateNtupleDColumn("he3TubeYPos", eventAction->getTubeY());
  analysisManager->CreateNtupleDColumn("he3TubeZPos", eventAction->getTubeZ());

 
  //no more branches added after this
  analysisManager->FinishNtuple();

}

RunAction::~RunAction()
{
   delete G4AnalysisManager::Instance();
}



void RunAction::BeginOfRunAction(const G4Run*)
{
  // save Rndm status
  //G4RunManager::GetRunManager()->SetRandomNumberStore(true);
  if (isMaster) G4Random::showEngineStatus();

  
   // Get analysis manager
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  const EventAction* constEventAction = static_cast<const EventAction*>(G4RunManager::GetRunManager()->GetUserEventAction());
  EventAction* eventAction = const_cast<EventAction*>(constEventAction);

  eventAction->Initialize();

  // Open an output file
  //
//   G4String fileName = "testem4";
  analysisManager->OpenFile();    
}

void RunAction::EndOfRunAction(const G4Run*)
{  
  // show Rndm status
  if (isMaster) G4Random::showEngineStatus();         

  //save ntuple      
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();
}




