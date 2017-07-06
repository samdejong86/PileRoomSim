#include <sstream>

#include "DetectorConstruction.hh"
#include "G4NistManager.hh"

#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>

#include "G4Material.hh"
#include "G4Trap.hh"
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"

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
  :G4VUserDetectorConstruction()
{  
  he3filename = "HE3TUBE-phase1.xml";
  SetParams();
}

DetectorConstruction::DetectorConstruction(G4String he3Desc)
  :G4VUserDetectorConstruction()
{
  if(he3Desc.size()!=0) he3filename = he3Desc;
  else he3filename = "HE3TUBE-phase1.xml";

  SetParams();
}


DetectorConstruction::~DetectorConstruction()
{
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
     
  G4Box* solid_World = new G4Box("world_All",  Box_Length,Box_Length,Box_Length);
  logicWorld = new G4LogicalVolume(solid_World,world_mat,  "world_Vol");    
  physiWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "world_phys", 0, false, 0);

  
  BuildGraphite(G4ThreeVector(gParam.getValue("x_pos")*cm, gParam.getValue("y_pos")*cm, gParam.getValue("z_pos")*cm));  
  physRoom = BuildRoom(G4ThreeVector(rParam.getValue("x_pos")*cm, rParam.getValue("y_pos")*cm, rParam.getValue("z_pos")*cm));
  
  
  
  for(int i=0; i<(int)tubeParams.size(); i++){
    //if(i>MaxTubes) break; //phys_HE3 has 10 entries. might implement something more sophisticated later....
    
    G4RotationMatrix* rot_he3tube = new G4RotationMatrix();

    rot_he3tube->rotateX(tubeParams[i].getValue("AngleX")*deg);
    rot_he3tube->rotateY(tubeParams[i].getValue("AngleY")*deg);
    rot_he3tube->rotateZ(tubeParams[i].getValue("AngleZ")*deg);
    

    He3TUBEpos[i] = (G4ThreeVector(tubeParams[i].getValue("x_pos")*cm,
				   tubeParams[i].getValue("y_pos")*cm,
				   tubeParams[i].getValue("z_pos")*cm));

    
    phys_HE3[i] = BuildHe3Tube(He3TUBEpos[i], rot_he3tube, i);
    
  }
  
  
   return physiWorld;
}

//-----------------------------------------------------------------------------------------

G4VPhysicalVolume* DetectorConstruction::BuildHe3Tube(G4ThreeVector tubeLoc, G4RotationMatrix* rotation, int ch) {
  
  double tube_outerRadius = tubeParams[ch].getValue("tube_outerRadius")*cm;
  double tube_innerRadius = tubeParams[ch].getValue("tube_innerRadius")*cm;
  double tube_hz = tubeParams[ch].getValue("tube_hz")*cm;
  double endcap_hz = tubeParams[ch].getValue("endcap_hz")*cm;
  double sensitiveVolOffset = tubeParams[ch].getValue("sensitiveVolOffset")*cm;

  double gas_outerRadius = tubeParams[ch].getValue("gas_outerRadius")*cm;
  double gas_hz = tubeParams[ch].getValue("gas_hz")*cm;

   //create he3tube volume. This logical volume contains the helium-3 tube components
   G4Tubs* solid_he3tube = new G4Tubs("solid_he3tube", 0, tube_outerRadius, tube_hz+2*endcap_hz, 0*deg,360*deg);
   G4LogicalVolume* logic_he3Tube = new G4LogicalVolume(solid_he3tube,  world_mat, "he3_vol");

   
   //create he3 inactive gas
   G4Tubs* s_iHe3Gas = new G4Tubs("s_iHe3Gas",
				  GasinnerRadius,
				  tube_innerRadius,
				  tube_hz,
				  0*deg, 360*deg);

   G4LogicalVolume* l_iHe3Gas = new G4LogicalVolume(s_iHe3Gas, Helium3, "l_iHe3Gas");   
   new G4PVPlacement(0, G4ThreeVector(), l_iHe3Gas, "p_iHe3Gas", logic_he3Tube, false, 1);



   //create he3 active gas: a sub volume of the inactive gas
   G4Tubs* s_He3Gas = new G4Tubs("s_He3Gas",
				 GasinnerRadius,
				 gas_outerRadius,
				 gas_hz,
				 0*deg, 360*deg);
   
   G4LogicalVolume* l_He3Gas = new G4LogicalVolume(s_He3Gas, Helium3, "l_He3Gas", 0);   
   stringstream ss;
   ss<<ch;   
   G4VPhysicalVolume* HE3ActiveVolume = new G4PVPlacement(0, G4ThreeVector(0, 0, sensitiveVolOffset), l_He3Gas, "p_He3Gas"+ss.str(), l_iHe3Gas, false, 0);
   ss.str("");


   //create stainless steel case
   G4Tubs* solid_Case = new G4Tubs("solid_he3tube", tube_innerRadius, tube_outerRadius, tube_hz, 0*deg,360*deg);
   G4LogicalVolume* logic_Case = new G4LogicalVolume(solid_Case,mat_steel,"vol_Case");   
   new G4PVPlacement(0,G4ThreeVector(), logic_Case,"vol_Case",logic_he3Tube,false,0);


   //create endcaps
   G4Tubs* endCap = new G4Tubs("solid_caseEndcap", 
			       EndcapinnerRadius, 
			       tube_outerRadius, 
			       endcap_hz, 
			       0*deg, 360*deg);
   G4LogicalVolume* logic_endcap = new G4LogicalVolume(endCap,mat_steel , "logic_endcap");

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
   
   //colour for the stainless steel case
   G4VisAttributes * tubeatt = new G4VisAttributes(G4Colour(196./255.,196./255.,196./255.));  
   tubeatt->SetForceWireframe(true);
   tubeatt->SetForceSolid(true);
   logic_Case->SetVisAttributes(tubeatt);
   logic_endcap->SetVisAttributes(tubeatt);
    
   //build the tube at the specified location
   new G4PVPlacement(rotation, tubeLoc,  logic_he3Tube, "l_He3tube", logicWorld, false, 0);
   
   //return the active area
   return HE3ActiveVolume;

}

