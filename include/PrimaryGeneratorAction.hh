//
/// \file electromagnetic/PileRoomSim/include/PrimaryGeneratorAction.hh
/// \brief Definition of the PrimaryGeneratorAction class
//
//
// $Id: PrimaryGeneratorAction.hh 66241 2012-12-13 18:34:42Z gunter $
//
// 


#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4GeneralParticleSource.hh"

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;
class G4GeneralParticleSource;
class EventAction;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction(EventAction*);    
   ~PrimaryGeneratorAction();
    virtual void GeneratePrimaries(G4Event*);

 private:
   //G4ParticleGun*  fParticleGun;    
   G4double Energy_in;
   EventAction* fEventAction; 
   G4GeneralParticleSource*  fParticleGun;  
 
      
};

#endif


