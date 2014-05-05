#include "StyleSara.h"

class AliHFJetContainerVertex;

void ReadResults(){
  
  // Load libraries before running this macro
  gROOT->Macro("LoadLibsSara.C");

  // Input file
  TFile *f1 = new TFile("outputsLHC11a1/AnalysisResults_LHC11a1b.root","r");
  //TFile *f1 = new TFile("AnalysisResults.root","r");
  TList *dhf = (TList*)f1->Get("HFJets_containers");
  
  // Get the container
  AliHFJetsContainerVertex *hfcont = (AliHFJetsContainerVertex*)dhf->FindObject("kJetVtx");

  // To check available variables/steps for the container
  //hfcont->PrintVars();
  //hfcont->PrintSteps();   
   
  // Dummy
  CreateCanvas("Dummy",kFALSE);
  TH1D *dummy = (TH1D*)hfcont->Project1D(AliHFJetsContainer::kCFStepVertex,"kCFJetPt"); 
  dummy->Draw();

}
