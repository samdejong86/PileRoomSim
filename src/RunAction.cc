//
/// \file electromagnetic/PileRoomSim/src/RunAction.cc
/// \brief Implementation of the RunAction class
//
// $Id: RunAction.cc 79239 2014-02-20 16:00:06Z gcosmo $
// 

#include <sys/time.h>
#include <sstream>

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
#include "DetectorConstruction.hh"

#include "xmlParser.h"
#include "formattingStrings.hh"

RunAction::RunAction(DetectorConstruction* detConstruction)
  : G4UserRunAction(),
    fDetConstruction(detConstruction)
{
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetVerboseLevel(fDetConstruction->GetVerbose());  
  analysisManager->SetFirstHistoId(1);
   
 
  saveGeo = fDetConstruction->GetSaveState();

  //create ntuple
  dataNtuple  = analysisManager->CreateNtuple("PileRoomSim", "Simulation of Pile Room");

  miscObjects = fDetConstruction->GetMiscParams(); 


  if(saveGeo){

    //geometry tree
    GeoNtuple = analysisManager->CreateNtuple("Geometry", "Geometry of Pile Room");
    analysisManager->CreateNtupleIColumn(GeoNtuple, "nProcessedEvents");
    

    tubes = fDetConstruction->GetTubeParams();
    
    HE3tags = tubes[0].getTags();  
    HE3vals.resize(HE3tags.size());
    
    for(int i=0; i<(int)HE3tags.size(); i++){
      analysisManager->CreateNtupleDColumn(GeoNtuple, "HE3TUBE_"+HE3tags[i], HE3vals[i]);
    }
    
    if(miscObjects.size()!=0){
      MISCtags = miscObjects[0].getTags();
      MISCvals.resize(MISCtags.size());
      
      for(int i=0; i<(int)MISCtags.size(); i++){
	analysisManager->CreateNtupleDColumn(GeoNtuple, "MISC_"+MISCtags[i], MISCvals[i]);
      }
    }
    
    gParam = fDetConstruction->GetgParam();
    GRAtags = gParam.getTags();
    GRAvals.resize(GRAtags.size());
    for(int i=0; i<(int)GRAtags.size(); i++){
      analysisManager->CreateNtupleDColumn(GeoNtuple, "GRAPHITE_"+GRAtags[i], GRAvals[i]);
    }
    
    rParam = fDetConstruction->GetrParam();
    ROOMtags = rParam.getTags();
    ROOMvals.resize(ROOMtags.size());
    for(int i=0; i<(int)ROOMtags.size(); i++){
      analysisManager->CreateNtupleDColumn(GeoNtuple, "ROOM_"+ROOMtags[i], ROOMvals[i]);
    }
    
    
    analysisManager->FinishNtuple(GeoNtuple);
  }




  
  //primitive branches
  analysisManager->CreateNtupleDColumn(dataNtuple, "Ekin_n_PostGraphite");
  analysisManager->CreateNtupleDColumn(dataNtuple,"Etot_n_initial");
  analysisManager->CreateNtupleDColumn(dataNtuple,"TotalEnergyDeposited");
  analysisManager->CreateNtupleIColumn(dataNtuple,"leftWall");

  analysisManager->CreateNtupleDColumn(dataNtuple,"he3TubeXPos");
  analysisManager->CreateNtupleDColumn(dataNtuple,"he3TubeYPos");
  analysisManager->CreateNtupleDColumn(dataNtuple,"he3TubeZPos");
  

 
  //need to link to the EventAction object to get the vectors that will be added to the ntuple
  const EventAction* constEventAction = static_cast<const EventAction*>(G4RunManager::GetRunManager()->GetUserEventAction());
  EventAction* eventAction = const_cast<EventAction*>(constEventAction);
  
  eventAction->setdataNtuple(dataNtuple);
  for(int i=0; i<(int)miscObjects.size(); i++){   	
    analysisManager->CreateNtupleIColumn(dataNtuple,"left"+miscObjects[i].getStringValue("Name"));
  }
  
  //vector branches
  analysisManager->CreateNtupleDColumn(dataNtuple,"EDEPinHe3", eventAction->getEDEPvec());
  analysisManager->CreateNtupleIColumn(dataNtuple,"PIDinHe3", eventAction->getPIDvec());
  analysisManager->CreateNtupleIColumn(dataNtuple,"neutronHits", eventAction->getNeutronHits());


 
  //no more branches added after this
  analysisManager->FinishNtuple(dataNtuple);

}

