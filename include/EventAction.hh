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
#include <iomanip>      // std::setprecision

class DetectorConstruction;

class EventAction : public G4UserEventAction
{
public:
  EventAction(const DetectorConstruction* detectorConstruction);
  ~EventAction();
  
public:
  virtual void BeginOfEventAction(const G4Event*);
  virtual void   EndOfEventAction(const G4Event*);
  virtual void Initialize();

  inline void He3Hit(int PDG, double edep, int ch);
  inline void LeftGraphite(double eKin);
  inline void leftWall();

  std::vector<double>& getEDEPvec() { return edepInHe3;}
  std::vector<double>& getTotalEdep() { return TotalEnergyDeposit;}
  std::vector<int>& getPIDvec() { return PIDinHe3;}
  std::vector<int>& getHitvec() { return he3Ch;}
  std::vector<int>& getChannelVec() {return channelVec;}
  std::vector<int>& getNeutronHits() {return neutronHitVec;}
  std::vector<double>& getTubeX() {return tubeX;}
  std::vector<double>& getTubeY() {return tubeY;}
  std::vector<double>& getTubeZ() {return tubeZ;}

  G4double input_energy;
  int nevent = 0;
  
private:
  G4double ePostGraphite;
  
  
  int nChannels;

  bool neutronHit;
  bool leftGraphite;
  bool leftConcrete;

  std::vector<int> neutronHitVec;
  std::vector<bool> isNeutronHitVec;
  std::vector<double> edepInHe3;
  std::vector<int> channelVec;
  std::vector<int> PIDinHe3;
  std::vector<int> he3Ch;
  std::vector<double> TotalEnergyDeposit;   // Energy deposited 
  std::vector<double> tubeX;
  std::vector<double> tubeY;
  std::vector<double> tubeZ;
  std::vector<G4ThreeVector> tubeLoc;
  

  const DetectorConstruction* fDetConstruction;
};

// inline functions

inline void EventAction::He3Hit(int PDG, double edep, int ch){
  neutronHit=true;
  isNeutronHitVec[ch] = true;
  PIDinHe3.push_back(PDG);
  edepInHe3.push_back(edep/eV);
  TotalEnergyDeposit[ch] += edep/MeV;
  he3Ch[ch]++;
  channelVec.push_back(ch);

  tubeX.push_back(tubeLoc[ch].x());
  tubeY.push_back(tubeLoc[ch].y());
  tubeZ.push_back(tubeLoc[ch].z());

  /*
  std::cout<<tupePos.x()<<std::endl;
  tubeX[ch] = tupePos.x();
  tubeY[ch] = tupePos.y();
  tubeZ[ch] = tupePos.z();
  */
  //G4cout<<"Hit by "<<PDG<<" in channel "<<ch<<"\n";
}

inline void EventAction::leftWall(){
  leftConcrete=true;
}

inline void EventAction::LeftGraphite(double eKin){
  if(!leftGraphite){
    ePostGraphite=eKin;
    leftGraphite=true;
  }
}



//
#endif

    
