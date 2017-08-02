#include <sstream>

#include "DetectorConstruction.hh"
#include "G4NistManager.hh"

#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>

#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/JamesRandom.h"

#include "G4Material.hh"
#include "G4Trap.hh"
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"

#include "Analysis.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4EmCalculator.hh"
#include "G4UnitsTable.hh"
#include "G4AssemblyVolume.hh"
#include <G4Electron.hh>

using namespace std;
#include "xmlParser.h"

DetectorConstruction::DetectorConstruction()
  :G4VUserDetectorConstruction(),
   he3filename(""),
   miscfilename(""),
   graphitefilename(""),
   ver(0),
   saveGeo(false)
{  
  SetParams();
}

DetectorConstruction::DetectorConstruction(G4String he3Desc, G4String miscFile, G4String graphiteFile, bool verbose, bool saveG)
  :G4VUserDetectorConstruction(),
   he3filename(he3Desc),
   miscfilename(miscFile),
   graphitefilename(graphiteFile),
   ver(verbose),
   saveGeo(saveG)
{  
  SetParams();
}


DetectorConstruction::~DetectorConstruction()
{
}


//construct the environment
G4VPhysicalVolume* DetectorConstruction::Construct()
{
     
  G4VisAttributes * worldatt = new G4VisAttributes(G4Colour(0.,0.,0.));  


  //the world is a big box of air
  G4Box* solid_World = new G4Box("world_All",  Box_Length,Box_Length,Box_Length);
  logicWorld = new G4LogicalVolume(solid_World,world_mat,  "world_Vol");    
  logicWorld->SetVisAttributes(worldatt);
  physiWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "world_phys", 0, false, 0);

  
  //build graphite and room
  BuildGraphite(G4ThreeVector(gParam.getValue("x_pos")*cm, gParam.getValue("y_pos")*cm, gParam.getValue("z_pos")*cm));  
  physRoom = BuildRoom(G4ThreeVector(rParam.getValue("x_pos")*cm, rParam.getValue("y_pos")*cm, rParam.getValue("z_pos")*cm));
  
  
  //build helium-3 tubes
  for(int i=0; i<(int)tubeParams.size(); i++){
    
    //rotate the tubes as specified in description
    G4RotationMatrix* rot_he3tube = new G4RotationMatrix();
    rot_he3tube->rotateX(tubeParams[i].getValue("AngleX")*deg);
    rot_he3tube->rotateY(tubeParams[i].getValue("AngleY")*deg);
    rot_he3tube->rotateZ(tubeParams[i].getValue("AngleZ")*deg);
    
    //set tube positions
    He3TUBEpos[i] = (G4ThreeVector(tubeParams[i].getValue("x_pos")*cm,
				   tubeParams[i].getValue("y_pos")*cm,
				   tubeParams[i].getValue("z_pos")*cm));

    
    phys_HE3[i] = BuildHe3Tube(He3TUBEpos[i], rot_he3tube, i);
    
  }
  
  //build other objects
  for(int i=0; i<(int)miscParams.size(); i++){
    double addX=0;
    double addY=0;
    double addZ=0;
    
    //if this object is supposed to be tied to a tube, add the tube position to the object's position
    if(miscParams[i].getValue("TieToHe3Tubes")){
      int ch = miscParams[i].getValue("he3TubeChannel");
      addX=tubeParams[ch].getValue("x_pos")*cm;
      addY=tubeParams[ch].getValue("y_pos")*cm;
      addZ=tubeParams[ch].getValue("z_pos")*cm;
    }
        
    G4ThreeVector loc(miscParams[i].getValue("x_pos")*cm+addX, 
		      miscParams[i].getValue("y_pos")*cm+addY,
		      miscParams[i].getValue("z_pos")*cm+addZ);    

    phys_misc[i] = BuildMiscObjects(loc, i);    
  } 



  
   return physiWorld;
}

