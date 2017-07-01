//
/// \file electromagnetic/PileRoomSim/include/RunAction.hh
/// \brief Definition of the RunAction class
//
// $Id: RunAction.hh 74270 2013-10-02 14:46:02Z gcosmo $
//

#ifndef RunAction_h
#define RunAction_h 1

#include "Analysis.hh"

#include "G4UserRunAction.hh"
#include "globals.hh"



class G4Run;


class RunAction : public G4UserRunAction
{
  public:
    RunAction();
   ~RunAction();

  public:
    virtual void BeginOfRunAction(const G4Run*);
    virtual void   EndOfRunAction(const G4Run*);
    inline G4double DEDX_Transform(G4double in1,G4double in2);
};

    inline G4double RunAction::DEDX_Transform(G4double in1,G4double in2){
      return pow(in1/in2,1.0/15.5);
      //return log(1+(in1/in2));
    }

#endif

