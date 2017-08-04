//
/// \file electromagnetic/PileRoomSim/src/RunAction.cc
/// \brief Implementation of the RunAction class
//
// $Id: RunAction.cc 79239 2014-02-20 16:00:06Z gcosmo $
// 

#include <sys/time.h>
#include <sstream>

#ifdef NOTIFY
#include <libnotify/notify.h>
#endif

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
  verbose = fDetConstruction->GetVerbose();
  
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetVerboseLevel(verbose);  
  
 
  saveGeo = fDetConstruction->GetSaveState();

  //create ntuple
  dataNtuple  = analysisManager->CreateNtuple("PileRoomSim", "Simulation of Pile Room");

  miscObjects = fDetConstruction->GetMiscParams(); 

  //create a tree for the geometry
  if(saveGeo)
    CreateGeoNtuple(analysisManager);

  
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
  if(verbose)
    if (isMaster) G4Random::showEngineStatus();

  
  // Get analysis manager
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  const EventAction* constEventAction = static_cast<const EventAction*>(G4RunManager::GetRunManager()->GetUserEventAction());
  EventAction* eventAction = const_cast<EventAction*>(constEventAction);

  eventAction->Initialize();

  // Open an output file
  analysisManager->OpenFile();    

  //save the geometry
  if(saveGeo)
    FillGeoNtuple(analysisManager, aRun);

}

void RunAction::EndOfRunAction(const G4Run* run)
{  
  // show Rndm status
  if(verbose)
    if (isMaster) G4Random::showEngineStatus();         

  gettimeofday(&timeMark,NULL);
  runEnd = (double)timeMark.tv_sec + (double)timeMark.tv_usec/1000000.;

  report(run);
  
  //save ntuple      
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();
}


//--------------------------------------------------------------------------------------------------

//create geometry ntuple
void RunAction::CreateGeoNtuple(G4AnalysisManager* analysisManager){

    //geometry tree
    GeoNtuple = analysisManager->CreateNtuple("Geometry", "Geometry of Pile Room");

    //save number of events (since data ntuple will only have he3tube events by default
    analysisManager->CreateNtupleIColumn(GeoNtuple, "nProcessedEvents");

    //some string branches (unfortunately these cannot be vectors.
    analysisManager->CreateNtupleSColumn(GeoNtuple, "HE3TUBE_Material");
    analysisManager->CreateNtupleSColumn(GeoNtuple, "GRAPHITE_Material");
    analysisManager->CreateNtupleSColumn(GeoNtuple, "ROOM_Material");

    stringstream ss;
    for(int i=0; i<(int)miscObjects.size(); i++){
      ss<<i;
      G4String n=ss.str();
      ss.str("");
      
      analysisManager->CreateNtupleSColumn(GeoNtuple, "MISC_"+n+"_Material");
      analysisManager->CreateNtupleSColumn(GeoNtuple, "MISC_"+n+"_Name");
      analysisManager->CreateNtupleSColumn(GeoNtuple, "MISC_"+n+"_Shape");
      
    }

    //get the he3tube parameters from the detector geometry class
    tubes = fDetConstruction->GetTubeParams();
    
    //get the he3tube tags
    HE3tags = tubes[0].getTags();  
    HE3vals.resize(HE3tags.size());
    
    //add a vector branch for each tag in the he3 description
    for(int i=0; i<(int)HE3tags.size(); i++){
      if(HE3tags[i].compare("Colour") == 0 || HE3tags[i].compare("Material") == 0) continue;  //exclude tags that are strings.
      analysisManager->CreateNtupleDColumn(GeoNtuple, "HE3TUBE_"+HE3tags[i], HE3vals[i]);
    }
    
    //make sure there are misc. objects defined
    if(miscObjects.size()!=0){
         
      //get the misc object tags
      MISCtags = miscObjects[0].getTags();
      MISCvals.resize(MISCtags.size());
      
      //add a vector branch for each tag in the misc description
      for(int i=0; i<(int)MISCtags.size(); i++){
	//exclude tags that are strings.
	if(MISCtags[i].compare("Colour") == 0 || MISCtags[i].compare("Material") == 0 || MISCtags[i].compare("Name") == 0 || MISCtags[i].compare("Shape") == 0) continue;
	analysisManager->CreateNtupleDColumn(GeoNtuple, "MISC_"+MISCtags[i], MISCvals[i]);
      }
    }
    //get the graphite  tags
    gParam = fDetConstruction->GetgParam();
    GRAtags = gParam.getTags();
    GRAvals.resize(GRAtags.size());
    
    //add a vector branch for each tag in the graphite  description
    for(int i=0; i<(int)GRAtags.size(); i++){
      if(GRAtags[i].compare("Colour") == 0 || GRAtags[i].compare("Material") == 0) continue;//exclude tags that are strings.
      analysisManager->CreateNtupleDColumn(GeoNtuple, "GRAPHITE_"+GRAtags[i], GRAvals[i]);
    }
    //get the room  tags
    rParam = fDetConstruction->GetrParam();
    ROOMtags = rParam.getTags();
    ROOMvals.resize(ROOMtags.size());
    
    //add a vector branch for each tag in the graphite  description
    for(int i=0; i<(int)ROOMtags.size(); i++){
      if(ROOMtags[i].compare("Colour") == 0 || ROOMtags[i].compare("Material") == 0) continue;//exclude tags that are strings.
      analysisManager->CreateNtupleDColumn(GeoNtuple, "ROOM_"+ROOMtags[i], ROOMvals[i]);
    }
    
    //that's all the branches
    analysisManager->FinishNtuple(GeoNtuple);


}


