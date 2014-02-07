TString trainName = "HFCJ";
Long64_t nentries=123567890,firstentry=0;
TString loadMacroPath="$ALICE_ROOT/PWGHF/vertexingHF/macros/";

// GRID ---------------------------
TString user="svallero";
TString pluginmode="test";
TString name="AnalysisBjets";
// packages
TString api="V1.1x";
TString root="v5-34-11";
TString aliroot="v5-05-45-AN";
TString boost="boost::v1_51_0";
TString cgal="cgal::v4.0.2";
TString fastjet="fastjet::v3.0.3";
// data
TString datadir="/alice/sim/2013/LHC13d18d";
TString datapattern="AOD/*AliAOD.root";
TString runs="126082";


// --- include path --------------- 
TString includepath=("-I. -I$ROOTSYS/include -I$ALICE_ROOT -I$ALICE_ROOT/include -I$ALICE_ROOT/ITS -I$ALICE_ROOT/TPC -I$ALICE_ROOT/CONTAINERS -I$ALICE_ROOT/STEER/STEER -I$ALICE_ROOT/STEER/STEERBase -I$ALICE_ROOT/STEER/ESD -I$ALICE_ROOT/STEER/AOD -I$ALICE_ROOT/TRD -I$ALICE_ROOT/macros -I$ALICE_ROOT/ANALYSIS  -I$ALICE_ROOT/OADB -I$ALICE_ROOT/PWGHF -I$ALICE_ROOT/PWGHF/base -I$ALICE_ROOT/PWGHF/vertexingHF -I$ALICE_ROOT/PWG/FLOW/Base -I$ALICE_ROOT/PWG/FLOW/Tasks -I$ALICE_ROOT/JETAN/fastjet -I$ALICE_ROOT/JETAN/fastjet/fastjet -I$ALICE_ROOT/JETAN -I$ALICE_ROOT/PWG/Tools -g"); 

// --- libraries ------------------
TString local_fastjet_path="/usr/local/lib";
// patch for VAF environment
//TString loaddylibs=("libCGAL libfastjet libsiscone libsiscone_spherical libfastjetplugins");
TString loaddylibs=("");
//TString loadlibs=("libJETAN libFASTJETAN libPWGTools libPWGflowBase libPWGflowTasks libPWGmuon libPWGHFbase libPWGHFvertexingHF libFASTJETAN libPWGJE");
TString loadlibs=("libCGAL libfastjet libsiscone libsiscone_spherical libSISConePlugin libJETAN libFASTJETAN libPWGTools libPWGflowBase libPWGflowTasks libPWGmuon libPWGHFbase libPWGHFvertexingHF libFASTJETAN libPWGJE");

// --- analysis sources -----------
//TString sources=("AliHFJetContainer AliRDHFJetsCuts AliRDHFJetsCutsVertex AliHFJetVertexTagging AliAnalysisTaskSEHFJets");

TString sources=("AliRDHFJetsCuts AliRDHFJetsCutsVertex AliHFJetsTagging AliHFJetsTaggingVertex AliHFJetsContainer AliHFJetsContainerVertex AliAnalysisTaskSEHFJets");

void AddTasks(){

  // JetFinder 
  Int_t kHighPtFilterMask = 272;
  UInt_t iPhysicsSelectionFlag = AliVEvent::kAny; 
  Float_t fTrackEtaWindow = 0.9;  

  gROOT->LoadMacro("AddTaskJetCluster.C");
  AliAnalysisTaskJetCluster *taskCl = 0;
  taskCl = AddTaskJetCluster("AOD","",kHighPtFilterMask,iPhysicsSelectionFlag,"ANTIKT",0.4,0,kTRUE,"",0.15,fTrackEtaWindow);
  // MC charged with full eta window (5)
  taskCl = AddTaskJetCluster("AODMC2","",kHighPtFilterMask,iPhysicsSelectionFlag,"ANTIKT",0.4,0,kTRUE,"",0.15,fTrackEtaWindow);
  // MC charged with restricted eta window
  taskCl = AddTaskJetCluster("AODMC2b","",kHighPtFilterMask,iPhysicsSelectionFlag,"ANTIKT",0.4,0,kTRUE,"",0.15,fTrackEtaWindow);

  // BTagging 
  gROOT->LoadMacro("AddTaskSEHFJets.C");
  AliAnalysisTaskSEHFJets* taskJet=AddTaskSEHFJets("standard",2,"","c",0);
  // Do not cut on max centrality
  //AliAnalysisTaskSEHFJets* taskJet=AddTaskSEHFJets("standard",2,"","c",0, 1000);


}
