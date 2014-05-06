#include "StyleSara.h"

class AliHFJetContainerVertex;

void StudyCuts(){
  
 
  // CONFIGURATION GOES HERE ----------------------------------------- 
  // What to plot
  TString var("Lxy");
  //TString var("mass");
  // Flavour ID method 
  TString meth("partBP");
  // Eta range of jets (MC-jets in all steps except kCFStepRecoB)
  Float_t etacut = 0.5;
  // pT range of jets (MC-jets in all steps except kCFStepRecoB)
  Float_t ptmin = 10.;
  Float_t ptmax = -1;
  // Tagging cuts
  Float_t lxymin=0.0;
  Float_t massmin=0.0;
  // Which farthest vertex for cuts 
  Int_t farthest=1;
  // Rebin 
  Int_t rebin= 2.; 
  // Plot ratio to B in Lxy and mass distributions
  Bool_t ratio = kFALSE;
  // ------------------------------------------------------------------



  // Load libraries before running this macro
  gROOT->Macro("LoadLibsSara.C");
  
  // Input files
  // For B
  TFile* f1=new TFile("outputs528/AR-LHC13d18bcde-plusplus.root","r"); 
  // For C
  TFile* f2=new TFile("outputs528/AR-LHC13d18bcde-plusplus.root","r"); 
  //TFile* f2=new TFile("outputsLHC13d18d/AnalysisResults_NEW_TRC0_MM1_FB768_merged.root","r"); 
  // For Light/Gluon
  TFile* f3=new TFile("outputs528/AR-LHC13d18bcde-plusplus.root","r"); 

  TList *dhf1= (TList*)f1->Get("HFJets_containers");
  TList *dhf2= (TList*)f2->Get("HFJets_containers");
  TList *dhf3= (TList*)f3->Get("HFJets_containers");
  
  // Get the containers
  AliHFJetsContainerVertex *hfcont1 = (AliHFJetsContainerVertex*)dhf1->FindObject("kJetVtx");
  AliHFJetsContainerVertex *hfcont2 = (AliHFJetsContainerVertex*)dhf2->FindObject("kJetVtx");
  AliHFJetsContainerVertex *hfcont3 = (AliHFJetsContainerVertex*)dhf3->FindObject("kJetVtx");

  // To check available variables/steps for the container
  //hfcont1->PrintVars();
  //hfcont1->PrintSteps();   
   
  // Restrict eta range of MC jets
  hfcont1->SetAxisRangeAllSteps("kCFJetEta", -etacut, etacut);
  hfcont2->SetAxisRangeAllSteps("kCFJetEta", -etacut, etacut);
  hfcont3->SetAxisRangeAllSteps("kCFJetEta", -etacut, etacut);
  // Restrict pT range of MC jets
  hfcont1->SetAxisRangeAllSteps("kCFJetPt", ptmin, ptmax);
  hfcont2->SetAxisRangeAllSteps("kCFJetPt", ptmin, ptmax);
  hfcont3->SetAxisRangeAllSteps("kCFJetPt", ptmin, ptmax);
 

  // CUTS
  AliHFJetsContainer::CFSteps arr[5];
  arr[0]=AliHFJetsContainer::kCFStepMatchedB;
  arr[1]=AliHFJetsContainer::kCFStepMatchedC;
  arr[2]=AliHFJetsContainer::kCFStepMatchedLight;
  arr[3]=AliHFJetsContainer::kCFStepMatchedGluon;
  arr[4]=AliHFJetsContainer::kCFStepMatchedAny;
  for(Int_t i=0; i<5; i++){
     // At least 1 vertex
     hfcont1->SetAxisRangeStep("nRecoVtx",1.,-1,arr[i], kTRUE);
     hfcont2->SetAxisRangeStep("nRecoVtx",1.,-1,arr[i], kTRUE);
     hfcont3->SetAxisRangeStep("nRecoVtx",1.,-1,arr[i], kTRUE);
     // Lxy 
     hfcont1->SetAxisRangeStep(Form("Lxy%d",farthest),lxymin,-1,arr[i], kTRUE);
     hfcont2->SetAxisRangeStep(Form("Lxy%d",farthest),lxymin,-1,arr[i], kTRUE);
     hfcont3->SetAxisRangeStep(Form("Lxy%d",farthest),lxymin,-1,arr[i], kTRUE);
     // Invariant mass
     hfcont1->SetAxisRangeStep(Form("mass%d",farthest),massmin,-1,arr[i], kTRUE);
     hfcont2->SetAxisRangeStep(Form("mass%d",farthest),massmin,-1,arr[i], kTRUE);
     hfcont3->SetAxisRangeStep(Form("mass%d",farthest),massmin,-1,arr[i], kTRUE);
     }

  // Lxy by flavour
  if (var.Contains("Lxy"))
  	CreateCanvas(Form("farthest vertex %d - %f < pT < %f - mass > %f", farthest, ptmin, ptmax, massmin));
  else if (var.Contains("mass"))
  	CreateCanvas(Form("farthest vertex %d - %f < pT < %f - lxy > %f", farthest, ptmin, ptmax, lxymin));
  hfcont1->SetAxisRangeStep(meth.Data(),4,4,AliHFJetsContainer::kCFStepRecoB);// select B
  TH1D *hlb = (TH1D*)hfcont1->Project1D(AliHFJetsContainer::kCFStepRecoB,Form("%s%d",var.Data(),farthest));
  Color(hlb,1,20);
  hlb->Rebin(rebin);
  hlb->Scale(1./rebin);
  TH1D *tmp = hlb->Clone();
  if (ratio){
	 hlb->GetYaxis()->SetTitle("Ratio");
         hlb->Divide(tmp);
  }else hlb->GetYaxis()->SetTitle("Entries");
  hlb->DrawCopy("hist");
  hfcont2->SetAxisRangeStep(meth.Data(),3,3,AliHFJetsContainer::kCFStepRecoB);// select C
  TH1D *hlc = (TH1D*)hfcont2->Project1D(AliHFJetsContainer::kCFStepRecoB,Form("%s%d",var.Data(),farthest));
  Color(hlc,2,20);
  hlc->Rebin(rebin);
  hlc->Scale(1./rebin);
  if (ratio) hlc->Divide(tmp);
  hlc->DrawCopy("hist,same");
  hfcont3->SetAxisRangeStep(meth.Data(),2,2,AliHFJetsContainer::kCFStepRecoB);// select Light
  TH1D *hll = (TH1D*)hfcont3->Project1D(AliHFJetsContainer::kCFStepRecoB,Form("%s%d",var.Data(),farthest));
  Color(hll,3,20);
  hll->Rebin(rebin);
  hll->Scale(1./rebin);
  if (ratio) hll->Divide(tmp);
  hll->DrawCopy("hist,same");
  hfcont3->SetAxisRangeStep(meth.Data(),1,1,AliHFJetsContainer::kCFStepRecoB);// select Gluon
  TH1D *hlg = (TH1D*)hfcont3->Project1D(AliHFJetsContainer::kCFStepRecoB,Form("%s%d",var.Data(),farthest));
  Color(hlg,4,20);
  hlg->Rebin(rebin);
  hlg->Scale(1./rebin);
  if (ratio) hlg->Divide(tmp);
  hlg->DrawCopy("hist,same");
 
  // Legend
  TLegend *leg1 = NewLegend("Flavour: ");
  leg1->AddEntry(hlb,"Beauty","lf");
  leg1->AddEntry(hlc,"Charm","lf");
  leg1->AddEntry(hll,"Light","lf");
  leg1->AddEntry(hlg,"Gluon","lf");
  leg1->Draw("same");
 
  // EFFICIENCY by FLAVOUR
  CreateCanvas("Efficiency",kFALSE);
  // Get efficiency for B
  TH1D *hbeffpt = (TH1D*)hfcont1->GetBEfficiencyPt(meth.Data());
  Color(hbeffpt,1,20);
  hbeffpt->Rebin(rebin);
  hbeffpt->Scale(1./rebin);
  hbeffpt->GetYaxis()->SetTitle("Efficiency");
  hbeffpt->DrawCopy();
  // Get efficiency for C
  TH1D *hceffpt = (TH1D*)hfcont2->GetCEfficiencyPt(meth.Data());
  Color(hceffpt,2,20);
  hceffpt->Rebin(rebin);
  hceffpt->Scale(1./rebin);
  hceffpt->DrawCopy("same");
  // Get efficiency for light-quarks
  TH1D *hleffpt = (TH1D*)hfcont3->GetLightEfficiencyPt(meth.Data());
  Color(hleffpt,3,20);
  hleffpt->Rebin(rebin);
  hleffpt->Scale(1./rebin);
  hleffpt->DrawCopy("same");
  // Get efficiency for gluons
  TH1D *hgeffpt = (TH1D*)hfcont3->GetGluonEfficiencyPt(meth.Data());
  Color(hgeffpt,4,20);
  hgeffpt->Rebin(rebin);
  hgeffpt->Scale(1./rebin);
  hgeffpt->DrawCopy("same");

  // Legend 
  TLegend *leg = NewLegend("Bit 528 - Jet flavour:");
  leg->AddEntry(hbeffpt,"Beauty","lp");
  leg->AddEntry(hceffpt,"Charm","lp");
  leg->AddEntry(hleffpt,"Light","lp");
  leg->AddEntry(hgeffpt,"Gluon","lp");
  leg->Draw("same");

}

