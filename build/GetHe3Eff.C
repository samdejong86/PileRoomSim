void GetHe3Eff(){

TFile *f = new TFile("Mono_Neutron.root"); 

 TH1D * h1 = (TH1D*)f->Get("h1_4"); 
 TH1D * h2 = (TH1D*)f->Get("h1_2"); 
 h1->Rebin(200); 
 h2->Rebin(200);
 
  vector<TGraphAsymmErrors*> Ratios_h;
  Ratios_h.push_back(new TGraphAsymmErrors);
  Ratios_h[0]->BayesDivide(h1,h2,"cl=0.683 b(1,1) mode");
  Ratios_h[0]->Draw();

//TH2D * h_2d = (TH2D*)f.Get("h2_1"); 

//vector<double> Eff_Bins(10000);
//vector<double> ENE_Bins(10000);
//
//for(int i=0;i<Eff_Bins.size();i++){
//  if(h2->GetBinContent(i+1)!=0){
//    Eff_Bins[i]=h1->GetBinContent(i+1)/h2->GetBinContent(i+1);
//  }else{
//    Eff_Bins[i]=0; 
//  }
//  ENE_Bins[i]=10.0/10000.0*(i);
//}
//
//TGraph * g = new TGraph(Eff_Bins.size(),&ENE_Bins[0],&Eff_Bins[0]);
//g->Draw("AP");

}
