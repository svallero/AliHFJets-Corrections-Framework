class AliHFJetContainerVertex;

void PlotEfficiency(){

  //TFile *f = new TFile("AnalysisResultsFull.root","r");
  TFile *f = new TFile("AnalysisResultsHFCJ.root","r");
    
  TList *dhf = (TList*)f->Get("HFJets_containers");
  
  AliHFJetsContainerVertex *hfcont = (AliHFJetsContainerVertex*)dhf->FindObject("kBJets");
    
  new TCanvas("c1");
  // Restrict eta range of MC jets
  //hfcont->SetAxisRangeAllSteps("kCFJetEta", -0.5, 0.5);
  TH1D *heffpt = (TH1D*)hfcont->GetEfficiencyPt();
  heffpt->DrawCopy();

  f->Close();
  delete hfcont;
}
