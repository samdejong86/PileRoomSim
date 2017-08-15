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
  inline void crossedSphere(int n);

  void setdataNtuple(int n) {dataNtuple=n;}
  void setGeoNtuple(int n) {geoNtuple=n;}

  //get methods for vectors, for creating vector branches in RunAction.hh
  std::vector<double>& getEDEPvec() { return edepInHe3;}
  std::vector<int>& getPIDvec() { return PIDinHe3;}
  std::vector<int>& getNeutronHits() {return neutronHitVec;}

  std::vector<double>& getDiffusionRadii() {return diffusionRadii;}
  std::vector<double>& getCrossedSphere() {return nCrossedRadii;}
  
  int getnNeutrons() {return nNeutrons;}
  int getnGraphite() {return GraphiteTotal;}
  void getTime(int n, double &meanTime, double &stDevTime){
    meanTime=eventDuration;
    stDevTime=sqrt(M/(n-1));
  }

  G4double input_energy;
  
private:

  int dataNtuple;
  int geoNtuple;

  G4double ePostGraphite;
  
  //number of channels and objects
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
  std::vector<double> diffusionRadii;
  std::vector<double> nCrossedRadii;
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
  int GraphiteTotal;

  struct timeval timeMark;

  double startTime;
  double endTime;
  double eventDuration;
  double M;
  double timeStdev;

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

inline void EventAction::crossedSphere(int n){
  nCrossedRadii[n] += 1/(4*3.14159*pow(diffusionRadii[n],2));
}


#endif

    
