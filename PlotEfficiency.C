class AliHFJetContainerVertex;

void PlotEfficiency(){

  //TFile *f = new TFile("AnalysisResultsFull.root","r");
  TFile *f = new TFile("AnalysisResultsHFCJ.root","r");
    
  TList *dhf = (TList*)f->Get("HFJets_containers");
  
  //AliHFJetsContainerVertex *hfcont = (AliHFJetsContainerVertex*)dhf->FindObject("kBJets");
  AliHFJetsContainerVertex *hfcont = (AliHFJetsContainerVertex*)dhf->FindObject("kJets");
    
  new TCanvas("c1");
  // Restrict eta range of MC jets
  hfcont->SetAxisRangeAllSteps("kCFJetEta", -0.5, 0.5);
  // Get efficiency for B
  TH1D *hbeffpt = (TH1D*)hfcont->GetBEfficiencyPt("partBH");
  hbeffpt->SetMarkerStyle(20);
  hbeffpt->SetMarkerColor(1);
  hbeffpt->DrawCopy();
  TH1D *hceffpt = (TH1D*)hfcont->GetCEfficiencyPt("partBH");
  hceffpt->SetMarkerStyle(20);
  hceffpt->SetMarkerColor(2);
  hceffpt->DrawCopy("same");
  //TH1D *test = (TH1D*)hfcont->Project1D(AliHFJetsContainer::kCFStepMatchedB,"partBH");
  //test->DrawCopy();
  //hfcont->PrintSteps();
  f->Close();
  delete hfcont;
}
