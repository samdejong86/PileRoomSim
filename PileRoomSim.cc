//
/// \file electromagnetic/PileRoomSim/PileRoomSim.cc
/// \brief Main program of the electromagnetic/PileRoomSim example
//
//
// $Id: PileRoomSim.cc 76340 2013-11-08 15:21:09Z gcosmo $

#include <algorithm>
#include <sstream>

#include "G4RunManager.hh"

#include "G4UImanager.hh"
#include "Randomize.hh"

#include "DetectorConstruction.hh"
#include "Commands.h"
#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"
#include "G4OpticalPhysics.hh"
#include "FTFP_BERT_HP.hh"
#include "G4HadronicProcessStore.hh"

#include "ActionInitialization.hh"
#include "SteppingVerbose.hh"

#include "formattingStrings.hh"

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
  bool saveGeo=true;
  
  //parse command line arguments
  for(int i=0; i<argc; i++){
    std::string input(argv[i]);
    if(input.compare("-m") == 0){    //macro file
      if(i+1<argc){ 
	std::string next(argv[i+1]);
	if(next.compare(0,1,"-") != 0)
	  fileName = string(argv[i+1]);
	gui=false;
      }
    }else if(input.compare("-o")==0){  //misc objects
      if(i+1<argc){ 
	std::string next(argv[i+1]);
	if(next.compare(0,1,"-") != 0)
	   miscFile = string(argv[i+1]);
      }
    }else if(input.compare("-d")==0){  //he3tube description
      if(i+1<argc){ 
	std::string next(argv[i+1]);
	if(next.compare(0,1,"-") != 0)
	   he3Desc = string(argv[i+1]);
      }
    }else if(input.compare("-g")==0){  //graphite description
      if(i+1<argc){ 
	std::string next(argv[i+1]);
	if(next.compare(0,1,"-") != 0)
	   graphiteDesc = string(argv[i+1]);
      }
    }else if(input.compare("-all")==0){   //save all events
      saveAll=true;
    }else if(input.compare("-noGeo")==0){  //save geometry
      saveGeo=false;
    }else if(input.compare("-verbose")==0){ //verbose
      verbose=true;
    }else if(input.compare("-n")==0){   //number of events
      if(i+1<argc){ 
	std::string next(argv[i+1]);
	if(next.compare(0,1,"-") != 0)
	  nevents = "/run/beamOn "+string(argv[i+1]);
	gui=false;
      }
    }else if(input.compare("-s")==0){   //seeds
      if(i+1<argc){ 
	std::string next(argv[i+1]);
	if(next.compare(0,1,"-") != 0)
	  seed = "/random/setSeeds "+ string(argv[i+1]);
      }
      if(i+2<argc){ 
	std::string next(argv[i+2]);
	if(next.compare(0,1,"-") != 0)
	  seed = seed + " " + string(argv[i+2]);
      }
    }else if(input.compare("-r")==0){   //root filename
      if(i+1<argc){ 
	std::string next(argv[i+1]);
	if(next.compare(0,1,"-") != 0)
	  outfile = "/analysis/setFileName " + string(argv[i+1]);
	gui=false;
      }
    }else if(input.compare("-novis")==0){   //no visualization
	vis=false;
    }else if(input.compare("-h")==0){   //help
      
#ifdef PORTABLE
      cout<<"Using the portable version: no visualization available\n\n"<<endl;
#endif
      help();


      return 0;
    }
  }
  
  G4cout<<topLeft;
  for(int i=0; i<60; i++) G4cout<<line;
  G4cout<<topRight<<G4endl;

  G4cout<<vertical<<"Summary of settings:";
  for(int i=0; i<40; i++) G4cout<<" ";
  G4cout<<vertical<<G4endl;

  G4cout<<leftDivider;
  for(int i=0; i<60; i++) G4cout<<thinLine;
  G4cout<<rightDivider<<G4endl;
  
  G4cout<<vertical<<"Helium-3 tube description: "<<left<<setw(33);
  if(he3Desc.size()!=0)G4cout<<he3Desc;
  else G4cout<<"Default";
  G4cout<<vertical<<G4endl;
  
  G4cout<<vertical<<"Graphite description: "<<left<<setw(38);
  if(graphiteDesc.size()!=0)G4cout<<graphiteDesc;
  else G4cout<<"Default";
  G4cout<<vertical<<G4endl;

  G4cout<<vertical<<"Miscellaneous objects: "<<left<<setw(37);
  if(miscFile.size()!=0)G4cout<<miscFile;
  else G4cout<<"None";
  G4cout<<vertical<<G4endl;


  G4cout<<vertical<<"Saving all events: "<<left<<setw(41);
  if(saveAll) G4cout<<"yes";
  else G4cout<<"no";
  G4cout<<vertical<<G4endl;

  G4cout<<vertical<<"Saving geometry: "<<left<<setw(43);
  if(saveGeo) G4cout<<"yes";
  else G4cout<<"no";
  G4cout<<vertical<<G4endl;

  G4cout<<vertical<<"Verbose mode: "<<left<<setw(46);
  if(verbose) G4cout<<"yes";
  else G4cout<<"no";
  G4cout<<vertical<<G4endl;


  if(gui){ 
    G4cout<<vertical<<"Entering interactive mode"<<left<<setw(35);
    if(vis) G4cout<<" with visualization";
    else G4cout<<" ";
    G4cout<<vertical<<G4endl;
	      

  }else{

    G4cout<<vertical<<"Macro file: "<<left<<setw(48);
    if(fileName.size()!=0) G4cout<<fileName;
    else G4cout<<"None";
    G4cout<<vertical<<G4endl;
    

    if(fileName.size()==0){
      G4cout<<vertical<<"Number of events: "<<left<<setw(42);
      if(nevents.size()!=0) G4cout<<nevents.substr(12);
      else G4cout<<"Not specified";
      G4cout<<vertical<<G4endl;
      
      G4cout<<vertical<<"Output file: "<<left<<setw(47);
      if(outfile.size()!=0) G4cout<<outfile.substr(22);
      else G4cout<<"Not specified";
      G4cout<<vertical<<G4endl;
      
      G4cout<<vertical<<"Seeds: "<<left<<setw(53);
      if(seed.size()!=0) G4cout<<seed.substr(17);
      else G4cout<<"Not specified";
      G4cout<<vertical<<G4endl;
    }

  }
    
  G4cout<<bottomLeft;
  for(int i=0; i<60; i++) G4cout<<line;
  G4cout<<bottomRight<<G4endl;




  //make sure environment variables are set
  char *checkEnv;
  checkEnv = getenv("G4NEUTRONHPDATA");
  if(checkEnv==NULL){
    cout<<"Need to source geant4.10.02-install/bin/geant4.sh before running\n";
    return 0;
  }

  std::streambuf *coutbuf;
  if(!verbose){
    stringstream ss;
    coutbuf = G4cout.rdbuf(); //save old buf
    G4cout.rdbuf(ss.rdbuf());
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
  DetectorConstruction* detConstruction = new DetectorConstruction(he3Desc, miscFile, graphiteDesc, verbose, saveGeo);  
  runManager->SetUserInitialization(detConstruction);
    
  
  // set user action classes
  runManager->SetUserInitialization(new ActionInitialization(detConstruction, saveAll));

  //Initialize G4 kernel
  runManager->Initialize();

  G4HadronicProcessStore::Instance()->SetVerbose(verbose);
    
  // get the pointer to the User Interface manager 
  G4UImanager* UI = G4UImanager::GetUIpointer();  

  if(!verbose) G4cout.rdbuf(coutbuf);

  
  if(fileName.size()!=0){ //user specified script
     G4String command = "/control/execute ";
     UI->ApplyCommand(command+fileName);  

  } else if(!gui){  //user specified seed, number of events, and output file

      if(outfile.size()==0){
	cout<<bold2<<red2<<"No output file specified! quitting."<<noFormat2<<endl;
	help();
	return 0;
      }if(nevents.size()==0){
	cout<<bold2<<red2<<"Number of events not specified! quitting."<<noFormat2<<endl;
	help();
	return 0;
      }

      int n = std::count(seed.begin(), seed.end(), ' ');
      


      if(n<2) cout<<bold2<<"Warning: Wrong number of seeds."<<noFormat2<<endl;
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
      help();

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


