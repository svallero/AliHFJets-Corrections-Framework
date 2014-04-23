#include "StyleSara.h"

class AliHFJetContainerVertex;

void PlotEfficiency(){
  
  // Load libraries before running this macro
  gROOT->Macro("LoadLibsSara.C");

  // Input file
  //TFile *f = new TFile("AnalysisResults.root","r");
  TFile *f = new TFile("AnalysisResultsHFCJ.root","r");
    
  TList *dhf = (TList*)f->Get("HFJets_containers");
  
  // Get the container
  //AliHFJetsContainerVertex *hfcont = (AliHFJetsContainerVertex*)dhf->FindObject("kBJets");
  AliHFJetsContainerVertex *hfcont = (AliHFJetsContainerVertex*)dhf->FindObject("kJets");

  // To check available variables/steps for the container
  //hfcont->PrintVars();
  //hfcont->PrintSteps();   
   
  // Restrict eta range of MC jets
  hfcont->SetAxisRangeAllSteps("kCFJetEta", -0.5, 0.5);

  // No cuts  
  CreateCanvas("No cuts",kFALSE);

  // Get efficiency for B
  TH1D *hbeffpt = (TH1D*)hfcont->GetBEfficiencyPt("partBH");
  Color(hbeffpt,1,20);
  hbeffpt->DrawCopy();
  // Get efficiency for C
  TH1D *hceffpt = (TH1D*)hfcont->GetCEfficiencyPt("partBH");
  Color(hceffpt,2,20);
  hceffpt->DrawCopy("same");
  // Get efficiency for light-quarks
  TH1D *hleffpt = (TH1D*)hfcont->GetLightEfficiencyPt("partBH");
  Color(hleffpt,3,20);
  hleffpt->DrawCopy("same");
  // Get efficiency for gluons
  TH1D *hgeffpt = (TH1D*)hfcont->GetGluonEfficiencyPt("partBH");
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