//-----------------------------------------------------------------------------------------
//This method constructs the helium-3 tubes.
G4VPhysicalVolume* DetectorConstruction::BuildHe3Tube(G4ThreeVector tubeLoc, G4RotationMatrix* rotation, int ch) {

  EndcapinnerRadius=0;
  GasinnerRadius=0;

  //get the geometry variables 
  double tube_outerRadius = tubeParams[ch].getValue("tube_outerRadius")*cm;
  double tube_innerRadius = tubeParams[ch].getValue("tube_innerRadius")*cm;
  double tube_hz = tubeParams[ch].getValue("tube_hz")*cm;
  double endcap_hz = tubeParams[ch].getValue("endcap_hz")*cm;
  double sensitiveVolOffset = tubeParams[ch].getValue("sensitive_volume_offset")*cm;
  double gas_outerRadius = tubeParams[ch].getValue("gas_outerRadius")*cm;
  double gas_hz = tubeParams[ch].getValue("gas_hz")*cm;
  double stem_radius = tubeParams[ch].getValue("stem_radius")*cm;
  double stem_hz = tubeParams[ch].getValue("stem_hz")*cm;



  //create he3tube volume. This logical volume contains the helium-3 tube components
  G4Tubs* solid_he3tube = new G4Tubs("solid_he3tube", 0, tube_outerRadius, tube_hz+2*endcap_hz, 0*deg,360*deg);
  G4LogicalVolume* logic_he3Tube = new G4LogicalVolume(solid_he3tube,  world_mat, "he3_vol");

   
  //create he3 inactive gas
  G4Tubs* s_iHe3Gas = new G4Tubs("s_iHe3Gas", GasinnerRadius,  tube_innerRadius, tube_hz, 0*deg, 360*deg);
  
  //logical volume for inactive gas
  G4LogicalVolume* l_iHe3Gas = new G4LogicalVolume(s_iHe3Gas, Helium3, "l_iHe3Gas");   
  new G4PVPlacement(0, G4ThreeVector(), l_iHe3Gas, "p_iHe3Gas", logic_he3Tube, false, 1);



   //create he3 active gas: a sub volume of the inactive gas
   G4Tubs* s_He3Gas = new G4Tubs("s_He3Gas", GasinnerRadius, gas_outerRadius, gas_hz, 0*deg, 360*deg);
   
   //logical volume for active gas
   G4LogicalVolume* l_He3Gas = new G4LogicalVolume(s_He3Gas, Helium3, "l_He3Gas", 0);   
   stringstream ss;
   ss<<ch;   
   //give it a name that contains it's channel number
   G4VPhysicalVolume* HE3ActiveVolume = new G4PVPlacement(0, G4ThreeVector(0, 0, sensitiveVolOffset), l_He3Gas, "p_He3Gas"+ss.str(), l_iHe3Gas, false, 0);
   ss.str("");


   //create stainless steel case
   G4Tubs* solid_Case = new G4Tubs("solid_he3tube", tube_innerRadius, tube_outerRadius, tube_hz, 0*deg,360*deg);
   G4LogicalVolume* logic_Case = new G4LogicalVolume(solid_Case,mat_steel,"vol_Case");   
   new G4PVPlacement(0,G4ThreeVector(), logic_Case,"vol_Case",logic_he3Tube,false,0);


   //create endcaps
   G4Tubs* endCap = new G4Tubs("solid_caseEndcap",  EndcapinnerRadius,   tube_outerRadius,    endcap_hz,   0*deg, 360*deg);
   G4LogicalVolume* logic_endcap = new G4LogicalVolume(endCap,mat_steel , "logic_endcap");

   //endcap positions
   G4ThreeVector endCap1 = G4ThreeVector(0, 0, -tube_hz-endcap_hz);   
   G4ThreeVector endCap2 = G4ThreeVector(0, 0,  tube_hz+endcap_hz);

   new G4PVPlacement(0, endCap1, logic_endcap, "vol_endcap1", logic_he3Tube,false,0);
   new G4PVPlacement(0, endCap2, logic_endcap, "vol_endcap2", logic_he3Tube,false,0);


   



   
   //colour for the inactive he3
   G4VisAttributes * he3_inactive_att = new G4VisAttributes(G4Colour(210./256,  38./256,  48./256));  
   he3_inactive_att->SetForceWireframe(true);
   he3_inactive_att->SetForceSolid(true);
   l_He3Gas->SetVisAttributes(he3_inactive_att);
   
   //colour for the active he3
   G4VisAttributes * he3att2 = new G4VisAttributes(G4Colour(60./256, 125./256, 196./256));  
   he3att2->SetForceWireframe(true);
   he3att2->SetForceSolid(true);
   l_iHe3Gas->SetVisAttributes(he3att2); 

   //get colour setting from xml file
   string colour =  tubeParams[ch].getStringValue("Colour");
   colour.erase(std::remove(colour.begin(), colour.end(), '#'), colour.end());
   unsigned int r, gg, b;
   sscanf(colour.c_str(), "%02x%02x%02x", &r, &gg, &b);

   //colour for the stainless steel case
   G4VisAttributes * tubeatt = new G4VisAttributes(G4Colour(r/255.,gg/255.,b/255.));  
   tubeatt->SetForceWireframe(true);
   tubeatt->SetForceSolid(true);
   logic_Case->SetVisAttributes(tubeatt);
   logic_endcap->SetVisAttributes(tubeatt);

   if(stem_hz!=0){
     G4Tubs* stem = new G4Tubs("solid_stem", stem_radius-0.1*cm, stem_radius, stem_hz, 0*deg, 360*deg);
     G4LogicalVolume* logic_stem = new G4LogicalVolume(stem,mat_steel , "logic_stem");
     
     G4ThreeVector stemLoc = G4ThreeVector(0,0, -tube_hz-2*endcap_hz-stem_hz);
     new G4PVPlacement(0, stemLoc, logic_stem, "vol_stem", logic_he3Tube,false,0);
     logic_stem->SetVisAttributes(tubeatt);
   }
    
   //build the tube at the specified location
   new G4PVPlacement(rotation, tubeLoc,  logic_he3Tube, "l_He3tube", logicWorld, false, 0);

   //return the active area
   return HE3ActiveVolume;

}

