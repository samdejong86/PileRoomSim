

TGraphErrors *getFluxVsRadius(TString filename){

  TFile *f = new TFile(filename);
  TTree *t = (TTree*) f->Get("PileRoomSim");

  vector<double> *flux;
  vector<double> *radius;

  t->SetBranchAddress("diffusionRadius", &radius);
  t->SetBranchAddress("diffusionFlux", &flux);

  int nentries=t->GetEntries();
  
  t->GetEntry(1);

  vector<double> fluxTotal;
  fluxTotal.resize(flux->size());


  for(int i=0; i<nentries; i++){
    t->GetEntry(i);
    for(int j=0; j<flux->size(); j++)
      fluxTotal[j]+=flux->at(j);   
    
  }

  TGraphErrors *gr = new TGraphErrors();
 
  for(int j=0; j<flux->size(); j++){
    double error = sqrt(fluxTotal[j]/(3.14159))/(2*radius->at(j));
    gr->SetPoint(j, radius->at(j), fluxTotal[j]);
    gr->SetPointError(j, 0, error);

  }

  gr->GetXaxis()->SetTitle("Radius (mm)");
  gr->GetYaxis()->SetTitle("Flux (neutrons/mm^{2})");

  return gr;



}
