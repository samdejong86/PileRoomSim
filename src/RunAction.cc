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
  analysisManager->CreateNtupleDColumn("TotalEnergyDeposited");
  analysisManager->CreateNtupleIColumn("leftWall");

  analysisManager->CreateNtupleDColumn("he3TubeXPos");
  analysisManager->CreateNtupleDColumn("he3TubeYPos");
  analysisManager->CreateNtupleDColumn("he3TubeZPos");


 
  //need to link to the EventAction object to get the vectors that will be added to the ntuple
  const EventAction* constEventAction = static_cast<const EventAction*>(G4RunManager::GetRunManager()->GetUserEventAction());
  EventAction* eventAction = const_cast<EventAction*>(constEventAction);
  
  //get the names of the misc objects to name the ntuple branches
  std::vector<XmlParser> miscObjects = XmlParser::getVector(miscFile);  
  for(int i=0; i<(int)miscObjects.size(); i++){
    analysisManager->CreateNtupleIColumn("left"+miscObjects[i].getStringValue("Name"));
  }

  //vector branches
  analysisManager->CreateNtupleDColumn("EDEPinHe3", eventAction->getEDEPvec());
  analysisManager->CreateNtupleIColumn("PIDinHe3", eventAction->getPIDvec());
  analysisManager->CreateNtupleIColumn("neutronHits", eventAction->getNeutronHits());


 
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
  if (isMaster) G4Random::showEngineStatus();

  
  // Get analysis manager
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  const EventAction* constEventAction = static_cast<const EventAction*>(G4RunManager::GetRunManager()->GetUserEventAction());
  EventAction* eventAction = const_cast<EventAction*>(constEventAction);

  eventAction->Initialize();

  // Open an output file
  analysisManager->OpenFile();    
}

void RunAction::EndOfRunAction(const G4Run* run)
{  
  // show Rndm status
  if (isMaster) G4Random::showEngineStatus();         

  const EventAction* constEventAction = static_cast<const EventAction*>(G4RunManager::GetRunManager()->GetUserEventAction());
  EventAction* eventAction = const_cast<EventAction*>(constEventAction);  

  int nevents = run->GetNumberOfEvent();

  G4cout<<"-------------------------------------------------------\n";
  G4cout<<"Number of events: "<<nevents<<", corresponding to "<<1000*(double)nevents/10000000<<" ms"<<G4endl<<G4endl;
  G4cout<<eventAction->getnNeutrons()<<" neutrons detected"<<G4endl;
  G4cout<<"-------------------------------------------------------\n";

  


  //save ntuple      
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();
}




