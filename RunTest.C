void RunTest()
{

  // Load configuration
  gROOT->LoadMacro("TestConfiguration.C");

  // Set paths
  gSystem->SetIncludePath(includepath.Data()); 
  gSystem->AddDynamicPath(local_fastjet_path.Data()); 

  // Load libraries locally 
  TString loadLibraries="LoadLibraries.C"; loadLibraries.Prepend(loadMacroPath.Data());
  gROOT->LoadMacro(loadLibraries.Data());
  LoadLibraries(kFALSE);
  // for FastJet
  TObjArray *arr;
  TObjString *objstr;
  arr = loaddylibs.Tokenize(" ");
  TIter next(arr);
  while ((objstr=(TObjString*)next())){
    TString str=(objstr->GetString()).Append(".dylib");
    Printf("Loading %s", str.Data());
    gSystem->Load(str.Data());
  }
  // standard AliRoot
  arr = loadlibs.Tokenize(" ");
  TIter next(arr);
  while ((objstr=(TObjString*)next())){
    TString str=objstr->GetString();
    Printf("Loading %s", str.Data());
    gSystem->Load(str.Data());
  }

  // Load analysis sources locally
  arr = sources.Tokenize(" ");
  TIter next(arr);
  while ((objstr=(TObjString*)next())){
    TString str=(objstr->GetString()).Append(".cxx++g");
    gROOT->LoadMacro(str.Data());
    //TString str2=(objstr->GetString()).Append("_cxx.so");
    //Printf("Loading %s", str2.Data());
    //gROOT->LoadClass(str2.Data());
  }

  if (0){
    const Int_t nvars = 3;
    Int_t nbins[nvars]={3,3,3};
    Double_t *binning[nvars];
    Double_t bins1[nvars+1]={0.,1.,2.,3.};
    Double_t bins2[nvars+1]={0.,1.,2.,3.};
    Double_t bins3[nvars+1]={0.,1.,2.,3.};
    binning[0]=bins1;
    binning[1]=bins2;
    binning[2]=bins3;
    const char* axistitle[nvars];
    axistitle[0]="First axis";
    axistitle[1]="Second axis";
    axistitle[2]="Third axis";
    const char* axisname[nvars];
    axisname[0]="axis1";
    axisname[1]="axis2";
    axisname[2]="axis3";

    AliHFJetContainer *cont= new AliHFJetContainer("Dummy",nvars, axisname, nbins, binning, axistitle);
    //Double_t point[7]={5.,7., 0.2, 0.2, 1.5,1.5,1.5}; 
    //cont->Fill(point);
    //cont->PrintSteps();
    //cont->PrintVars();
    //cont->SetAxisRangeStep("kCFJetPt", 10., 20., AliHFJetContainer::kCFStepAll);
    cont->SetAxisRangeAllSteps("kCFJetPt", 10., 20.);
    //cont->ResetAxisStep("axis1", AliHFJetContainer::kCFStepAll);
    //cont->ResetAxisAllSteps("axis1");
    //TH1D *h = cont->Project1D(AliHFJetContainer::kCFStepAll, "axis1");
    //h->DrawCopy();
    
    TH2D *heff=cont->GetEfficiency2D();
    new TCanvas();
    heff->DrawCopy();
  } else if (1){  
    //AliHFJetContainer *cont= new AliHFJetContainer("Dummy");
    AliHFJetsContainerVertex *cont1= new AliHFJetsContainerVertex("cont1",AliHFJetsContainerVertex::kJets);
    AliHFJetsContainerVertex *cont2= new AliHFJetsContainerVertex("cont2",AliHFJetsContainerVertex::kBJets);
    Double_t point[13]={1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.};
    TArrayD *apoint = new TArrayD(13,point);
    cont2->FillStep(0,apoint);
    //new TCanvas();
    //cont->ResetAxisStep(AliHFJetContainer::kCFMult, AliHFJetContainer::kCFStepAll);
    TH1D *h2 = cont2->Project1D(AliHFJetsContainer::kCFStepAll, "kCFMult");
    h2->DrawCopy();

    TFile *f = new TFile("file.root","RECREATE");
    cont2->Write();
    f->Close();
    // Vecchia versione
    //AliCFContainer *cf2 =(AliCFContainer*)cont->GetContainerPrimVtx();
    //Double_t point[13]={0.5,0.5,0.5,3,0,1,2,3,0.5,0.1,0.1,0.1,0.5};
    //if (cf1)
    //  cf1->Fill(point,0);
    //else Printf("cf1 does not exist!!!");
    //TFile *f=new TFile("tmp.root","RECREATE");
    //cf->Write();
    //f->Close();
  }
  return;
}



