#include "StyleSara.h"

class AliHFJetContainerVertex;

void PlotMatchingEfficiency(){
  
  // Load libraries before running this macro
  gROOT->Macro("LoadLibsSara.C");

  // Input file
  TFile* f1=new TFile("outputs528/AR-LHC13d18bcde-plusplus.root","r"); 
  TFile* f2=new TFile("outputs528/AnalysisResults-LHC10f7a.root","r"); 

  TList *tmp = (TList*)f1->Get("HFJets_containers");
  TList *tmp2 = (TList*)f2->Get("HFJets_containers");
  TList *dhf = (TList*)tmp->FindObject("HFJets_containers");
  TList *dhf2 = (TList*)tmp2->FindObject("HFJets_containers");
  dhf->ls();
  return; 
  // Get the container
  AliHFJetsContainerVertex *hfcont = (AliHFJetsContainerVertex*)dhf->FindObject("kJetVtx");
  AliHFJetsContainerVertex *hfcont2 = (AliHFJetsContainerVertex*)dhf2->FindObject("kJetVtx"); 

  // To check available variables/steps for the container
  //hfcont->PrintVars();
  //hfcont->PrintSteps();   
   
  // Restrict eta range of MC jets
  hfcont->SetAxisRangeAllSteps("kCFJetEta", -0.5, 0.5);
  hfcont2->SetAxisRangeAllSteps("kCFJetEta", -0.5, 0.5);
  // Restrict pT range of MC jets
  hfcont->SetAxisRangeAllSteps("kCFJetPt", 10., -1);
  hfcont2->SetAxisRangeAllSteps("kCFJetPt", 10., -1);
 
  // Flavour ID method
  TString meth("partBP");
  
  // Rebin
  Int_t rebin= 2.; 

  // CUTS
  Float_t lxymin=0.0;
  Float_t massmin=0.;
  Float_t nvtxmin=0.;
  AliHFJetsContainer::CFSteps arr[5];
  arr[0]=AliHFJetsContainer::kCFStepMatchedB;
  arr[1]=AliHFJetsContainer::kCFStepMatchedC;
  arr[2]=AliHFJetsContainer::kCFStepMatchedLight;
  arr[3]=AliHFJetsContainer::kCFStepMatchedGluon;
  arr[4]=AliHFJetsContainer::kCFStepMatchedAny;
  for(Int_t i=0; i<5; i++){
     //hfcont->SetAxisRangeStep("Lxy1",lxymin,-1,arr[i], kTRUE);
     //hfcont->SetAxisRangeStep("mass1",massmin,-1,arr[i], kTRUE);
     //hfcont->SetAxisRangeStep("nRecoVtx",nvtxmin,-1,arr[i], kTRUE);
     }
 
  // Matching efficiency (all flavours)
  CreateCanvas("Matching efficiency", kFALSE);
  TH1D *hmatcheffpt = (TH1D*)hfcont->GetMatchingEfficiencyPt(meth.Data());
  Color(hmatcheffpt,1,20);
  hmatcheffpt->DrawCopy();
  TH1D *hmatcheffpt2 = (TH1D*)hfcont2->GetMatchingEfficiencyPt(meth.Data());
  Color(hmatcheffpt2,2,20);
  hmatcheffpt2->DrawCopy("same");
  // Legend
  TLegend *leg1 = NewLegend("MC production: ");
  leg1->AddEntry(hmatcheffpt,"LHC13d18 (beauty -realistic decay)","lpf");
  leg1->AddEntry(hmatcheffpt2,"LHC10f7a (charm - j/psi to ee)","lpf");
  leg1->Draw("same");
  
  // Canvas 
  CreateCanvas("No cuts",kFALSE);
  //CreateCanvas("Lxy1 > 0.4 cm",kFALSE);
  // Get efficiency for B
  TH1D *hbeffpt = (TH1D*)hfcont->GetBEfficiencyPt(meth.Data());
  Color(hbeffpt,1,20);
  hbeffpt->Rebin(rebin);
  hbeffpt->Scale(1./rebin);
  hbeffpt->GetYaxis()->SetTitle("Efficiency");
  hbeffpt->DrawCopy();
  // Get efficiency for C
  TH1D *hceffpt = (TH1D*)hfcont->GetCEfficiencyPt(meth.Data());
  Color(hceffpt,2,20);
  hceffpt->Rebin(rebin);
  hceffpt->Scale(1./rebin);
  hceffpt->DrawCopy("same");
  //hceffpt->DrawCopy();
  // Get efficiency for light-quarks
  TH1D *hleffpt = (TH1D*)hfcont->GetLightEfficiencyPt(meth.Data());
  Color(hleffpt,3,20);
  hleffpt->Rebin(rebin);
  hleffpt->Scale(1./rebin);
  hleffpt->DrawCopy("same");
  // Get efficiency for gluons
  TH1D *hgeffpt = (TH1D*)hfcont->GetGluonEfficiencyPt(meth.Data());
  Color(hgeffpt,4,20);
  hgeffpt->Rebin(rebin);
  hgeffpt->Scale(1./rebin);
  hgeffpt->DrawCopy("same");
  //hgeffpt->DrawCopy();
  // Get matching efficiency
  TH1D *hmeffpt = (TH1D*)hfcont->GetMatchingEfficiencyPt(meth.Data());
  Color(hmeffpt,1,20);
  hmeffpt->SetLineStyle(9);
  hmeffpt->Rebin(rebin);
  hmeffpt->Scale(1./rebin);
  hmeffpt->DrawCopy("histo,same");
  //hgeffpt->DrawCopy();

  // Legend 
  TLegend *leg = NewLegend(Form("Method: %s - Jet flavour:",meth.Data()));
  leg->AddEntry(hbeffpt->Clone(),"Beauty","lpf");
  leg->AddEntry(hceffpt,"Charm","lpf");
  //leg->AddEntry(hleffpt,"Light + Gluon","lpf");
  leg->AddEntry(hleffpt,"Light","lpf");
  leg->AddEntry(hgeffpt,"Gluon","lpf");
  leg->AddEntry(hmeffpt,"All","l");
  leg->Draw("same");

  //f->Close();
  //delete hfcont;
}
