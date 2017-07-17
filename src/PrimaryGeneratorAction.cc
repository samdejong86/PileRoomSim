
#include "PrimaryGeneratorAction.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "Analysis.hh"
#include "EventAction.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction(EventAction* eventAction)
: G4VUserPrimaryGeneratorAction(),
  fEventAction(eventAction),
  fParticleGun(0)
{
 fParticleGun = new G4GeneralParticleSource();
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{  
fParticleGun->GeneratePrimaryVertex(anEvent);
//G4cout << fParticleGun->GetParticleEnergy()/MeV << G4endl;
//G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
//analysisManager->FillH1(2, fParticleGun->GetParticleEnergy()/eV);
fEventAction->input_energy=fParticleGun->GetParticleEnergy();
}
