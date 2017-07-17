//
/// \file electromagnetic/PileRoomSim/include/EventAction.hh
/// \brief Definition of the EventAction class
//
//
// $Id: EventAction.hh 75839 2013-11-06 17:27:26Z gcosmo $
//
// 

#include <iostream>

#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include <vector>
#include <iomanip>      

class DetectorConstruction;

class EventAction : public G4UserEventAction
{
public:
  EventAction(const DetectorConstruction* detectorConstruction, bool save);
  ~EventAction();
  
public:
  virtual void BeginOfEventAction(const G4Event*);
  virtual void   EndOfEventAction(const G4Event*);
  virtual void Initialize();

  inline void He3Hit(int PDG, double edep, int ch);
  inline void leftGraphite(double eKin);
  inline void leftWall();
  inline void leftObject(int n);

  //get methods for vectors, for creating vector branches in RunAction.hh
  std::vector<double>& getEDEPvec() { return edepInHe3;}
  std::vector<int>& getPIDvec() { return PIDinHe3;}
  std::vector<int>& getNeutronHits() {return neutronHitVec;}

  int getnNeutrons() {return nNeutrons;}

  G4double input_energy;
  
private:
  
  const std::string bold = "\033[1m";
  const std::string green = "\033[32m";
  const std::string underline = "\033[4m";
  const std::string blue = "\033[34m";
  const std::string red = "\033[31m";
  const std::string white = "\033[37m";
  const std::string noFormat = "\033[0m";

  G4double ePostGraphite;
  
  //numner of channels and objects
  int nChannels;
  int nobj;

  bool neutronHit;
  bool leftGrap;
  bool leftConcrete;
  std::vector<int> leftObj;

  std::vector<int> neutronHitVec;
  std::vector<bool> isNeutronHitVec;
  std::vector<double> edepInHe3;
  std::vector<int> PIDinHe3;
  double TotalEnergyDeposit;  
  double tubeX;
  double tubeY;
  double tubeZ;

  //location of tubes
  std::vector<G4ThreeVector> tubeLoc;
  
  //detector geometry
  const DetectorConstruction* fDetConstruction;

  //if true, all events are saved
  bool saveAll;

  //number of neutrons detected
  int nNeutrons;

};

// inline functions

inline void EventAction::He3Hit(int PDG, double edep, int ch){  //hit occurs in he3 tube
  
  if(!neutronHit){ 
    tubeX = tubeLoc[ch].x();
    tubeY = tubeLoc[ch].y();
    tubeZ = tubeLoc[ch].z();
  }
  neutronHit=true;               //there was a neutron hit
  isNeutronHitVec[ch] = true;    //there was a neutron hit in channel ch
  PIDinHe3.push_back(PDG);      
  edepInHe3.push_back(edep/eV);
  TotalEnergyDeposit += edep/MeV;
}

inline void EventAction::leftWall(){
  leftConcrete=true;
}

inline void EventAction::leftGraphite(double eKin){
  if(!leftGrap){
    ePostGraphite=eKin;
    leftGrap=true;
  }
}

inline void EventAction::leftObject(int n){
  leftObj[n] = 1;
}

#endif

    
