void RunVaf(){

  // List of AliRoot parameters
  TList *list = new TList();
  list->Add(new TNamed("ALIROOT_MODE", "AliRoot"));

  // Here extra AliRoot libs
  TString alilibs("STEERBase:ESD:AOD:ANALYSIS:OADB:ANALYSISalice:CORRFW:PWGLFforward2:PWGHFbase:PWGflowBase:PWGflowTasks:PWGHFvertexingHF:JETAN:SISConePlugin:FASTJETAN:PWGJE");
  // Here FASTJET stuff
  TString fastjetlibs("CGAL:fastjet:siscone:siscone_spherical");

  TString libs("");
  libs.Form("%s:%s",fastjetlibs.Data(), alilibs.Data());
  list->Add(new TNamed("ALIROOT_EXTRA_LIBS", libs.Data()));
  
  // Here extra include path
  TString includepath=("ITS:TPC:CONTAINERS:STEER/STEER:STEER/STEERBase:STEER/ESD:STEER/AOD:TRD:macros:ANALYSIS:OADB:PWGHF:PWGHF/base:PWGHF/vertexingHF:PWG/FLOW/Base:PWG/FLOW/Tasks:JETAN/fastjet:JETAN/fastjet/fastjet:JETAN:PWG/Tools:PWGLF/FORWARD/analysis2"); 
  list->Add(new TNamed("ALIROOT_EXTRA_INCLUDES", includepath.Data()));

  // Open PROOF connection
  TProof::Open("pod://");
  
  // Upload and enable package
  gProof->UploadPackage("AliRoot.par");
  gProof->EnablePackage("AliRoot.par", list);

  // Define dataset
  TString dataset=("Find;BasePath=/alice/sim/2013/LHC13d18d/126082;FileName=AOD/*/AliAOD.root");
  
  
  // Create AnalysisManager
  AliAnalysisManager *mgr  = new AliAnalysisManager("My Manager","My Manager");
  mgr->SetDebugLevel(10);
 
  //Create InputHandler 
  AliAODInputHandler *inputHandler = new AliAODInputHandler("handler","handler for HFCJ");
  mgr->SetInputEventHandler(inputHandler);
  
  // Load analysis sources
  TString sources=("AliRDHFJetsCuts AliRDHFJetsCutsVertex AliHFJetsTagging AliHFJetsTaggingVertex AliHFJetsContainer AliHFJetsContainerVertex AliAnalysisTaskSEHFJets"); 
  arr = sources.Tokenize(" ");
  TIter next(arr);
  while ((objstr=(TObjString*)next())){
    TString str=(objstr->GetString()).Append(".cxx++g");
    //gROOT->LoadMacro(str.Data());
    gProof->Load(str.Data());
  }
  
 
  // Add tasks
  AddTaskJetFinder();
  //AddTaskBJets(); 

  // Start analysis
  Int_t nentries=100;
  Int_t firstentry=0;
  if(!mgr->InitAnalysis()) return;
  mgr->PrintStatus();
  //mgr->StartAnalysis("proof",dataset.Data(),nentries,firstentry);
  mgr->StartAnalysis("proof",dataset.Data());
  return;

}

//------------------------------------------------------------------
void AddTaskJetFinder(){

  Int_t kHighPtFilterMask = 272;
  UInt_t iPhysicsSelectionFlag = AliVEvent::kAny;
  Float_t fTrackEtaWindow = 0.9;

  gROOT->LoadMacro("AddTaskJetCluster.C");
  AliAnalysisTaskJetCluster *taskCl = 0;
  taskCl = AddTaskJetCluster("AOD","",kHighPtFilterMask,iPhysicsSelectionFlag,"ANTIKT",0.4,0,kTRUE,"",0.15,fTrackEtaWindow);
  // MC charged with full eta window (5)
  taskCl = AddTaskJetCluster("AODMC2","",kHighPtFilterMask,iPhysicsSelectionFlag,"ANTIKT",0.4,0,kTRUE,"",0.15,fTrackEtaWindow);
  // MC charged with restricted eta window
  //taskCl = AddTaskJetCluster("AODMC2b","",kHighPtFilterMask,iPhysicsSelectionFlag,"ANTIKT",0.4,0,kTRUE,"",0.15,fTrackEtaWindow);

}

//------------------------------------------------------------------
void AddTaskBJets(){

  gROOT->LoadMacro("AddTaskSEHFJets.C");
  AliAnalysisTaskSEHFJets* taskJet=AddTaskSEHFJets("standard",2,"","c",0);
  // Do not cut on max centrality
  //AliAnalysisTaskSEHFJets* taskJet=AddTaskSEHFJets("standard",2,"","c",0, 1000);

}
