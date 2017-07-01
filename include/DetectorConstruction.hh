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
    DetectorConstruction(G4String he3Desc);
   ~DetectorConstruction();
     
    virtual G4VPhysicalVolume* Construct();

  const G4VPhysicalVolume* Getphysicry(int ch) const;
  const G4VPhysicalVolume* GetphysiWorld() const; 
  const G4VPhysicalVolume* GetphysiGraphite() const;
  const G4VPhysicalVolume* GetphysiRoom() const;
  const G4ThreeVector GetHe3Posn(int ch) const;
  int GetNChannels() const;
  void SetParams();
  G4String he3filename;

private:

  XmlParser gParam;
  XmlParser rParam;
  std::vector<XmlParser> tubeParams;
  double EndcapinnerRadius;
  G4double GasinnerRadius;


  G4double Box_Length;

  G4VPhysicalVolume* BuildRoom(G4ThreeVector roomLoc);
  G4VPhysicalVolume* BuildHe3Tube(G4ThreeVector tubeLoc, G4RotationMatrix* rotation, int ch);
  void BuildGraphite(G4ThreeVector pileLoc);

  void getHe3Params(std::string filename);
  void getGraphiteParams(std::string filename);
  void getRoomParams(std::string filename);

  static const int nChannels=4;
  G4LogicalVolume* logicWorld;
  G4NistManager* nist;
  G4Material* world_mat;
  G4Material* pile_mat;
  G4Material* room_mat;
  G4Material* Helium3;
  G4Material* mat_steel;
  
  static const int MaxTubes=10;

  G4ThreeVector He3TUBEpos[MaxTubes];
  G4VPhysicalVolume* phys_HE3[MaxTubes];
  G4VPhysicalVolume* physiWorld;
  G4VPhysicalVolume* physRoom;
  protected:

private:

};

inline  int DetectorConstruction::GetNChannels() const {
  int a=tubeParams.size();//nChannels;
  return a;
}

inline const G4ThreeVector DetectorConstruction::GetHe3Posn(int ch) const{
  //if(ch>nChannels) return He3TUBEpos[0];
  return He3TUBEpos[ch];
}

// inline functions
inline const G4VPhysicalVolume* DetectorConstruction::Getphysicry(int ch) const { 
  //if(ch>nChannels) return phys_HE3[0];
  return  phys_HE3[ch]; 
}
inline const G4VPhysicalVolume* DetectorConstruction::GetphysiWorld() const { 
  return  physiWorld; 
}

inline const G4VPhysicalVolume* DetectorConstruction::GetphysiRoom() const { 
  return  physRoom; 
}



#endif