RunAction::~RunAction()
{
   delete G4AnalysisManager::Instance();
}



void RunAction::BeginOfRunAction(const G4Run* aRun)
{
  gettimeofday(&timeMark,NULL);
  runStart = (double)timeMark.tv_sec + (double)timeMark.tv_usec/1000000.;

  // save Rndm status
  if (isMaster) G4Random::showEngineStatus();

  
  // Get analysis manager
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  const EventAction* constEventAction = static_cast<const EventAction*>(G4RunManager::GetRunManager()->GetUserEventAction());
  EventAction* eventAction = const_cast<EventAction*>(constEventAction);

  eventAction->Initialize();

  // Open an output file
  analysisManager->OpenFile();    

  if(saveGeo){

    for(int i=0; i<(int)tubes.size(); i++){
      for(int j=0; j<(int)HE3tags.size(); j++){
	HE3vals[j].push_back(tubes[i].getValue(HE3tags[j]));
      }
    }
    
    for(int i=0; i<(int)miscObjects.size(); i++){
      for(int j=0; j<(int)MISCtags.size(); j++){
	MISCvals[j].push_back(miscObjects[i].getValue(MISCtags[j]));
      }
    }

    for(int j=0; j<(int)GRAtags.size(); j++){
      GRAvals[j].push_back(gParam.getValue(GRAtags[j]));
    }
    for(int j=0; j<(int)ROOMtags.size(); j++){
      ROOMvals[j].push_back(rParam.getValue(ROOMtags[j]));
    }
    
    analysisManager->FillNtupleIColumn(GeoNtuple, 0, aRun->GetNumberOfEventToBeProcessed());
    analysisManager->AddNtupleRow(GeoNtuple);
  }

}

void RunAction::EndOfRunAction(const G4Run* run)
{  
  // show Rndm status
  if (isMaster) G4Random::showEngineStatus();         

  const EventAction* constEventAction = static_cast<const EventAction*>(G4RunManager::GetRunManager()->GetUserEventAction());
  EventAction* eventAction = const_cast<EventAction*>(constEventAction);  

  int nevents = run->GetNumberOfEvent();
  double meanTime, stdev;
  eventAction->getTime(nevents, meanTime, stdev);

  
  gettimeofday(&timeMark,NULL);
  runEnd = (double)timeMark.tv_sec + (double)timeMark.tv_usec/1000000.;
  


  G4cout<<topLeft;
  for(int i=0; i<60; i++) G4cout<<line;
  G4cout<<topRight<<G4endl;

  
  G4cout<<vertical<<"Number of events: "<<right<<setw(7)<<nevents<<", corresponding to "<<right<<setw(7)<<1000*(double)nevents/10000000<<" ms"<<right<<setw(6)<<" "<<vertical<<G4endl;

  G4cout<<leftDivider;
  for(int i=0; i<60; i++) G4cout<<thinLine;
  G4cout<<rightDivider<<G4endl;

  G4cout<<vertical<<right<<setw(7)<<eventAction->getnGraphite()<<" neutrons left the Graphite"<<right<<setw(26)<<" "<<vertical<<G4endl;
  G4cout<<vertical<<right<<setw(7)<<eventAction->getnNeutrons()<<" neutrons detected"<<right<<setw(35)<<" "<<vertical<<G4endl;


  G4cout<<leftDivider;
  for(int i=0; i<60; i++) G4cout<<thinLine;
  G4cout<<rightDivider<<G4endl;

  G4cout.precision(4);
  G4cout<<vertical<<" "<<right<<setw(7)<<meanTime<<plusMinus<<left<<setw(10)<<stdev<<" seconds per event"<<right<<setw(23)<<" "<<vertical<<G4endl;
  G4cout<<vertical<<" Total run time: "<<right<<setw(8)<<runEnd-runStart<<" seconds"<<right<<setw(27)<<" "<<vertical<<G4endl;



  G4cout<<bottomLeft;
  for(int i=0; i<60; i++) G4cout<<line;
  G4cout<<bottomRight<<G4endl;


  //save ntuple      
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();
}




