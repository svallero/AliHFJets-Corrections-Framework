#include "StyleSara.h"

class AliHFJetContainerVertex;

void PlotEfficiency(){
  
  // Load libraries before running this macro
  gROOT->Macro("LoadLibsSara.C");

  // Input file
  //TFile *f1 = new TFile("outputsLHC11a1/AnalysisResults_LHC11a1c.root","r");
  TFile* f2=new TFile("outputsLHC13d18d/AnalysisResults_NEW_TRC0_MM1_FB768_merged.root","r"); // good for matching eff.
  TFile* f1=new TFile("outputsLHC13d18d/AnalysisResults_NEW_TRC0_MM1_FB272_merged.root","r"); 
  //TFile* f1=new TFile("outputsLHC13d18d/AnalysisResults_NEW_TRC1_MM2_FB768.root","r"); // good for matching eff.
  //TFile* f2=new TFile("outputsLHC13d18d/AnalysisResults_NEW_TRC1_MM2_FB272.root","r"); 

  TList *dhf = (TList*)f1->Get("HFJets_containers");
  TList *dhf2 = (TList*)f2->Get("HFJets_containers");
  
  // Get the container
  //AliHFJetsContainerVertex *hfcont = (AliHFJetsContainerVertex*)dhf->FindObject("kBJets");
  //AliHFJetsContainerVertex *hfcont = (AliHFJetsContainerVertex*)dhf->FindObject("kJets");
  AliHFJetsContainerVertex *hfcont = (AliHFJetsContainerVertex*)dhf->FindObject("kJetVtx");
  AliHFJetsContainerVertex *hfcont2 = (AliHFJetsContainerVertex*)dhf2->FindObject("kJetVtx"); // bit 272

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
  //TString meth("partBH");
  
  // Rebin
  Int_t rebin= 2.; 

  // CUTS
  Float_t lxymin=0.06;
  Float_t massmin=2.;
  Float_t nvtxmin=1.;
  AliHFJetsContainer::CFSteps arr[5];
  arr[0]=AliHFJetsContainer::kCFStepMatchedB;
  arr[1]=AliHFJetsContainer::kCFStepMatchedC;
  arr[2]=AliHFJetsContainer::kCFStepMatchedLight;
  arr[3]=AliHFJetsContainer::kCFStepMatchedGluon;
  arr[4]=AliHFJetsContainer::kCFStepMatchedAny;
  for(Int_t i=0; i<5; i++){
     hfcont->SetAxisRangeStep("Lxy1",lxymin,-1,arr[i], kTRUE);
     hfcont->SetAxisRangeStep("mass1",massmin,-1,arr[i], kTRUE);
     hfcont->SetAxisRangeStep("nRecoVtx",nvtxmin,-1,arr[i], kTRUE);
     }
 


  // Dummy
  //CreateCanvas("Parton nature",kFALSE);
  //TH1D *pnat = (TH1D*)hfcont->Project1D(AliHFJetsContainer::kCFStepRecoB,"kCFJetEta"); 
  //hfcont->SetAxisRangeStep("kCFJetPt",10.,14.,AliHFJetsContainer::kCFStepVertex);
  //hfcont->SetAxisRangeStep("kCFJetPt",10.,14.,AliHFJetsContainer::kCFStepMatchedAny);
  //TH1D *pnatvertex = (TH1D*)hfcont->Project1D(AliHFJetsContainer::kCFStepVertex,meth.Data()); 
  //Color(pnatvertex,1);
  //pnatvertex->Draw();
  //TH1D *pnatany = (TH1D*)hfcont->Project1D(AliHFJetsContainer::kCFStepMatchedAny,meth.Data()); 
  //Color(pnatany,2);
  //pnatany->Draw("same");
  // Legend
  //TLegend *leg0 = NewLegend(Form("Bit 768 - Method %s - Step: ", meth.Data()));
  //leg0->AddEntry(pnatvertex,"Vertex","lpf");
  //leg0->AddEntry(pnatany,"MatchedAny","lpf");
  //leg0->Draw("same");

  //pnat->DrawCopy();

  // Matching efficiency (all flavours)
  CreateCanvas("Matching efficiency", kFALSE);
  TH1D *hmatcheffpt = (TH1D*)hfcont->GetMatchingEfficiencyPt(meth.Data());
  Color(hmatcheffpt,1,20);
  hmatcheffpt->DrawCopy();
  TH1D *hmatcheffpt2 = (TH1D*)hfcont2->GetMatchingEfficiencyPt(meth.Data());
  Color(hmatcheffpt2,2,20);
  hmatcheffpt2->DrawCopy("same");
  // Legend
  TLegend *leg1 = NewLegend("Filterbit: ");
  leg1->AddEntry(hmatcheffpt,"272","lpf");
  leg1->AddEntry(hmatcheffpt2,"768","lpf");
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
  //hmeffpt->DrawCopy("histo,same");
  //hgeffpt->DrawCopy();

  // Legend 
  TLegend *leg = NewLegend(Form("Method: %s - Jet flavour:",meth.Data()));
  leg->AddEntry(hbeffpt->Clone(),"Beauty","lpf");
  leg->AddEntry(hceffpt,"Charm","lpf");
  //leg->AddEntry(hleffpt,"Light + Gluon","lpf");
  leg->AddEntry(hleffpt,"Light","lpf");
  leg->AddEntry(hgeffpt,"Gluon","lpf");
  //leg->AddEntry(hmeffpt,"All","l");
  leg->Draw("same");

  //f->Close();
  //delete hfcont;
}
