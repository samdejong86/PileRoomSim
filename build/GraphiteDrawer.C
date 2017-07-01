

void GraphiteDrawer(TString filename="debug.root"){

  TFile *f = new TFile(filename);
  TH1F *aG = (TH1F*)f->Get("h1_7");
  
  aG->Scale(1/597.);

  TGraph *afterGraphite = new TGraph();

  for(int i=1; i<aG->GetNbinsX(); i++){
    double X = aG->GetBinCenter(i);
    double Y = aG->GetBinContent(i);

    afterGraphite->SetPoint(i-1, X,Y);
  }



  afterGraphite->GetXaxis()->SetTitle("Neutron Kinetic Energy (eV)");
  afterGraphite->GetYaxis()->SetTitle("Relative Abundance");

  afterGraphite->SetLineColor(1004);
  afterGraphite->SetLineWidth(2);
  
  afterGraphite->SetTitle("");
 
  afterGraphite->Draw();
  
  



}

void BinLogX(TH1*h){

   TAxis *axis = h->GetXaxis();
   int bins = axis->GetNbins();

   Axis_t from = axis->GetXmin();
   Axis_t to = axis->GetXmax();
   Axis_t width = (to - from) / bins;
   Axis_t *new_bins = new Axis_t[bins + 1];

   for (int i = 0; i <= bins; i++) {
     new_bins[i] = TMath::Power(10, from + i * width);

   }
   axis->Set(bins, new_bins);
   delete new_bins;
} 

void GraphiteDrawerLog(TString filename="PostGraphite.dat"){

  TH1F *hist = new TH1F("ag","", 300, -4, 8);
  BinLogX(hist);


  if(filename.Contains(".dat")){
      ifstream InFile;
      InFile.open(filename);
      
      double last=-3;
      //while(true){
      while(!InFile.eof()){
	double a;
	InFile>>a;
	hist->Fill(a);
      }
    }else{
      TFile *f = new TFile(filename);
      TTree *t = (TTree*)f->Get("PileRoomSim");
      
      double eKin;
      t->SetBranchAddress("Ekin_n_PostGraphite", &eKin);
      
      int nentries = t->GetEntries();
      for(int i=0; i<nentries; i++){
	t->GetEntry(i);
	if(eKin>0) hist->Fill(eKin);
      }
    }
    



  //hist->Draw();

  TGraph *afterGraphite = new TGraph();

  for(int i=1; i<hist->GetNbinsX(); i++){
    double X = hist->GetBinCenter(i);
    double Y = hist->GetBinContent(i);

    afterGraphite->SetPoint(i-1, X,Y);
  }



  afterGraphite->GetXaxis()->SetTitle("Neutron Kinetic Energy (eV)");
  afterGraphite->GetYaxis()->SetTitle("Relative Abundance");

  afterGraphite->SetLineColor(1004);
  afterGraphite->SetLineWidth(2);
  
  afterGraphite->SetTitle("");
 
  //afterGraphite->Draw();
  hist->Draw();

  TFile *f = new TFile("aftergraphite.root", "RECREATE");
  hist->Write();
  f->Close();


  gPad->SetLogx();




}
