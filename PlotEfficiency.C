#include "StyleSara.h"

class AliHFJetContainerVertex;

void PlotEfficiency(){
  
  // Load libraries before running this macro
  gROOT->Macro("LoadLibsSara.C");

  // Input file
  TFile *f = new TFile("AnalysisResults.root","r");
  //TFile *f = new TFile("AnalysisResultsHFCJ.root","r"); // meth = 2
  //TFile *f = new TFile("AnalysisResultsMeth1.root","r"); // meth = 1 
    
  TList *dhf = (TList*)f->Get("HFJets_containers");
  
  // Get the container
  //AliHFJetsContainerVertex *hfcont = (AliHFJetsContainerVertex*)dhf->FindObject("kBJets");
  //AliHFJetsContainerVertex *hfcont = (AliHFJetsContainerVertex*)dhf->FindObject("kJets");
  AliHFJetsContainerVertex *hfcont = (AliHFJetsContainerVertex*)dhf->FindObject("kJetVtx");

  // To check available variables/steps for the container
  hfcont->PrintVars();
  hfcont->PrintSteps();   
   
  // Restrict eta range of MC jets
  hfcont->SetAxisRangeAllSteps("kCFJetEta", -0.5, 0.5);
  
  // CUTS
  hfcont->SetAxisRangeStep("Lxy1",0.4,1.0,AliHFJetsContainer::kCFStepMatchedB);
  //hfcont->SetAxisRangeStep("mass1",2.,5.,AliHFJetsContainer::kCFStepMatchedB);
  hfcont->SetAxisRangeStep("Lxy1",0.4,1.,AliHFJetsContainer::kCFStepMatchedC);
  //hfcont->SetAxisRangeStep("mass1",2.,5.,AliHFJetsContainer::kCFStepMatchedC);
  hfcont->SetAxisRangeStep("Lxy1",0.4,1.,AliHFJetsContainer::kCFStepMatchedLight);
  //hfcont->SetAxisRangeStep("mass1",2.,5.,AliHFJetsContainer::kCFStepMatchedLight);
  hfcont->SetAxisRangeStep("Lxy1",0.4,1.,AliHFJetsContainer::kCFStepMatchedGluon);
  //hfcont->SetAxisRangeStep("mass1",2.,5.,AliHFJetsContainer::kCFStepMatchedGluon);


  // Dummy
  CreateCanvas("Dummy",kFALSE);
  TH1D *pnat = (TH1D*)hfcont->Project1D(AliHFJetsContainer::kCFStepMatchedB,"mass1"); 
  //TH1D *pnat = (TH1D*)hfcont->Project1D(AliHFJetsContainer::kCFStepMatchedB,"Lxy1"); 
  pnat->Draw();

  // Canvas 
  CreateCanvas("No cuts",kFALSE);
  // Flavour ID method
  TString meth("partBP");
  
  // Get efficiency for B
  TH1D *hbeffpt = (TH1D*)hfcont->GetBEfficiencyPt(meth.Data());
  Color(hbeffpt,1,20);
  hbeffpt->DrawCopy();
  // Get efficiency for C
  TH1D *hceffpt = (TH1D*)hfcont->GetCEfficiencyPt(meth.Data());
  Color(hceffpt,2,20);
  hceffpt->DrawCopy("same");
  // Get efficiency for light-quarks
  TH1D *hleffpt = (TH1D*)hfcont->GetLightEfficiencyPt(meth.Data());
  Color(hleffpt,3,20);
  hleffpt->DrawCopy("same");
  // Get efficiency for gluons
  TH1D *hgeffpt = (TH1D*)hfcont->GetGluonEfficiencyPt(meth.Data());
  Color(hgeffpt,4,20);
  hgeffpt->DrawCopy("same");

  // Legend 
  TLegend *leg = NewLegend("Jet flavour");
  leg->AddEntry(hbeffpt->Clone(),"Beauty","lpf");
  leg->AddEntry(hceffpt,"Charm","lpf");
  leg->AddEntry(hleffpt,"Light","lpf");
  leg->AddEntry(hgeffpt,"Gluon","lpf");
  leg->Draw("same");

  //f->Close();
  //delete hfcont;
}