//-----------------------------------------------------------------------------------------

void DetectorConstruction::BuildGraphite(G4ThreeVector pileLoc){
  
  double rodLength = gParam.getValue("rodLength")*cm;
  double rodWidth = gParam.getValue("rodWidth")*cm;
  int nRodsY = gParam.getValue("nRodsY");
  int nRodsZ = gParam.getValue("nRodsZ");
  
  double AngleX=gParam.getValue("AngleX")*deg;
  double AngleY=gParam.getValue("AngleY")*deg;
  double AngleZ=gParam.getValue("AngleZ")*deg;

   // Define a rod
  G4Box* RodBox = new G4Box( "RodBox", rodLength, rodWidth, rodWidth);
  G4LogicalVolume* rodLV = new G4LogicalVolume( RodBox, pile_mat, "RodLV", 0, 0, 0 );
  
  G4VisAttributes * graph3att = new G4VisAttributes(G4Colour(97./255.,97./255.,97./255.)); 
  graph3att->SetForceWireframe(true);
  graph3att->SetForceSolid(true);
  rodLV->SetVisAttributes(graph3att);
  
  
  // Define one layer as one assembly volume
  G4AssemblyVolume* assemblyDetector = new G4AssemblyVolume();
  
  // Rotation and translation of a plate inside the assembly
  G4RotationMatrix Ra;
  G4ThreeVector Ta;
  G4Transform3D Tr;
  
  for(int i=0; i<nRodsY; i++){
       Ta.setX(0);
       Ta.setY(2*rodWidth*i);
       Ta.setZ(0);
       
       Tr = G4Transform3D(Ra,Ta);
       assemblyDetector->AddPlacedVolume( rodLV, Tr );
       
       Ta.setX(2*rodLength);
       Ta.setY(2*rodWidth*i);
       Ta.setZ(0);
       
       Tr = G4Transform3D(Ra,Ta);
       assemblyDetector->AddPlacedVolume( rodLV, Tr );
     
   }

   // Now instantiate the layers
  for( int i = 0; i < nRodsZ; i++ )
   {
     G4ThreeVector Rr;
     // Translation of the assembly inside the world
     G4RotationMatrix Rm;
     double X=0;//pileLoc.x();
     double Y=0;//pileLoc.y();
     

     if(i%2==0){       
       Y=rodLength-rodWidth;
       X=3*rodLength-rodWidth;
       Rm.rotateZ(90*deg);
     }

     Rm.rotateX(AngleX);
     Rm.rotateY(AngleY);
     Rm.rotateZ(AngleZ);

     Rr.setZ(pileLoc.z()+i*2*rodWidth-2*rodLength);
     Rr.setY(pileLoc.y()+Y-2*rodLength);
     Rr.setX(pileLoc.x()+X-rodLength);

     Tr = G4Transform3D(Rm,Rr);
     assemblyDetector->MakeImprint( logicWorld, Tr );
   }
}

//-----------------------------------------------------------------------------------------

