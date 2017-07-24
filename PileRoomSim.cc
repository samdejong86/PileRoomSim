//
/// \file electromagnetic/PileRoomSim/PileRoomSim.cc
/// \brief Main program of the electromagnetic/PileRoomSim example
//
//
// $Id: PileRoomSim.cc 76340 2013-11-08 15:21:09Z gcosmo $

#include <algorithm>

#include "G4RunManager.hh"

#include "G4UImanager.hh"
#include "Randomize.hh"

#include "DetectorConstruction.hh"
#include "Commands.h"
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
  bool verbose=false;
  G4String nevents="";
  G4String seed="";
  G4String outfile="";
  bool gui=true;
  bool vis=true;
  
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
    }else if(input.compare("-verbose")==0){
      verbose=true;
    }else if(input.compare("-n")==0){
      if(i+1<argc){ 
	std::string next(argv[i+1]);
	if(next.compare(0,1,"-") != 0)
	  nevents = "/run/beamOn "+string(argv[i+1]);
	gui=false;
      }
    }else if(input.compare("-s")==0){
      if(i+1<argc){ 
	std::string next(argv[i+1]);
	if(next.compare(0,1,"-") != 0)
	  seed = "/random/setSeeds "+ string(argv[i+1]);
	gui=false;
      }
      if(i+2<argc){ 
	std::string next(argv[i+2]);
	if(next.compare(0,1,"-") != 0)
	  seed = seed + " " + string(argv[i+2]);
      }
    }else if(input.compare("-r")==0){
      if(i+1<argc){ 
	std::string next(argv[i+1]);
	if(next.compare(0,1,"-") != 0)
	  outfile = "/analysis/setFileName " + string(argv[i+1]);
	gui=false;
      }
    }else if(input.compare("-novis")==0){
	vis=false;
    }else if(input.compare("-h")==0){
      
#ifdef PORTABLE
      cout<<"Using the portable version: no visualization available\n\n"<<endl;
#endif

      cout<<"usage: PileRoomSim  [-h] [-m MACRO] [-d HE3TUBEDESCRIPTION] [-g GRAPHITEDESCRIPTION]\n";
      cout<<"                    [-o MISCOBJECTS] [-n NEVENTS] [-s SEED1 SEED2] [-r ROOTFILE]\n";
      cout<<"                    [-novis] [-all]\n";

      cout<<"\nSimulates helium-3 tube response to AmBe source in the centre of a graphite cube.\n";

      cout<<"\noptional arguments:"<<endl;
      cout<<"  -h,                      show this help message and exit\n";
      cout<<"  -m MACRO                 The macro to run. If this parameters is used, inputs\n";
      cout<<"                           from -n, -s, and -r are ignored\n";
      cout<<"  -d HE3TUBEDESCRIPTION    An xml file containing a description of the helium-3 \n";
      cout<<"                           tubes. If this parameter is unused, a default file is\n";
      cout<<"                           used.\n";
      cout<<"  -g GRAPHITEDESCRIPTION   An xml file containing a description of the graphite cube.\n";
      cout<<"                           If this parameter is unused, a default file is used\n";
      cout<<"  -o MISCOBJECTS           An xml file containing additional objects to be\n";
      cout<<"                           implemented.\n";
      cout<<"  -n NEVENTS               Number of events to run. If used, -r must be used\n";
      cout<<"  -s SEED1 SEED2           Seeds for random number generator.\n";
      cout<<"  -r ROOTFILE              Output rootfile (without .root extension). If used, -n\n";
      cout<<"                           must be used\n";
      cout<<"  -all                     If this parameter is used, all events are saved to the \n";
      cout<<"                           output ntuple. If not, only events containing a neutron\n"; 
      cout<<"                           hit in in a helium-3 tube are saved.\n";
      cout<<"  -novis                   If no other parameters specified, PileRoomSum runs in\n";
      cout<<"                           interactive mode with no visualization set.\n";

      cout<<"\nIf no arguments are specified, PileRoomSim runs in visualization mode\n";

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

  runManager->SetVerboseLevel(verbose);

  //physics list
  G4VModularPhysicsList* physicsList = new FTFP_BERT_HP(verbose);

  runManager->SetUserInitialization(physicsList);
  //pass he3tube description and misc object description filenames to DetectorConstruction
  DetectorConstruction* detConstruction = new DetectorConstruction(he3Desc, miscFile, graphiteDesc, verbose);  
  runManager->SetUserInitialization(detConstruction);
    
  
  // set user action classes
 //pass misc object description filename ActionInitilization, when then passes it on to RunAction
  runManager->SetUserInitialization(new ActionInitialization(detConstruction, miscFile, saveAll));

  //Initialize G4 kernel
  runManager->Initialize();
    
  // get the pointer to the User Interface manager 
  G4UImanager* UI = G4UImanager::GetUIpointer();  


  
  if(fileName.size()!=0){ //user specified script
     G4String command = "/control/execute ";
     UI->ApplyCommand(command+fileName);  

  } else if(!gui){  //user specified seed, number of events, and output file

      if(outfile.size()==0){
	cout<<"\033[1;31mNo output file specified! quitting.\n\033[0m";
	return 0;
      }if(nevents.size()==0){
	cout<<"\033[1;31mNumber of events not specified! quitting.\n\033[0m";
	return 0;
      }

      int n = std::count(seed.begin(), seed.end(), ' ');
      
      if(n<2) cout<<"\033[1mWarning: Wrong number of seeds.\n\033[0m";
      else UI->ApplyCommand(seed);

      std::vector<std::string> commands = UICommands();
  
      for(int i=0; i<(int)commands.size(); i++) UI->ApplyCommand(commands[i]);
      UI->ApplyCommand(outfile);
      UI->ApplyCommand(nevents);
      

   
  }  else           // define visualization and UI terminal for interactive mode 
    { 
#ifdef G4VIS_USE
      G4VisManager* visManager = new G4VisExecutive;
      visManager->Initialize();
#endif
      
#ifdef G4UI_USE
      G4UIExecutive * ui = new G4UIExecutive(argc,argv);      

#ifndef PORTABLE
      if(vis){
	std::vector<std::string> commands = visCommands();
	for(int i=0; i<(int)commands.size(); i++) UI->ApplyCommand(commands[i]);
      }
#endif

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


