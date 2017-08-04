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

#include "xmlParser.h"



class DetectorConstruction;
class G4Run;


class RunAction : public G4UserRunAction
{
  public:
  RunAction(DetectorConstruction* detConstruction);
   ~RunAction();

  public:
    virtual void BeginOfRunAction(const G4Run*);
    virtual void   EndOfRunAction(const G4Run*);
    inline G4double DEDX_Transform(G4double in1,G4double in2);

private:
  DetectorConstruction* fDetConstruction; 
  struct timeval timeMark;
  double runStart;
  double runEnd;
  int dataNtuple;
  int GeoNtuple;

  std::vector<XmlParser> tubes;
  std::vector<XmlParser> miscObjects;
  std::vector<std::string> HE3tags;
  std::vector<std::vector<double>> HE3vals;

  std::vector<std::string> MISCtags;
  std::vector<std::vector<double>> MISCvals;

  XmlParser gParam;
  std::vector<std::string> GRAtags;
  std::vector<std::vector<double>> GRAvals;

  XmlParser rParam;
  std::vector<std::string> ROOMtags;
  std::vector<std::vector<double>> ROOMvals;

  bool saveGeo;
  bool verbose;

  void CreateGeoNtuple(G4AnalysisManager* analysisManager);
  void FillGeoNtuple(G4AnalysisManager* analysisManager, const G4Run* aRun);
  void report(const G4Run* run);

};

    inline G4double RunAction::DEDX_Transform(G4double in1,G4double in2){
      return pow(in1/in2,1.0/15.5);
      //return log(1+(in1/in2));
    }

#endif

