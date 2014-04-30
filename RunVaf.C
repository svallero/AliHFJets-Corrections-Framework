void RunVaf(){
  
  // Local mode is needed to debug and run valgrind
  //const char *mode="local"; // can be "proof" or "local"
  const char *mode="proof"; // can be "proof" or "local"
  
  // List of AliRoot parameters
  TList *list = new TList();
  list->Add(new TNamed("ALIROOT_MODE", "AliRoot"));

  // Here extra AliRoot libs
  TString alilibs("STEERBase:ESD:AOD:ANALYSIS:OADB:ANALYSISalice:CORRFW:PWGTools:PWGLFforward2:PWGHFbase:PWGflowBase:PWGflowTasks:PWGHFvertexingHF:JETAN:SISConePlugin:FASTJETAN:PWGJE");
  // Here FASTJET stuff
  TString fastjetlibs("CGAL:fastjet:siscone:siscone_spherical");

  TString libs("");
  libs.Form("%s:%s",fastjetlibs.Data(), alilibs.Data());
  list->Add(new TNamed("ALIROOT_EXTRA_LIBS", libs.Data()));
  
  // Here extra include path
  TString includepath=("ITS:TPC:CONTAINERS:STEER/STEER:STEER/STEERBase:STEER/ESD:STEER/AOD:TRD:macros:ANALYSIS:OADB:PWGHF:PWGHF/base:PWGHF/vertexingHF:PWG/FLOW/Base:PWG/FLOW/Tasks:JETAN/fastjet:JETAN/fastjet/fastjet:JETAN:PWG/Tools:PWGLF/FORWARD/analysis2"); 
  list->Add(new TNamed("ALIROOT_EXTRA_INCLUDES", includepath.Data()));
  
  // Define dataset
  // just one run
  //TString dataset=("Find;BasePath=/alice/sim/2013/LHC13d18d/126082;FileName=AOD/*/AliAOD.root");
  // all runs
  TString dataset=("Find;BasePath=/alice/sim/2013/LHC13d18d/;FileName=AOD/*/AliAOD.root");

  TChain *chainAOD = 0;
  switch (mode){
     case "proof": 
        // Open PROOF connection
        TProof::Open("pod://");
        // Upload and enable package
        gProof->UploadPackage("AliRoot.par");
        gProof->EnablePackage("AliRoot.par", list);
  	// Check the dataset before running the analysis!
  	gProof->ShowDataSet( dataset.Data() );  
        break;
     case "local":
        TProof *gProof=NULL;
        // Setup package
        gROOT->LoadMacro("SetupPar.C");
        setupPar("AliRoot", list);
        // Prepare local input
        TString makeAODInputChain="MakeAODInputChain.C"; 
        gROOT->LoadMacro(makeAODInputChain.Data());
        chainAOD = CreateChain("List.txt");
	printf("ENTRIES %d\n",chainAOD->GetEntries());
        break; 
     default:
        Printf("ERROR: invalid analysis mode!"); 
        Printf("Supported modes are 'local' and 'proof'"); 
        return;
  }
  

  // Create AnalysisManager
  AliAnalysisManager *mgr  = new AliAnalysisManager("My Manager","My Manager");
  mgr->SetDebugLevel(10);
 
  //Create InputHandler 
  AliAODInputHandler *inputHandler = new AliAODInputHandler("handler","handler for HFCJ");
  mgr->SetInputEventHandler(inputHandler);
  
  // Load analysis sources
  TString sources=("AliRDHFJetsCuts AliRDHFJetsCutsVertex AliHFJetsTagging AliHFJetsTaggingVertex AliHFJetsContainer AliHFJetsContainerVertex AliAnalysisTaskSEHFJets"); 
  //TString sources=("AliRDHFJetsCuts AliRDHFJetsCutsVertex AliHFJetTagging AliHFJetVertexTagging AliAnalysisTaskSEHFJetsOrig");
  arr = sources.Tokenize(" ");
  TIter next(arr);
  while ((objstr=(TObjString*)next())){
    TString str=(objstr->GetString()).Append(".cxx++g");
    if (mode == "local") gROOT->LoadMacro(str.Data());
    else gProof->Load(str.Data());
  }
  
 
  // Add tasks
  AddTaskJetFinder();
  AddTaskBJets(); 

  // Start analysis
  Int_t nentries=9999999999999;
  //Int_t nentries=100;
  Int_t firstentry=0;
  if(!mgr->InitAnalysis()) return;
  mgr->PrintStatus();
  if (mode == "local") mgr->StartAnalysis("local", chainAOD, nentries);
  else mgr->StartAnalysis("proof",dataset.Data(),nentries,firstentry);
  //else mgr->StartAnalysis("proof",dataset.Data());
  return;

}

//------------------------------------------------------------------
void AddTaskJetFinder(){

  Int_t kHighPtFilterMask = 272;
  //Int_t kHighPtFilterMask = 768;
  UInt_t iPhysicsSelectionFlag = AliVEvent::kAny;
  Float_t fTrackEtaWindow = 0.9;
  gROOT->LoadMacro("AddTaskJetCluster.C");
  AliAnalysisTaskJetCluster *taskCl = 0;
  taskCl = AddTaskJetCluster("AOD","",kHighPtFilterMask,iPhysicsSelectionFlag,"ANTIKT",0.4,0,kTRUE,"",0.15,fTrackEtaWindow);
  // MC charged with full eta window (5)
  taskCl = AddTaskJetCluster("AODMC2","",kHighPtFilterMask,iPhysicsSelectionFlag,"ANTIKT",0.4,0,kTRUE,"",0.15,fTrackEtaWindow);
  // MC charged with restricted eta window
  //taskCl = AddTaskJetCluster("AODMC2b","",kHighPtFilterMask,iPhysicsSelectionFlag,"ANTIKT",0.3,0,kTRUE,"",0.15,fTrackEtaWindow);

}

//------------------------------------------------------------------
void AddTaskBJets(){

  gROOT->LoadMacro("AddTaskSEHFJets.C");
  //gROOT->LoadMacro("AddTaskSEHFJetsOrig.C");
  AliAnalysisTaskSEHFJets* taskJet=AddTaskSEHFJets("standard",2,"","c",1000);
  //AliAnalysisTaskSEHFJets* taskJet=AddTaskSEHFJets("standard",2,"","c",0);
  //AliAnalysisTaskSEHFJetsOrig* taskJet=AddTaskSEHFJetsOrig("standard",2,"","c",1000);
  // Do not cut on max centrality
  //AliAnalysisTaskSEHFJets* taskJet=AddTaskSEHFJets("standard",2,"","c",0, 1000);

}