//-----------------------------------------------------------------------------------------
//build the graphite cube out of a bunch of rods
void DetectorConstruction::BuildGraphite(G4ThreeVector pileLoc){
  
  //get geometry variables
  double rodLength = gParam.getValue("rodLength")*cm;
  double rodWidth = gParam.getValue("rodWidth")*cm;
  int nRodsY = gParam.getValue("nRodsY");
  int nRodsZ = gParam.getValue("nRodsZ");
  
  double AngleX=gParam.getValue("AngleX")*deg;
  double AngleY=gParam.getValue("AngleY")*deg;
  double AngleZ=gParam.getValue("AngleZ")*deg;

  //set visulization colour
  string colour =  gParam.getStringValue("Colour");
  colour.erase(std::remove(colour.begin(), colour.end(), '#'), colour.end());
  unsigned int r, gg, b;
  sscanf(colour.c_str(), "%02x%02x%02x", &r, &gg, &b);
  G4VisAttributes * graph3att = new G4VisAttributes(G4Colour(r/255.,gg/255.,b/255.)); 
  graph3att->SetForceWireframe(true);
  graph3att->SetForceSolid(true);

  
  // Define one layer as one assembly volume
  G4AssemblyVolume* assemblyDetector = new G4AssemblyVolume();
  
  // Rotation and translation of a plate inside the assembly
  G4RotationMatrix Ra;
  G4ThreeVector Ta;
  G4Transform3D Tr;

  //random number generator for rod dimentions
  CLHEP::HepJamesRandom theRandEngine(42);
  double mmm=0;
  double ss=gParam.getValue("SizeSigma");  
  CLHEP::RandGauss gsn(theRandEngine);

  //create one layer of rods
  for(int i=0; i<nRodsY; i++){

    //add a small perturbation to the rod sizes to mimic actual cube
    double lengthAdd=fabs(gsn.shoot(mmm,ss))*cm;
    double widthAdd1=fabs(gsn.shoot(mmm,ss))*cm;
    double widthAdd2=fabs(gsn.shoot(mmm,ss))*cm;

    // Define a single rod
    G4Box* RodBox = new G4Box( "RodBox", rodLength-lengthAdd, rodWidth-widthAdd1, rodWidth-widthAdd2);
    G4LogicalVolume* rodLV = new G4LogicalVolume( RodBox, pile_mat, "RodLV", 0, 0, 0 );

    
    lengthAdd=fabs(gsn.shoot(mmm,ss))*cm;
    widthAdd1=fabs(gsn.shoot(mmm,ss))*cm;
    widthAdd2=fabs(gsn.shoot(mmm,ss))*cm;

    G4Box* RodBox2 = new G4Box( "RodBox", rodLength-lengthAdd, rodWidth-widthAdd1, rodWidth-widthAdd2);
    G4LogicalVolume* rodLV2 = new G4LogicalVolume( RodBox2, pile_mat, "RodLV", 0, 0, 0 );

    //apply visulization settings
    rodLV->SetVisAttributes(graph3att);
    rodLV2->SetVisAttributes(graph3att);

  
    Ta.setX(0);
    Ta.setY(2*rodWidth*i);
    Ta.setZ(0);
    
    Tr = G4Transform3D(Ra,Ta);
    assemblyDetector->AddPlacedVolume( rodLV, Tr );
    
    Ta.setX(2*rodLength);
    Ta.setY(2*rodWidth*i);
    Ta.setZ(0);
    
    Tr = G4Transform3D(Ra,Ta);
    assemblyDetector->AddPlacedVolume( rodLV2, Tr );
    
   }

   // Now instantiate the layers
  for( int i = 0; i < nRodsZ; i++ )
   {
     G4ThreeVector Rr;
     // Translation of the assembly inside the world
     G4RotationMatrix Rm;
     double X=0;
     double Y=0;     

     //rotate every other layer
     if(i%2==0){       
       Y=rodLength-rodWidth;
       X=3*rodLength-rodWidth;
       Rm.rotateZ(90*deg);
     }

     //apply rotation
     Rm.rotateX(AngleX);
     Rm.rotateY(AngleY);
     Rm.rotateZ(AngleZ);

     Rr.setZ(pileLoc.z()+i*2*rodWidth-2*rodLength+rodWidth);
     Rr.setY(pileLoc.y()+Y-2*rodLength+rodWidth);
     Rr.setX(pileLoc.x()+X-rodLength);

     Tr = G4Transform3D(Rm,Rr);
     //add the layers to the world
     assemblyDetector->MakeImprint( logicWorld, Tr );
   }
}

