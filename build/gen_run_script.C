#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream> 
using namespace std;

//string pp="Side_Pb";
//string pp="Front_Pb";
string pp="Cave";

void makeListMacFile(int instr){

stringstream ss;
ss << instr;
string str = ss.str();

  ofstream f;
  string tempstr =pp+"_input_"+str+string(".mac");
  f.open (tempstr.c_str());

  f << "/random/setSeeds " << 57+instr <<" "<<  83+instr << endl;

  f << "/gps/verbose 0" << endl;
  f << "/gps/particle neutron" << endl;
  f << "/gps/pos/centre 177.0 0.0 0.0 cm" << endl;
  f << "/gps/ang/type iso" << endl;
  f << "/gps/pos/type Point" << endl;
  f << "/gps/ang/mintheta 80 deg" << endl;
  f << "/gps/ang/maxtheta 100 deg" << endl;
  f << "/gps/ang/minphi -15 deg" << endl;
  f << "/gps/ang/maxphi 15 deg" << endl;
//  f << "/gps/pos/centre 15.0 0.0 0.0 cm" << endl;
////  f << "/gps/pos/centre -3.0 0.0 40.0 cm" << endl;
//  f << "/gps/pos/type Beam" << endl;
//  f << "/gps/ang/type beam2d" << endl;
//  f << "/gps/direction -1 0 0" << endl;
////  f << "/gps/direction 0 0 -1" << endl;
//  f << "/gps/ang/sigma_x 2 deg" << endl;
//  f << "/gps/ang/sigma_y 2 deg" << endl;
  f << "/gps/ene/type Arb" << endl;
  f << "/gps/hist/type arb" << endl;
  f << "/gps/hist/point    0.11	0.905149" << endl;
  f << "/gps/hist/point    0.33	0.848238" << endl;
  f << "/gps/hist/point    0.54	0.761518" << endl;
  f << "/gps/hist/point    0.75	0.677507" << endl;
  f << "/gps/hist/point    0.97	0.579946" << endl;
  f << "/gps/hist/point    1.18	0.536585" << endl;
  f << "/gps/hist/point    1.40	0.474255" << endl;
  f << "/gps/hist/point    1.61	0.520325" << endl;
  f << "/gps/hist/point    1.82	0.604336" << endl;
  f << "/gps/hist/point    2.04	0.582656" << endl;
  f << "/gps/hist/point    2.25	0.609756" << endl;
  f << "/gps/hist/point    2.47	0.617886" << endl;
  f << "/gps/hist/point    2.68	0.799458" << endl;
  f << "/gps/hist/point    2.90	0.964770" << endl;
  f << "/gps/hist/point    3.11	1.000000" << endl;
  f << "/gps/hist/point    3.32	0.937669" << endl;
  f << "/gps/hist/point    3.54	0.831978" << endl;
  f << "/gps/hist/point    3.75	0.813008" << endl;
  f << "/gps/hist/point    3.97	0.728997" << endl;
  f << "/gps/hist/point    4.18	0.775068" << endl;
  f << "/gps/hist/point    4.39	0.861789" << endl;
  f << "/gps/hist/point    4.61	0.831978" << endl;
  f << "/gps/hist/point    4.82	0.902439" << endl;
  f << "/gps/hist/point    5.04	0.823848" << endl;
  f << "/gps/hist/point    5.25	0.742547" << endl;
  f << "/gps/hist/point    5.47	0.631436" << endl;
  f << "/gps/hist/point    5.68	0.558266" << endl;
  f << "/gps/hist/point    5.89	0.493225" << endl;
  f << "/gps/hist/point    6.11	0.479675" << endl;
  f << "/gps/hist/point    6.32	0.552846" << endl;
  f << "/gps/hist/point    6.54	0.495935" << endl;
  f << "/gps/hist/point    6.75	0.441734" << endl;
  f << "/gps/hist/point    6.96	0.455285" << endl;
  f << "/gps/hist/point    7.18	0.455285" << endl;
  f << "/gps/hist/point    7.39	0.509485" << endl;
  f << "/gps/hist/point    7.61	0.498645" << endl;
  f << "/gps/hist/point    7.82	0.457995" << endl;
  f << "/gps/hist/point    8.03	0.390244" << endl;
  f << "/gps/hist/point    8.25	0.262331" << endl;
  f << "/gps/hist/point    8.46	0.176694" << endl;
  f << "/gps/hist/point    8.68	0.115447" << endl;
  f << "/gps/hist/point    8.89	0.099458" << endl;
  f << "/gps/hist/point    9.11	0.103252" << endl;
  f << "/gps/hist/point    9.32	0.137127" << endl;
  f << "/gps/hist/point    9.53	0.169377" << endl;
  f << "/gps/hist/point    9.75	0.149593" << endl;
  f << "/gps/hist/point    9.96	0.126829" << endl;
  f << "/gps/hist/point    10.2	0.100271" << endl;
  f << "/gps/hist/point    10.4	0.075338" << endl;
  f << "/gps/hist/point    10.6	0.040921" << endl;
  f << "/gps/hist/point    10.8	0.009837" << endl;
  //
  f << "/gps/hist/inter Spline" << endl;
  f << "/analysis/setFileName "<< pp <<"_Run_AmBeSource_" << instr   <<endl;
  f << "/run/beamOn 250" << endl;
  f.close();
  
  ofstream f2;
  string tempstr2 = pp+"_batch_"+str+string(".pbs");
  f2.open (tempstr2.c_str());
  f2 << "cd $PBS_O_WORKDIR" << endl;
  f2 << "echo \"Starting run at: \`date\`\"" <<endl; 
  f2 << "./PileRoomSim "<<pp<<"_input_" << instr << string(".mac") << endl;
  f2 << "echo \"End at: \`date\`\"" <<endl; 
  f2.close();

}


int main () {

  ofstream of;
  string rnam="run_script_"+pp+string(".sh");
  of.open (rnam.c_str());
  of << "#!/bin/bash" << endl;
  of << "mv *?.pbs.*? trask/" << endl;
  of << "mv *.root trask/" << endl;
  for(int i=0;i<1;i++){
    makeListMacFile(i);
    of << "qsub -l walltime=4:00:00,mem=6gb "<< pp <<"_batch_" <<i<<".pbs"<<endl;
  }
    of.close();
  return 0;
}
