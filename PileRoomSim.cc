//
/// \file electromagnetic/PileRoomSim/PileRoomSim.cc
/// \brief Main program of the electromagnetic/PileRoomSim example
//
//
// $Id: PileRoomSim.cc 76340 2013-11-08 15:21:09Z gcosmo $


#include "G4RunManager.hh"

#include "G4UImanager.hh"
#include "Randomize.hh"

#include "DetectorConstruction.hh"
#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"
#include "G4OpticalPhysics.hh"
#include "FTFP_BERT_HP.hh"

#include "ActionInitialization.hh"
#include "SteppingVerbose.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif


int main(int argc,char** argv) {

  G4String fileName = "";
  G4String he3Desc = "";
  std::string miscFile = "";
  G4String graphiteDesc = "";
  bool saveAll=false;
  
  //parse command line arguments
  for(int i=0; i<argc; i++){
    std::string input(argv[i]);
    if(input.compare("-m") == 0){
      if(i+1<argc){ 
	std::string next(argv[i+1]);
	if(next.compare(0,1,"-") != 0)
	  fileName = string(argv[i+1]);
      }
    }else if(input.compare("-o")==0){
      if(i+1<argc){ 
	std::string next(argv[i+1]);
	if(next.compare(0,1,"-") != 0)
	   miscFile = string(argv[i+1]);
      }
    }else if(input.compare("-d")==0){
      if(i+1<argc){ 
	std::string next(argv[i+1]);
	if(next.compare(0,1,"-") != 0)
	   he3Desc = string(argv[i+1]);
      }
    }else if(input.compare("-g")==0){
      if(i+1<argc){ 
	std::string next(argv[i+1]);
	if(next.compare(0,1,"-") != 0)
	   graphiteDesc = string(argv[i+1]);
      }
    }else if(input.compare("-all")==0){
      saveAll=true;
    }else if(input.compare("-h")==0){
      
#ifdef PORTABLE
      cout<<"Using the portable version: no visualization available\n\n"<<endl;
#endif

      cout<<"usage: PileRoomSim  [-h] [-m MACRO] [-d HE3TUBEDESCRIPTION] [-g GRAPHITEDESCRIPTION]\n";
      cout<<"                    [-o MISCOBJECTS] [-all]\n";

      cout<<"\nSimulates helium-3 tube response to AmBe source in the centre of a graphite cube.\n";

      cout<<"\noptional arguments:"<<endl;
      cout<<"  -h,                      show this help message and exit\n";
      cout<<"  -m MACRO                 The macro to run. If this parameter is unused, the program runs interactively.\n";
      cout<<"  -d HE3TUBEDESCRIPTION    An xml file containing a description of the helium-3 tubes. If this parameter\n";
      cout<<"                           is unused, a default file is used\n";
      cout<<"  -g GRAPHITEDESCRIPTION   An xml file containing a description of the graphite cube. If this parameter\n";
      cout<<"                           is unused, a default file is used\n";
      cout<<"  -o MISCOBJECTS           An xml file containing additional objects to be implemented.\n";
      cout<<"  -all                     If this parameter is used, all events are saved to the output ntuple. If not,\n";
      cout<<"                           only events containing a neutron hit in in a helium-3 tube are saved.\n";

      return 0;
    }
  }

  //make sure environment variables are set
  char *checkEnv;
  checkEnv = getenv("G4NEUTRONHPDATA");
  if(checkEnv==NULL){
    cout<<"Need to source geant4.10.02-install/bin/geant4.sh before running\n";
    return 0;
  }


  //choose the Random engine
  G4Random::setTheEngine(new CLHEP::RanecuEngine);
  
  // Construct the default run manager
  G4VSteppingVerbose::SetInstance(new SteppingVerbose);  
  G4RunManager* runManager = new G4RunManager;

  //physics list
  G4VModularPhysicsList* physicsList = new FTFP_BERT_HP;

  runManager->SetUserInitialization(physicsList);
  DetectorConstruction* detConstruction = new DetectorConstruction(he3Desc, miscFile, graphiteDesc);  //pass he3tube description and misc object description filenames to DetectorConstruction
  runManager->SetUserInitialization(detConstruction);
    
  
  // set user action classes
  runManager->SetUserInitialization(new ActionInitialization(detConstruction, miscFile, saveAll)); //pass misc object description filename ActionInitilization, when then passes it on to RunAction

  //Initialize G4 kernel
  runManager->Initialize();
    
  // get the pointer to the User Interface manager 
    G4UImanager* UI = G4UImanager::GetUIpointer();  

    
    if(fileName.size()!=0)  //batch mode
    { 
     G4String command = "/control/execute ";
     UI->ApplyCommand(command+fileName);  
    }
   
  else           // define visualization and UI terminal for interactive mode 
    { 
#ifdef G4VIS_USE
     G4VisManager* visManager = new G4VisExecutive;
     visManager->Initialize();
#endif
     
#ifdef G4UI_USE
      G4UIExecutive * ui = new G4UIExecutive(argc,argv);      
      ui->SessionStart();
      delete ui;
#endif
     
#ifdef G4VIS_USE
     delete visManager;
#endif     
    }

  // job termination
  //
  delete runManager;

  return 0;
}