//-----------------------------------------------------------------------------------------
//build the room 
G4VPhysicalVolume* DetectorConstruction::BuildRoom(G4ThreeVector roomLoc){

  double AngleX=rParam.getValue("AngleX")*deg;
  double AngleY=rParam.getValue("AngleY")*deg;
  double AngleZ=rParam.getValue("AngleZ")*deg;
  
  //the outside of the room
  G4Box* solid_RoomOutside = new G4Box("solid_room_out", (rParam.getValue("roomXLength")+2*rParam.getValue("concThick"))*cm, (rParam.getValue("roomYLength")+2*rParam.getValue("concThick"))*cm, (rParam.getValue("roomZLength")+2*rParam.getValue("concThick"))*cm);
  
  //the inside of the room
  G4Box* solid_RoomInSide = new G4Box("solid_room_in", rParam.getValue("roomXLength")*cm, rParam.getValue("roomYLength")*cm, rParam.getValue("roomZLength")*cm);
  
  //subtract the inside from the outside to get a empty room
  G4SubtractionSolid* solid_walls = new G4SubtractionSolid("solid_room", solid_RoomOutside, solid_RoomInSide);
  
 
  //set colour of the room
  string color =  rParam.getStringValue("Colour");
  color.erase(std::remove(color.begin(), color.end(), '#'), color.end());
  unsigned int r, gg, b;
  sscanf(color.c_str(), "%02x%02x%02x", &r, &gg, &b);
  G4VisAttributes * conc3att = new G4VisAttributes(G4Colour(r/255.,gg/255.,b/255.));  
  conc3att->SetForceWireframe(true);
  
  //define the room's logical volume
  G4LogicalVolume* logic_Room = new G4LogicalVolume(solid_walls,room_mat,"room_Vol");
  logic_Room->SetVisAttributes(conc3att);

  //apply rotation
  G4RotationMatrix *RoomRot = new G4RotationMatrix();
  RoomRot->rotateX(AngleX);
  RoomRot->rotateY(AngleY);
  RoomRot->rotateZ(AngleZ);
  
  //build room at specified location
  G4VPhysicalVolume *Room = new G4PVPlacement(RoomRot,roomLoc, logic_Room,"phys_Room",logicWorld,false,0);
  return Room;
   
}

