#include "StyleSara.h"

class AliHFJetContainerVertex;

void PlotVertexEfficiency(){
  
  // Load libraries before running this macro
  gROOT->Macro("LoadLibsSara.C");
  
  // Flavour ID method -----------------------------------------------
  TString meth("partBP");
  
  // Rebin -----------------------------------------------------------
  Int_t rebin= 2.; 

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
  hfcont1->SetAxisRangeAllSteps("kCFJetEta", -0.5, 0.5);
  hfcont2->SetAxisRangeAllSteps("kCFJetEta", -0.5, 0.5);
  hfcont3->SetAxisRangeAllSteps("kCFJetEta", -0.5, 0.5);
  // Restrict pT range of MC jets
  hfcont1->SetAxisRangeAllSteps("kCFJetPt", 10., -1);
  hfcont2->SetAxisRangeAllSteps("kCFJetPt", 10., -1);
  hfcont3->SetAxisRangeAllSteps("kCFJetPt", 10., -1);
 
  // CUTS
  AliHFJetsContainer::CFSteps arr[5];
  arr[0]=AliHFJetsContainer::kCFStepMatchedB;
  arr[1]=AliHFJetsContainer::kCFStepMatchedC;
  arr[2]=AliHFJetsContainer::kCFStepMatchedLight;
  arr[3]=AliHFJetsContainer::kCFStepMatchedGluon;
  arr[4]=AliHFJetsContainer::kCFStepMatchedAny;
  for(Int_t i=0; i<5; i++){
     hfcont1->SetAxisRangeStep("nRecoVtx",1.,-1,arr[i], kTRUE);
     hfcont2->SetAxisRangeStep("nRecoVtx",1.,-1,arr[i], kTRUE);
     hfcont3->SetAxisRangeStep("nRecoVtx",1.,-1,arr[i], kTRUE);
     }

  // Canvas--------------------------------------------------------------
  CreateCanvas("Vertex reco efficiency * jet-matching efficiency",kFALSE);
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
  // Get matching efficiency
  TH1D *hmeffpt = (TH1D*)hfcont1->GetMatchingEfficiencyPt(meth.Data());
  Color(hmeffpt,1,20);
  hmeffpt->SetLineStyle(9);
  hmeffpt->Rebin(rebin);
  hmeffpt->Scale(1./rebin);
  //hmeffpt->DrawCopy("histo,same");

  // Legend 
  //TLegend *leg = NewLegend(Form("Method: %s - Jet flavour:",meth.Data()));
  TLegend *leg = NewLegend("Bit 528 - Jet flavour:");
  leg->AddEntry(hbeffpt->Clone(),"Beauty","lpf");
  leg->AddEntry(hceffpt,"Charm","lpf");
  //leg->AddEntry(hleffpt,"Light + Gluon","lpf");
  leg->AddEntry(hleffpt,"Light","lpf");
  leg->AddEntry(hgeffpt,"Gluon","lpf");

  // Compare bits
  CreateCanvas("Vertex reco efficiency - bits",kFALSE);
  TH1D *h1 = (TH1D*)hfcont1->Project1D(AliHFJetsContainer::kCFStepMatchedAny, "kCFJetPt");
  hfcont1->ResetAxisStep("nRecoVtx", AliHFJetsContainer::kCFStepMatchedAny);
  TH1D *h1den = (TH1D*)hfcont1->Project1D(AliHFJetsContainer::kCFStepMatchedAny, "kCFJetPt");
  h1->Divide(h1,h1den,1,1,"B"); 
  Color(h1,1,20);
  h1->Rebin(rebin);
  h1->Scale(1./rebin);
  h1->GetYaxis()->SetTitle("Efficiency");
  h1->DrawCopy();
  TH1D *h2 = (TH1D*)hfcont2->Project1D(AliHFJetsContainer::kCFStepMatchedAny, "kCFJetPt");
  hfcont2->ResetAxisStep("nRecoVtx", AliHFJetsContainer::kCFStepMatchedAny);
  TH1D *h2den = (TH1D*)hfcont2->Project1D(AliHFJetsContainer::kCFStepMatchedAny, "kCFJetPt");
  h2->Divide(h2,h2den,1,1,"B"); 
  Color(h2,6,20);
  h2->Rebin(rebin);
  h2->Scale(1./rebin);
  h2->DrawCopy("same");
  TLegend *legbits = NewLegend("Filter-bit:");
  legbits->AddEntry(h1,"528","lpf");
  legbits->AddEntry(h2,"768","lpf");
  legbits->Draw("same");

  // Compare Flavours 
  CreateCanvas("Vertex reco efficiency",kFALSE);
  // Get efficiency for B
  hfcont1->SetAxisRangeStep(meth.Data(),4,4,AliHFJetsContainer::kCFStepMatchedB);// select B
  TH1D *hbnum = (TH1D*)hfcont1->Project1D(AliHFJetsContainer::kCFStepMatchedB, "kCFJetPt");
  hfcont1->ResetAxisStep("nRecoVtx", AliHFJetsContainer::kCFStepMatchedB);
  TH1D *hbden = (TH1D*)hfcont1->Project1D(AliHFJetsContainer::kCFStepMatchedB, "kCFJetPt");
  hbnum->Divide(hbnum,hbden,1,1,"B"); 
  Color(hbnum,1,20);
  hbnum->Rebin(rebin);
  hbnum->Scale(1./rebin);
  hbnum->GetYaxis()->SetTitle("Efficiency");
  hbnum->DrawCopy();
  // Get efficiency for C
  hfcont2->SetAxisRangeStep(meth.Data(),3,3,AliHFJetsContainer::kCFStepMatchedC);// select C
  TH1D *hcnum = (TH1D*)hfcont2->Project1D(AliHFJetsContainer::kCFStepMatchedC, "kCFJetPt");
  hfcont2->ResetAxisStep("nRecoVtx", AliHFJetsContainer::kCFStepMatchedC);
  TH1D *hcden = (TH1D*)hfcont2->Project1D(AliHFJetsContainer::kCFStepMatchedC, "kCFJetPt");
  hcnum->Divide(hcnum,hcden,1,1,"B"); 
  Color(hcnum,2,20);
  hcnum->Rebin(rebin);
  hcnum->Scale(1./rebin);
  hcnum->DrawCopy("same");
  // Get efficiency for light-quarks
  hfcont3->SetAxisRangeStep(meth.Data(),2,2,AliHFJetsContainer::kCFStepMatchedLight);// select Light
  TH1D *hlnum = (TH1D*)hfcont3->Project1D(AliHFJetsContainer::kCFStepMatchedLight, "kCFJetPt");
  hfcont3->ResetAxisStep("nRecoVtx", AliHFJetsContainer::kCFStepMatchedLight);
  TH1D *hlden = (TH1D*)hfcont3->Project1D(AliHFJetsContainer::kCFStepMatchedLight, "kCFJetPt");
  hlnum->Divide(hlnum,hlden,1,1,"B"); 
  Color(hlnum,3,20);
  hlnum->Rebin(rebin);
  hlnum->Scale(1./rebin);
  hlnum->DrawCopy("same");
  // Get efficiency for gluons
  hfcont3->SetAxisRangeStep(meth.Data(),1,1,AliHFJetsContainer::kCFStepMatchedGluon);// select Gluon
  TH1D *hgnum = (TH1D*)hfcont3->Project1D(AliHFJetsContainer::kCFStepMatchedGluon, "kCFJetPt");
  hfcont3->ResetAxisStep("nRecoVtx", AliHFJetsContainer::kCFStepMatchedGluon);
  TH1D *hgden = (TH1D*)hfcont3->Project1D(AliHFJetsContainer::kCFStepMatchedGluon, "kCFJetPt");
  hgnum->Divide(hgnum,hgden,1,1,"B"); 
  Color(hgnum,4,20);
  hgnum->Rebin(rebin);
  hgnum->Scale(1./rebin);
  hgnum->DrawCopy("same");
  
  leg->Draw("same");

}

