/*
 *  This root script returns a graph of the flux vs radius in the graphite
 */

TGraphErrors *getFluxVsRadius(TString filename){

  //load ntuple
  TFile *f = new TFile(filename);
  TTree *t = (TTree*) f->Get("PileRoomSim");

  //create vectors
  vector<double> *flux;
  vector<double> *radius;

  //assign branches
  t->SetBranchAddress("diffusionRadius", &radius);
  t->SetBranchAddress("diffusionFlux", &flux);

  int nentries=t->GetEntries();
  t->GetEntry(1);

  vector<double> fluxTotal;
  fluxTotal.resize(flux->size());

  //get total flux in this run (sum of the flux in all events)
  for(int i=0; i<nentries; i++){
    t->GetEntry(i);
    for(int j=0; j<flux->size(); j++)
      fluxTotal[j]+=flux->at(j);   
    
  }

  TGraphErrors *gr = new TGraphErrors();
 
  for(int j=0; j<flux->size(); j++){
    double error = sqrt(fluxTotal[j]/(3.14159))/(2*radius->at(j));  //calculate error on flux = (1/2r)*sqrt(flux/pi)
    gr->SetPoint(j, radius->at(j), fluxTotal[j]);
    gr->SetPointError(j, 0, error);

  }

  //label axes
  gr->GetXaxis()->SetTitle("Radius (mm)");
  gr->GetYaxis()->SetTitle("Flux (neutrons/mm^{2})");

  return gr;



}