//-----------------------------------------------------------------------------------------
//build other objects defined in miscfilename
G4VPhysicalVolume* DetectorConstruction::BuildMiscObjects(G4ThreeVector objLoc, int num){

  //rotation of the object
  G4RotationMatrix* rotObj = new G4RotationMatrix();
  rotObj->rotateX(miscParams[num].getValue("AngleX")*deg);
  rotObj->rotateY(miscParams[num].getValue("AngleY")*deg);
  rotObj->rotateZ(miscParams[num].getValue("AngleZ")*deg);

  //material of the object
  G4Material *obj_mat = nist->FindOrBuildMaterial(miscParams[num].getStringValue("Material")); 

  //name of object
  G4String name = miscParams[num].getStringValue("Name");

  //parent class of G4Box and G4Tubs
  G4CSGSolid *solidObject;
  
  if(miscParams[num].getStringValue("Shape")=="Box"){         //use G4Box constructior for a Box
    solidObject = new G4Box("solid_"+name, miscParams[num].getValue("xLength")*cm, miscParams[num].getValue("yLength")*cm, miscParams[num].getValue("zLength")*cm);

  }else if(miscParams[num].getStringValue("Shape")=="Tube"){  //use G4Tubs constructior for a Tube
    solidObject = new G4Tubs("solid_"+name, miscParams[num].getValue("innerRadius")*cm, miscParams[num].getValue("outerRadius")*cm, miscParams[num].getValue("hz")*cm, 0*deg, 360*deg);

  } else {   //print an error if the shape is not known, and ignore it.
    cout<<"Shape "<<miscParams[num].getStringValue("Shape")<<" not known, ignoring"<<endl;
    return NULL;
  }

  G4LogicalVolume *logicObject = new G4LogicalVolume(solidObject, obj_mat, "logical_"+name);

  //set colour of object
  string color =  miscParams[num].getStringValue("Colour");
  color.erase(std::remove(color.begin(), color.end(), '#'), color.end());
  unsigned int r, gg, b;
  sscanf(color.c_str(), "%02x%02x%02x", &r, &gg, &b);
  G4VisAttributes * miscAtt = new G4VisAttributes(G4Colour(r/255.,gg/255.,b/255.)); 
  miscAtt->SetForceWireframe(true);
  miscAtt->SetForceSolid(true);
  logicObject->SetVisAttributes(miscAtt);


  //build object at specified location
  G4VPhysicalVolume *object = new G4PVPlacement(rotObj, objLoc, logicObject, "phys_"+name, logicWorld, false, 0);
  return object;

}


