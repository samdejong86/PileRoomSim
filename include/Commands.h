#include <vector>
#include <string>


std::vector<std::string> UICommands(){
  
  std::vector<std::string> commands;
  
  commands.push_back("/gps/particle neutron");
  commands.push_back("/gps/pos/centre 0.0 0.0 0.0 cm");
  commands.push_back("/gps/ang/type iso");
  commands.push_back("/gps/pos/type Point");
  commands.push_back("/gps/ene/type Arb");
  commands.push_back("/gps/hist/type arb");
  
  commands.push_back("/gps/hist/point    0.11 0.905149");
  commands.push_back("/gps/hist/point    0.33 0.848238");
  commands.push_back("/gps/hist/point    0.54 0.761518");
  commands.push_back("/gps/hist/point    0.75 0.677507");
  commands.push_back("/gps/hist/point    0.97 0.579946");
  commands.push_back("/gps/hist/point    1.18 0.536585");
  commands.push_back("/gps/hist/point    1.40 0.474255");
  commands.push_back("/gps/hist/point    1.61 0.520325");
  commands.push_back("/gps/hist/point    1.82 0.604336");
  commands.push_back("/gps/hist/point    2.04 0.582656");
  commands.push_back("/gps/hist/point    2.25 0.609756");
  commands.push_back("/gps/hist/point    2.47 0.617886");
  commands.push_back("/gps/hist/point    2.68 0.799458");
  commands.push_back("/gps/hist/point    2.90 0.964770");
  commands.push_back("/gps/hist/point    3.11 1.000000");
  commands.push_back("/gps/hist/point    3.32 0.937669");
  commands.push_back("/gps/hist/point    3.54 0.831978");
  commands.push_back("/gps/hist/point    3.75 0.813008");
  commands.push_back("/gps/hist/point    3.97 0.728997");
  commands.push_back("/gps/hist/point    4.18 0.775068");
  commands.push_back("/gps/hist/point    4.39 0.861789");
  commands.push_back("/gps/hist/point    4.61 0.831978");
  commands.push_back("/gps/hist/point    4.82 0.902439");
  commands.push_back("/gps/hist/point    5.04 0.823848");
  commands.push_back("/gps/hist/point    5.25 0.742547");
  commands.push_back("/gps/hist/point    5.47 0.631436");
  commands.push_back("/gps/hist/point    5.68 0.558266");
  commands.push_back("/gps/hist/point    5.89 0.493225");
  commands.push_back("/gps/hist/point    6.11 0.479675");
  commands.push_back("/gps/hist/point    6.32 0.552846");
  commands.push_back("/gps/hist/point    6.54 0.495935");
  commands.push_back("/gps/hist/point    6.75 0.441734");
  commands.push_back("/gps/hist/point    6.96 0.455285");
  commands.push_back("/gps/hist/point    7.18 0.455285");
  commands.push_back("/gps/hist/point    7.39 0.509485");
  commands.push_back("/gps/hist/point    7.61 0.498645");
  commands.push_back("/gps/hist/point    7.82 0.457995");
  commands.push_back("/gps/hist/point    8.03 0.390244");
  commands.push_back("/gps/hist/point    8.25 0.262331");
  commands.push_back("/gps/hist/point    8.46 0.176694");
  commands.push_back("/gps/hist/point    8.68 0.115447");
  commands.push_back("/gps/hist/point    8.89 0.099458");
  commands.push_back("/gps/hist/point    9.11 0.103252");
  commands.push_back("/gps/hist/point    9.32 0.137127");
  commands.push_back("/gps/hist/point    9.53 0.169377");
  commands.push_back("/gps/hist/point    9.75 0.149593");
  commands.push_back("/gps/hist/point    9.96 0.126829");
  commands.push_back("/gps/hist/point    10.2 0.100271");
  commands.push_back("/gps/hist/point    10.4 0.075338");
  commands.push_back("/gps/hist/point    10.6 0.040921");
  commands.push_back("/gps/hist/point    10.8 0.009837");
  commands.push_back("/gps/hist/inter Spline");
  
  return commands;

}