//--------------------------------------------------------------------------------------------------

//fill geometry ntuple
void RunAction::FillGeoNtuple(G4AnalysisManager* analysisManager, const G4Run* aRun){

    //save values of the he3tags, excluding strings
    for(int i=0; i<(int)tubes.size(); i++){
      for(int j=0; j<(int)HE3tags.size(); j++){
	if(HE3tags[j].compare("Colour") == 0 || HE3tags[j].compare("Material") == 0) continue;
	HE3vals[j].push_back(tubes[i].getValue(HE3tags[j]));
      }
    }
    
    for(int i=0; i<(int)miscObjects.size(); i++){
      for(int j=0; j<(int)MISCtags.size(); j++){
	if(MISCtags[j].compare("Colour") == 0 || MISCtags[j].compare("Material") == 0 || MISCtags[j].compare("Name") == 0 || MISCtags[j].compare("Shape") == 0) continue;
	MISCvals[j].push_back(miscObjects[i].getValue(MISCtags[j]));
      }
    }

    for(int j=0; j<(int)GRAtags.size(); j++){
      if(GRAtags[j].compare("Colour") == 0 || GRAtags[j].compare("Material") == 0) continue;
      GRAvals[j].push_back(gParam.getValue(GRAtags[j]));
    }
    for(int j=0; j<(int)ROOMtags.size(); j++){
      if(ROOMtags[j].compare("Colour") == 0 || ROOMtags[j].compare("Material") == 0) continue;
      ROOMvals[j].push_back(rParam.getValue(ROOMtags[j]));
    }
    
    //save the string values to 
    analysisManager->FillNtupleIColumn(GeoNtuple, 0, aRun->GetNumberOfEventToBeProcessed());
    analysisManager->FillNtupleSColumn(GeoNtuple, 1, tubes[0].getStringValue("Material"));
    analysisManager->FillNtupleSColumn(GeoNtuple, 2, gParam.getStringValue("Material"));
    analysisManager->FillNtupleSColumn(GeoNtuple, 3, rParam.getStringValue("Material"));

    //save the misc parameter string values
    int n=4;
    for(int i=0; i<(int)miscObjects.size(); i++){
      analysisManager->FillNtupleSColumn(GeoNtuple, n, miscObjects[0].getStringValue("Material"));
      n++;
      analysisManager->FillNtupleSColumn(GeoNtuple, n, miscObjects[0].getStringValue("Name"));
      n++;
      analysisManager->FillNtupleSColumn(GeoNtuple, n, miscObjects[0].getStringValue("Shape"));
      n++;
    }


    analysisManager->AddNtupleRow(GeoNtuple);

}


//--------------------------------------------------------------------------------------------------

//print an end of run report
void RunAction::report(const G4Run* run){

const EventAction* constEventAction = static_cast<const EventAction*>(G4RunManager::GetRunManager()->GetUserEventAction());
  EventAction* eventAction = const_cast<EventAction*>(constEventAction);  

  int nevents = run->GetNumberOfEvent();
  double meanTime, stdev;
  eventAction->getTime(nevents, meanTime, stdev);

  stringstream ss;
  ss<<runEnd-runStart;
  string runTime = ss.str();
  


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
  G4cout<<vertical<<" Total run time: "<<right<<setw(8)<<runTime<<" seconds"<<right<<setw(27)<<" "<<vertical<<G4endl;



  G4cout<<bottomLeft;
  for(int i=0; i<60; i++) G4cout<<line;
  G4cout<<bottomRight<<G4endl;


#ifdef NOTIFY

  ss.str("");
  ss<<nevents;
  string numEvents = ss.str();


  char *cd;
  cd = getenv("PWD");
  string pwd = cd;

  string subMessage = numEvents+" events, runtime of "+runTime+"s";
  string icon = pwd+"/43-Geant4-logoV5.png";


  notify_init("Sample");
  NotifyNotification* n = notify_notification_new ("Run Complete", 
						   subMessage.c_str(),
						   icon.c_str());
  notify_notification_set_timeout(n, 10000); // 10 seconds
  
  if (!notify_notification_show(n, 0)) 
    {
      std::cerr << "show has failed" << std::endl;
      return ;
    }

#endif

}