//-----------------------------------------------------------------------------------------
//sets various parameters
void DetectorConstruction::SetParams(){

  //create an instance of the nist manager
  nist = G4NistManager::Instance();

  //size of the world
  Box_Length = 500*cm;

  //parse the value of $XMLLOCATION, an environment variable which has the location of Graphite.xml and Room.xml
  char* xmlLoc;
  string Location;
  xmlLoc = getenv("XMLLOCATION");
  if(xmlLoc==NULL) Location="../xml";   //if the environment variable is not set, set the xml location to the current directory
  else Location = string(xmlLoc);

  //if miscfilename is not empty, get the misc object  parameters
  if(miscfilename.size()!=0){
    miscParams = XmlParser::getVector(miscfilename, ver);    
    if(ver) G4cout<<"Creating volumes defined in "<<miscfilename<<G4endl;
    phys_misc.resize(miscParams.size());
  }
  
  
#ifdef PORTABLE
  Location=".";
#endif

  //if he3filename is empty, use default value
  if(he3filename.size()==0)
    he3filename = Location+"/HE3TUBE-default.xml";



  //state location of XML files
  if(ver) G4cout<<"XML files located in "<<Location<<"/"<<G4endl;
  

  if(graphitefilename.size()==0)
    graphitefilename = Location+"/Graphite.xml";


  //get Graphite and Room parameters
  gParam=XmlParser(graphitefilename, ver);
  rParam=XmlParser(Location+"/Room.xml", ver);

  cubeSize = 2*gParam.getValue("rodLength")*cm;

  //get helium 3 tube parameters
  if(ver) G4cout<<"Helium-3 tube parameters described in "<<he3filename<<"\n";
  tubeParams = XmlParser::getVector(he3filename, ver);
  if(ver) G4cout<<"There are "<<tubeParams.size()<<" tubes implemented\n";

  phys_HE3.resize(tubeParams.size());
  He3TUBEpos.resize(tubeParams.size());

  //define world and room materials
  world_mat = nist->FindOrBuildMaterial("G4_AIR");
  room_mat = nist->FindOrBuildMaterial(rParam.getStringValue("Material")); 
  
  makeGraphiteMaterial();


  if(ver) G4cout << *(G4Material::GetMaterialTable()) << endl;
  


  //He3 def from geant4 forum:
  //http://hypernews.slac.stanford.edu/HyperNews/geant4/get/materials/122.html?inline=-1
  G4int protons=2, neutrons=1, nucleons=protons+neutrons;
  G4double atomicMass = 3.016*g/mole;
  G4Isotope* he3 = new G4Isotope("He3", protons, nucleons, atomicMass);
  G4Element* He3 = new G4Element("Helium3", "He3", 1);
  He3->AddIsotope(he3, 100*perCent);
  G4double pressure = 4*bar;
  G4double temperature = 293*kelvin;
  G4double molar_constant = CLHEP::Avogadro*CLHEP::k_Boltzmann;  //from clhep
  G4double density = (atomicMass*pressure)/(temperature*molar_constant);
  Helium3 = new G4Material("Helium3", density, 1, kStateGas, temperature, pressure);
  Helium3->AddElement(He3, 100*perCent); 
  
  //build he3tube material
  mat_steel = nist->FindOrBuildMaterial(tubeParams[0].getStringValue("Material"));
  
  
}

void DetectorConstruction::makeGraphiteMaterial(){

  //define graphite material
  G4Material *baseMat  =  nist->BuildMaterialWithNewDensity("Graphite_1.63",gParam.getStringValue("Material"),gParam.getValue("density")*g/cm3  );  
  
  if(gParam.getValue("GraphitePerCent")==100){
    pile_mat=baseMat;
    if(ver) G4cout<<"using pure graphite\n";
    return;
  }
  
  
  double ImeanBase = baseMat->GetIonisation()->GetMeanExcitationEnergy();
  
  //get some boron
  G4Material* Boron= nist->FindOrBuildMaterial("G4_B");
  double ImeanBoron = Boron->GetIonisation()->GetMeanExcitationEnergy();


  double GraphiteFraction = gParam.getValue("GraphitePerCent")*perCent;
  double BoronFraction = 1-gParam.getValue("GraphitePerCent")*perCent;
  
 
  //define mixture
  pile_mat=new G4Material("DopedGraphite", gParam.getValue("density")*g/cm3, 2);
  pile_mat->AddMaterial(baseMat, GraphiteFraction);
  pile_mat->AddMaterial(Boron, BoronFraction);

  
  /*creating a mixture sets Imean for carbon to 81eV, but it should be 78eV (like graphite)
   *We need to manually combine Imean for graphite and Imean for boron based on the fraction
   *of each.
   *
   * See the nist database for the value of IMean for graphite:
   *  https://www.physics.nist.gov/PhysRefData/XrayMassCoef/tab1.html
   */
  double ImeanDoped = GraphiteFraction*ImeanBase+ImeanBoron*BoronFraction;
  pile_mat->GetIonisation()->SetMeanExcitationEnergy(ImeanDoped);


}
