TString loadMacroPath="$ALICE_ROOT/PWGHF/vertexingHF/macros/";

// --- include path --------------- 
TString includepath=("-I. -I$ROOTSYS/include -I$ALICE_ROOT -I$ALICE_ROOT/include -I$ALICE_ROOT/ITS -I$ALICE_ROOT/TPC -I$ALICE_ROOT/CONTAINERS -I$ALICE_ROOT/STEER/STEER -I$ALICE_ROOT/STEER/STEERBase -I$ALICE_ROOT/STEER/ESD -I$ALICE_ROOT/STEER/AOD -I$ALICE_ROOT/TRD -I$ALICE_ROOT/macros -I$ALICE_ROOT/ANALYSIS  -I$ALICE_ROOT/OADB -I$ALICE_ROOT/PWGHF -I$ALICE_ROOT/PWGHF/base -I$ALICE_ROOT/PWGHF/vertexingHF -I$ALICE_ROOT/PWG/FLOW/Base -I$ALICE_ROOT/PWG/FLOW/Tasks -I$ALICE_ROOT/JETAN/fastjet -I$ALICE_ROOT/JETAN/fastjet/fastjet -I$ALICE_ROOT/JETAN -I$ALICE_ROOT/PWG/Tools -g"); 

// --- libraries ------------------
TString local_fastjet_path="/usr/local/lib";
//TString loaddylibs=("libCGAL libfastjet libsiscone libsiscone_spherical libfastjetplugins");
TString loaddylibs=("");
//TString loadlibs=("libJETAN libFASTJETAN libPWGTools libPWGflowBase libPWGflowTasks libPWGmuon libPWGHFbase libPWGHFvertexingHF libFASTJETAN libPWGJE");
TString loadlibs=("libJETAN  libPWGTools libPWGflowBase libPWGflowTasks libPWGmuon libPWGHFbase libPWGHFvertexingHF");


// --- analysis sources -----------
//TString sources=("AliHFJetsContainer AliHFJetsContainerVertex");
//TString sources=("AliRDHFJetsCuts AliRDHFJetsCutsVertex AliHFJetsTagging AliHFJetsTaggingVertex");
//TString sources=("AliRDHFJetsCuts AliRDHFJetsCutsVertex AliHFJetsTagging AliHFJetsTaggingVertex AliHFJetsContainer AliHFJetsContainerVertex AliAnalysisTaskSEHFJets");
TString sources=("AliRDHFJetsCuts AliRDHFJetsCutsVertex AliHFJetsTagging AliHFJetsTaggingVertex AliHFJetsContainer AliHFJetsContainerVertex AliAnalysisTaskSEHFJets");

