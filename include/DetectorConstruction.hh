//
/// \file electromagnetic/PileRoomSim/include/DetectorConstruction.hh
/// \brief Definition of the DetectorConstruction class
//
//
// $Id: DetectorConstruction.hh 66241 2012-12-13 18:34:42Z gunter $
// 



#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include <vector>

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"

#include "xmlParser.h"



class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  
  DetectorConstruction();
  DetectorConstruction(G4String he3Desc, G4String miscFile);
  ~DetectorConstruction();
     
  virtual G4VPhysicalVolume* Construct();

  //get physical volumes
  const G4VPhysicalVolume* Getphysicry(int ch) const;
  const G4VPhysicalVolume* GetphysiMisc(int n) const;
  const G4VPhysicalVolume* GetphysiWorld() const; 
  const G4VPhysicalVolume* GetphysiGraphite() const;
  const G4VPhysicalVolume* GetphysiRoom() const;

  //get helium 3 tube positions
  const G4ThreeVector GetHe3Posn(int ch) const;

  double GetCubeSize() const;

  int GetNChannels() const;        //number of he3 channels
  int GetNMiscObjects() const;     //number of misc objects
    
  void SetParams(); //set parameters

private:

  //geometry parameters for the objects
  XmlParser gParam;
  XmlParser rParam;
  double cubeSize;

  std::vector<XmlParser> tubeParams;
  std::vector<XmlParser> miscParams;
  double EndcapinnerRadius;
  G4double GasinnerRadius;

  //filenames
  G4String he3filename;
  G4String miscfilename;

  //size of the world
  G4double Box_Length;

  //geometry creators
  G4VPhysicalVolume* BuildRoom(G4ThreeVector roomLoc);
  G4VPhysicalVolume* BuildHe3Tube(G4ThreeVector tubeLoc, G4RotationMatrix* rotation, int ch);
  void BuildGraphite(G4ThreeVector pileLoc);
  G4VPhysicalVolume* BuildMiscObjects(G4ThreeVector objLoc, int num);

  static const int nChannels=4;
  G4LogicalVolume* logicWorld;
  G4NistManager* nist;
  
  //materials
  G4Material* world_mat;
  G4Material* pile_mat;
  G4Material* room_mat;
  G4Material* Helium3;
  G4Material* mat_steel;
  
  //static const int MaxTubes=10;
  //static const int MaxObjects=10;

  vector<G4ThreeVector> He3TUBEpos;

  vector<G4VPhysicalVolume*> phys_HE3;
  vector<G4VPhysicalVolume*> phys_misc;
  G4VPhysicalVolume* physiWorld;
  G4VPhysicalVolume* physRoom;
  protected:

private:

};
// inline functions
inline  int DetectorConstruction::GetNChannels() const {
  int a=tubeParams.size();
  return a;
}

inline  int DetectorConstruction::GetNMiscObjects() const {
  int a=miscParams.size();
  return a;
}

inline const G4ThreeVector DetectorConstruction::GetHe3Posn(int ch) const{
  return He3TUBEpos[ch];
}

inline const G4VPhysicalVolume* DetectorConstruction::Getphysicry(int ch) const { 
  return  phys_HE3[ch]; 
}
inline const G4VPhysicalVolume* DetectorConstruction::GetphysiWorld() const { 
  return  physiWorld; 
}

inline const G4VPhysicalVolume* DetectorConstruction::GetphysiRoom() const { 
  return  physRoom; 
}

inline const G4VPhysicalVolume* DetectorConstruction::GetphysiMisc(int n) const {
  return phys_misc[n];
}

inline double DetectorConstruction::GetCubeSize() const {
  return cubeSize;
}


#endif