G4VPhysicalVolume* DetectorConstruction::BuildRoom(G4ThreeVector roomLoc){

  double AngleX=rParam.getValue("AngleX")*deg;
  double AngleY=rParam.getValue("AngleY")*deg;
  double AngleZ=rParam.getValue("AngleZ")*deg;
  
  G4Box* solid_RoomOutside = new G4Box("solid_room_out", (rParam.getValue("roomXLength")+2*rParam.getValue("concThick"))*cm, (rParam.getValue("roomYLength")+2*rParam.getValue("concThick"))*cm, (rParam.getValue("roomZLength")+2*rParam.getValue("concThick"))*cm);
  G4Box* solid_RoomInSide = new G4Box("solid_room_in", rParam.getValue("roomXLength")*cm, rParam.getValue("roomYLength")*cm, rParam.getValue("roomZLength")*cm);
  

  G4SubtractionSolid* solid_walls = new G4SubtractionSolid("solid_room", solid_RoomOutside, solid_RoomInSide);
  
  
  G4VisAttributes * conc3att = new G4VisAttributes(G4Colour(195./255.,195./255.,195./255.));  
  conc3att->SetForceWireframe(true);
  
 
  G4LogicalVolume* logic_Room = new G4LogicalVolume(solid_walls,room_mat,"room_Vol");
  logic_Room->SetVisAttributes(conc3att);

  G4RotationMatrix *RoomRot = new G4RotationMatrix();
  RoomRot->rotateX(AngleX);
  RoomRot->rotateY(AngleY);
  RoomRot->rotateZ(AngleZ);
  
  G4VPhysicalVolume *Room = new G4PVPlacement(RoomRot,roomLoc, logic_Room,"phys_Room",logicWorld,false,0);
  return Room;
   
}

//-----------------------------------------------------------------------------------------

void DetectorConstruction::SetParams(){

  nist = G4NistManager::Instance();

  char* xmlLoc;
  xmlLoc = getenv("XMLLOCATION");
  if(xmlLoc==NULL) xmlLoc=".";

  std::string str(xmlLoc);

  cout<<str<<endl;

  Box_Length = 500*cm;

  
  gParam=XmlParser(str+"/Graphite.xml");
  rParam=XmlParser(str+"/Room.xml");

  G4cout<<"Helium-3 tube parameters described in "<<he3filename<<"\n";

  getHe3Params(he3filename);

  G4cout<<"There are "<<tubeParams.size()<<" tubes implemented\n";

  world_mat = nist->FindOrBuildMaterial("G4_AIR");
  room_mat = nist->FindOrBuildMaterial(rParam.getStringValue("Material")); 


  G4Material *baseMat=  nist->BuildMaterialWithNewDensity("Graphite_1.63",gParam.getStringValue("Material"),gParam.getValue("density")*g/cm3  );


  
  pile_mat=new G4Material("impure", gParam.getValue("density")*g/cm3, 1);
  pile_mat->AddMaterial(baseMat, 100.*perCent);
  
  G4cout << *(G4Material::GetMaterialTable()) << endl;

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
  
  mat_steel = nist->FindOrBuildMaterial(tubeParams[0].getStringValue("Material"));
  
 
}

//-----------------------------------------------------------------------------------------

void DetectorConstruction::getHe3Params(string filename){


  ifstream in(filename);

  char str[600];

  XmlParser aHe3Params;

  XmlParser * currentHe3Params = 0;

  string activeTag = "";
  string text;

  string xmlStartTag = "<xml>";
  string xmlEndTag = "</xml>";

  while(in) {

    in.getline(str, 256);    
    string st = str;    
    st = aHe3Params.removeSpaces(st);  		
    if((st.compare(xmlStartTag))==0){
      G4cout<<"\nworking...\n";
    }
    else if((st.compare(xmlEndTag))==0){
      G4cout<<"done!\n";
    }
    else if ((st.compare(aHe3Params.startTag))==0) {
      currentHe3Params = new XmlParser();
    }
    else if ((st.compare(aHe3Params.endTag))==0) {
      if(currentHe3Params!=0){
	if(tubeParams.size()==MaxTubes){
	  G4cout<<"More than "<<MaxTubes<<" described in "<<filename<<". Skipping the rest\n";
	  break;
	}
	tubeParams.push_back(*currentHe3Params);
      }
    }
    else if (currentHe3Params->isXMLStartTag(st)) {
      activeTag = st;
    }
    else if (currentHe3Params->isXMLEndTag(st)) {
      if(currentHe3Params!=0){
	text = aHe3Params.removeSpaces(text);
	//cout<<activeTag<<"\t"<<text<<endl;
	currentHe3Params->setXMLField(activeTag, text);
	text = "";
      }
    }else if ((st.find("</"))!= std::string::npos){
      text="";
    }else
      text = text+ "\n" + st;
  }
  in.close();

  EndcapinnerRadius = 0.*cm;
  GasinnerRadius = 0.*cm;

}
