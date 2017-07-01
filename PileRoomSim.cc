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
//#include "PhysicsList.hh"
#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"
#include "G4OpticalPhysics.hh"
#include "FTFP_BERT_HP.hh"

#include "ActionInitialization.hh"
#include "SteppingVerbose.hh"
//#include "PhysicsList.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif


int main(int argc,char** argv) {

  G4String fileName = "";
  G4String he3Desc = "";

  for(int i=0; i<argc; i++){
    std::string param(argv[i]);
    if(param.find(".mac")!= std::string::npos) fileName = param;
    if(param.find(".xml")!= std::string::npos) he3Desc = param;

    //std::cout<<param<<std::endl;
    

  }

  std::cout<<fileName.size()<<std::endl;

  
  
  //choose the Random engine
  G4Random::setTheEngine(new CLHEP::RanecuEngine);
  
  // Construct the default run manager

  G4VSteppingVerbose::SetInstance(new SteppingVerbose);  
  G4RunManager* runManager = new G4RunManager;

  //G4VModularPhysicsList* physicsList = new FTFP_BERT_HP;
  //physicsList->RegisterPhysics(new G4OpticalPhysics(0));
  //runManager->SetUserInitialization(physicsList);
  G4VModularPhysicsList* physicsList = new FTFP_BERT_HP;

  runManager->SetUserInitialization(physicsList);

  DetectorConstruction* detConstruction = new DetectorConstruction(he3Desc);
  runManager->SetUserInitialization(detConstruction);
    
  //Setting Physics List

//  G4PhysListFactory factory;
//  G4VModularPhysicsList* phys = 0;
//  G4String physName = "";
//  
//  const std::vector<G4String> v = factory.AvailablePhysLists();
//    physName = "FTFP_BERT_HP";//argv[2];
////    physName = "QGSP_BIC_EMY";//argv[2];
//    G4cout << "Physics List is: "<< physName << G4endl;
//      // reference PhysicsList via its name
//        if("" != physName && factory.IsReferencePhysList(physName)) {
//          phys = factory.GetReferencePhysList(physName);
//         // phys->RegisterPhysics(new G4EmUserPhysics(1));
//         }else{
//            G4cout << "Physics List Specified Not ReferenceList!" << G4endl;
//            G4cout << "EXITING" << G4endl;
//            return 0;
//         }
  
  // set user action classes
  runManager->SetUserInitialization(new ActionInitialization(detConstruction));

  //Initialize G4 kernel
  runManager->Initialize();
    
  // get the pointer to the User Interface manager 
    G4UImanager* UI = G4UImanager::GetUIpointer();  

    
    //if (argc!=1)   // batch mode  
    if(fileName.size()!=0)
    { 
     G4String command = "/control/execute ";
     //UI->ApplyCommand(command+"neu.mac");  
     //UI->ApplyCommand(command+"vis.mac");  
     //G4String fileName = argv[1];
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


