#include "StyleSara.h"

void PlotEfficiencySupraya(Float_t mass=0.,Int_t partonmethod=2, Float_t pt=10. ){

  // Load libraries before running this macro
  gROOT->Macro("LoadLibsSara.C");
  
  //partonmethod
      // 0=weights
      // 1=parton in jet cone
      // 2=meson in jet cone

  // Rebin factor
  Int_t rebin = 2;

  TString outdir="plots_test";
  //TFile* f=new TFile("AnalysisResults.root","r");
  TFile* f1=new TFile("outputsLHC13d18d/AnalysisResults_NEW_TRC1_MM2_FB768.root","r"); // good for matching eff.
  TFile* f2=new TFile("outputsLHC13d18d/AnalysisResults_NEW_TRC1_MM2_FB272.root","r");
  //TFile* f2=new TFile("outputsLHC13d18d/AnalysisResults_NEW_TRC0_MM1_FB768.root","r"); // good for matching eff.
  //TFile* f1=new TFile("outputsLHC13d18d/AnalysisResults_NEW_TRC0_MM1_FB272.root","r");

  TList *dhf = (TList*)f1->Get("HFJets_containers");
  TList *dhf2 = (TList*)f2->Get("HFJets_containers"); // bit 272

  // Get the 4 containers
  //fSparseRecoJets=(THnSparseF*)dir->Get("fSparseRecoJets");
  AliHFJetsContainerVertex *hfcontJets = (AliHFJetsContainerVertex*)dhf->FindObject("kJets");
  AliHFJetsContainerVertex *hfcontJets2 = (AliHFJetsContainerVertex*)dhf2->FindObject("kJets");
  //fSparseVerticesProp=(THnSparseF*)dir->Get("fSparseVerticesProp");
  //AliHFJetsContainerVertex *hfcontQaVtx = (AliHFJetsContainerVertex*)dhf->FindObject("kQaVtx");
  //fSparseJetVerticesProp=(THnSparseF*)dir->Get("fSparseJetVerticesProp");
  AliHFJetsContainerVertex *hfcontJetsVtx = (AliHFJetsContainerVertex*)dhf->FindObject("kJetVtx");
  AliHFJetsContainerVertex *hfcontJetsVtx2 = (AliHFJetsContainerVertex*)dhf2->FindObject("kJetVtx");
  //fSparsebTaggedJets=(THnSparseF*)dir->Get("fSparsebTaggedJets");
  //AliHFJetsContainerVertex *hfcontBJets = (AliHFJetsContainerVertex*)dhf->FindObject("kBJets");

  // STEP
  AliHFJetsContainer::CFSteps step = AliHFJetsContainer::kCFStepRecoB;
 
  // Restrict parton axis corresponding to chosen method
  TString axisname("");
  if(partonmethod==0)axisname=Form("partDP");
  if(partonmethod==1)axisname=Form("partBP");
  if(partonmethod==2)axisname=Form("partBH");

  // Restrict eta range 
  // should have no effect since cuts are already in AliRDHFJetsCuts
  hfcontJets->SetAxisRangeStep("kCFJetEta", -0.5, 0.5, step);
  hfcontJetsVtx->SetAxisRangeStep("kCFJetEta", -0.5, 0.5, step);
  hfcontJets2->SetAxisRangeStep("kCFJetEta", -0.5, 0.5, step);
  hfcontJetsVtx2->SetAxisRangeStep("kCFJetEta", -0.5, 0.5, step);
  // Restrict pT range
  hfcontJets->SetAxisRangeStep("kCFJetPt", pt, -1, step);
  hfcontJetsVtx->SetAxisRangeStep("kCFJetPt", pt, -1, step);
  hfcontJets2->SetAxisRangeStep("kCFJetPt", pt, -1, step);
  hfcontJetsVtx2->SetAxisRangeStep("kCFJetPt", pt, -1, step);
  // Ask for at least 1 vtx at numerator
  // compulsory to include overflow (last parameter) 
  //hfcontJetsVtx->SetAxisRangeStep("nRecoVtx", 1., -1, step,kTRUE);
  hfcontJetsVtx2->SetAxisRangeStep("nRecoVtx", 1., -1, step,kTRUE);

  CreateCanvas("Dummy",kFALSE);
  TH1D* dummy=(TH1D*)hfcontJetsVtx->Project1D(step, "Lxy1");
  //TH1D* dummy=(TH1D*)hfcontJets->Project1D(step, "partBP");
  Color(dummy,1);
  dummy->DrawCopy();
  //hfcontJets->SetAxisRangeStep(axisname.Data(), 1, 2,step);
  //TH1D *dummy2=(TH1D*)hfcontJets->Project1D(step, "partBP");
  //Color(dummy2,2, 25);
  //dummy2->DrawCopy("same");

  // B
  hfcontJets->SetAxisRangeStep(axisname.Data(), 4, 4,step);
  TH1D* hallb=(TH1D*)hfcontJets->Project1D(step, "kCFJetPt");
  hfcontJets2->SetAxisRangeStep(axisname.Data(), 4, 4,step);
  TH1D* hallb2=(TH1D*)hfcontJets2->Project1D(step, "kCFJetPt");
  // C
  hfcontJets->SetAxisRangeStep(axisname.Data(), 3, 3,step);
  TH1D* hallc=(TH1D*)hfcontJets->Project1D(step, "kCFJetPt");
  // gluons + light quarks
  //hfcontJets->SetAxisRangeStep(axisname.Data(), 0.5, 1.5,step);
  hfcontJets->SetAxisRangeStep(axisname.Data(), 1, 2,step);
  TH1D* hallg=(TH1D*)hfcontJets->Project1D(step, "kCFJetPt");
  // light quarks
  //TH1D *halllq;
  //hfcontJets->SetAxisRangeStep(axisname.Data(), 2, 2,step);
  //halllq=(TH1D*)hfcontJets->Project1D(step, "kCFJetPt");

  // EFFICIENCY FOR B 
  CreateCanvas("Compare with Supraya - no cuts", kFALSE);  

  // Set mass1 range
  hfcontJetsVtx->SetAxisRangeStep("mass1", 2., -1,step, kTRUE);
  // Set Lxy1 range
  // in this way you exclude underflow
  //hfcontJetsVtx->SetAxisRangeStep("Lxy1", 0.06, -1,step, kTRUE); // include overflow
  //hfcontJetsVtx2->SetAxisRangeStep("Lxy1", 0.4, -1,step, kTRUE); // include overflow

  // Set parton range B
  hfcontJetsVtx->SetAxisRangeStep(axisname.Data(), 4,4 ,step); 
  hfcontJetsVtx2->SetAxisRangeStep(axisname.Data(), 4, 4,step); 
  // Project on jet pT
  TH1D *h0=(TH1D*)hfcontJetsVtx->Project1D(step, "kCFJetPt");
  TH1D *h02=(TH1D*)hfcontJetsVtx2->Project1D(step, "kCFJetPt");
  Color(h0,1,21);
  Color(h02,2,21);
  h0->Divide(hallb);
  h02->Divide(hallb2);
  h0->DrawCopy();
  h02->DrawCopy("same");

  // Legend
  TLegend *leg1 = NewLegend("Filterbit: ");
  leg1->AddEntry(h0,"768","lpf");
  leg1->AddEntry(h02,"272","lpf");
  leg1->Draw("same");
  
  CreateCanvas("Filterbit 768 - no cuts");
  h0->Rebin(rebin);
  h0->Scale(1./rebin);
  h0->DrawCopy();
  // Set parton range C
  hfcontJetsVtx->SetAxisRangeStep(axisname.Data(), 3, 3,step); 
  // Project on jet pT
  TH1D *h1=(TH1D*)hfcontJetsVtx->Project1D(step, "kCFJetPt");
  Color(h1,2,21);
  h1->Divide(hallc);
  h1->Rebin(rebin);
  h1->Scale(1./rebin);
  h1->DrawCopy("same");

  // Set parton range Light
  //hfcontJetsVtx->SetAxisRangeStep(axisname.Data(), 2, 2,step); 
  // Project on jet pT
  //TH1D *h2=(TH1D*)hfcontJetsVtx->Project1D(step, "kCFJetPt");
  //Color(h2,3,21);
  //h2->Divide(halllq);
  //h2->DrawCopy("same");

  // Set parton range Gluons + Light
  //hfcontJetsVtx->SetAxisRangeStep(axisname.Data(), 1, 1,step); 
  hfcontJetsVtx->SetAxisRangeStep(axisname.Data(), 1, 2,step); 
  // Project on jet pT
  TH1D *h3=(TH1D*)hfcontJetsVtx->Project1D(step, "kCFJetPt");
  Color(h3,4,21);
  h3->Divide(hallg);
  h3->Rebin(rebin);
  h3->Scale(1./rebin);
  h3->DrawCopy("same");
  
  // Legend 
  TLegend *leg = NewLegend("Jet flavour");
  leg->AddEntry(h0->Clone(),"Beauty","lpf");
  leg->AddEntry(h1,"Charm","lpf");
  //leg->AddEntry(h2,"Light","lpf");
  leg->AddEntry(h3,"Light + Gluon","lpf");
  leg->Draw("same");
}