std::vector<std::string> visCommands(){

  std::vector<std::string> commands;
  
  commands.push_back("/control/verbose 2");
  commands.push_back("/run/verbose 2");


  commands.push_back("/vis/open OGL 600x600-0+0");
  
  commands.push_back("/vis/viewer/set/autoRefresh false");
  commands.push_back("/vis/verbose errors");

  commands.push_back("/vis/drawVolume");
  
  commands.push_back("/vis/viewer/zoom 1.5");
  
  commands.push_back("/vis/scene/add/trajectories smooth");
  commands.push_back("/vis/modeling/trajectories/create/drawByCharge");
  commands.push_back("/vis/modeling/trajectories/drawByCharge-0/default/setDrawStepPts true");
  commands.push_back("/vis/modeling/trajectories/drawByCharge-0/default/setStepPtsSize 2");
  
  commands.push_back("/vis/scene/endOfEventAction accumulate");
  
  commands.push_back("/vis/viewer/set/autoRefresh true");
  commands.push_back("/vis/verbose warnings");
  
  commands.push_back("/vis/viewer/set/viewpointVector -1 -2 0.5");
  commands.push_back("/vis/viewer/set/upVector 0 0 1");
  commands.push_back("/vis/viewer/set/lightsVector 0 -0.5 1");

  return commands;


}


void help(){
      G4cout<<"usage: PileRoomSim  [-h] [-m MACRO] [-d HE3TUBEDESCRIPTION] [-g GRAPHITEDESCRIPTION]\n";
      G4cout<<"                    [-o MISCOBJECTS] [-n NEVENTS] [-s SEED1 SEED2] [-r ROOTFILE]\n";
      G4cout<<"                    [-novis] [-all] [-verbose] [-noGeo]\n";

      G4cout<<"\nSimulates helium-3 tube response to AmBe source in the centre of a graphite cube.\n";

      G4cout<<"\noptional arguments:"<<endl;
      G4cout<<"  -h,                      show this help message and exit\n";
      G4cout<<"  -m MACRO                 The macro to run. If this parameters is used, inputs\n";
      G4cout<<"                           from -n, -s, and -r are ignored\n";
      G4cout<<"  -d HE3TUBEDESCRIPTION    An xml file containing a description of the helium-3 \n";
      G4cout<<"                           tubes. If this parameter is unused, a default file is\n";
      G4cout<<"                           used.\n";
      G4cout<<"  -g GRAPHITEDESCRIPTION   An xml file containing a description of the graphite cube.\n";
      G4cout<<"                           If this parameter is unused, a default file is used\n";
      G4cout<<"  -o MISCOBJECTS           An xml file containing additional objects to be\n";
      G4cout<<"                           implemented.\n";
      G4cout<<"  -n NEVENTS               Number of events to run. If used, -r must be used\n";
      G4cout<<"  -s SEED1 SEED2           Seeds for random number generator.\n";
      G4cout<<"  -r ROOTFILE              Output rootfile (without .root extension). If used, -n\n";
      G4cout<<"                           must be used\n";
      G4cout<<"  -all                     If this parameter is used, all events are saved to the \n";
      G4cout<<"                           output ntuple. If not, only events containing a neutron\n"; 
      G4cout<<"                           hit in in a helium-3 tube are saved.\n";
      G4cout<<"  -novis                   If no other parameters specified, PileRoomSum runs in\n";
      G4cout<<"                           interactive mode with no visualization set.\n";
      G4cout<<"  -verbose                 Prints GEANT4 information\n";
      G4cout<<"  -noGeo                   Geometry is not saved to ntuple\n";

      G4cout<<"\nIf no arguments are specified, PileRoomSim runs in visualization mode"<<G4endl;

}



