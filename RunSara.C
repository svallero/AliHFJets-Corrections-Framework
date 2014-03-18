class AliAnalysisGrid;
class AliAnalysisAlien;

void RunSara()
{

  // This macro is to run "local" or "grid"
  // To run with PROOF see RunVaf.C

  TString mode = "grid";
  //TString mode = "local";

  // Load configuration
  gROOT->LoadMacro("Configuration.C");
  //gROOT->LoadMacro("ConfigurationVAF.C");

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

  // Create AnalysisManager
  AliAnalysisManager *mgr  = new AliAnalysisManager("My Manager","My Manager");
  mgr->SetDebugLevel(10);


  // Create AlienHandler (if "grid")
  AliAnalysisGrid *alienHandler;
  if (mode == "grid"){
    alienHandler = CreateAlienHandler();  
    if(!alienHandler) return;
    mgr->SetGridHandler(alienHandler);
  }
  
  // Prepare local input (if "local")
  TChain *chainAOD = 0;
  if (mode == "local"){
     TString makeAODInputChain="MakeAODInputChainSara.C"; 
     gROOT->LoadMacro(makeAODInputChain.Data());
     // with this it reads ./AliAOD.root and ./AliAOD.Jets.root
     chainAOD = CreateChain("List.txt");
     printf("ENTRIES %d\n",chainAOD->GetEntries());
  } 

  //Cretae InputHandler 
  AliAODInputHandler *inputHandler = new AliAODInputHandler("handler","handler for HFCJ");
  mgr->SetInputEventHandler(inputHandler);
  
  // AOD handler
  //AliAODHandler* aodHandler   = new AliAODHandler();
  //aodHandler->SetOutputFileName("AliAOD.root");
  //mgr->SetOutputEventHandler(aodHandler);
  //mgr->GetCommonInputContainer()->SetSpecialOutput();

  // Load analysis sources locally
  arr = sources.Tokenize(" ");
  TIter next(arr);
  while ((objstr=(TObjString*)next())){
    TString str=(objstr->GetString()).Append(".cxx++g");
    gROOT->LoadMacro(str.Data());
  }

  // Add Tasks
  AddTasks();  

  if(!mgr->InitAnalysis()) return;
  mgr->PrintStatus();
  mgr->StartAnalysis(mode, chainAOD,10);
  //mgr->StartAnalysis(mode, chainAOD);
  return;
}



AliAnalysisGrid* CreateAlienHandler(){

  AliAnalysisAlien *plugin = new AliAnalysisAlien();
  plugin->SetRunMode(pluginmode.Data());
  plugin->SetUser(user.Data());

  // General configuration
  plugin->SetNtestFiles(3);
  plugin->SetAnalysisMacro(Form("%s.C",name.Data()));
  plugin->SetJDLName(Form("%s.jdl",name.Data()));
  plugin->SetExecutable(Form("%s.sh",name.Data()));
  plugin->SetGridWorkingDir(name.Data());
  // outputs
  plugin->SetGridOutputDir("output");
  plugin->SetDefaultOutputs(kTRUE);
  // packages
  plugin->SetAPIVersion(api.Data());
  plugin->SetROOTVersion(root.Data());
  plugin->SetAliROOTVersion(aliroot.Data());
  plugin->AddExternalPackage(boost.Data());
  plugin->AddExternalPackage(cgal.Data());
  plugin->AddExternalPackage(fastjet.Data());

  // Data samples
  TString reducedpath(loadMacroPath.Data());
  reducedpath.ReplaceAll("/macros", "");
  gROOT->LoadMacro(Form("%s/AddGoodRuns.C",reducedpath.Data()));
  plugin->SetGridDataDir(datadir.Data());
  plugin->SetDataPattern(datapattern.Data()); // specify AOD set
  TObjArray *arr;
  TObjString *objstr;
  arr = runs.Tokenize(" ");
  TIter next(arr);
  while ((objstr=(TObjString*)next())){
    plugin->AddRunNumber((objstr->GetString()).Data());  
  }

  // Analysis sources
  TString allsources=" ";
  TString allheaders=" ";
  TString strcc, strh;
  arr = sources.Tokenize(" ");
  TIter next(arr);
  while ((objstr=(TObjString*)next())){
    strcc=(objstr->GetString()).Append(".cxx ");
    strh=(objstr->GetString()).Append(".h ");
    allsources.Append(strcc.Data());
    allheaders.Append(strh.Data());
  }
  plugin->SetAnalysisSource(allsources.Data());

  // Additional libraries
  TString addlibs=" ";
  arr = loaddylibs.Tokenize(" ");
  TIter next(arr);
  while ((objstr=(TObjString*)next())){
    TString str;
    if (pluginmode == "test") str.Form("%s.dylib ",(objstr->GetString()).Data());
    else str.Form("%s.so ",(objstr->GetString()).Data()); 
    str.Form("%s.so ",(objstr->GetString()).Data()); 
    addlibs.Append(str.Data()); 
  }
  arr = loadlibs.Tokenize(" ");
  TIter next(arr);
  while ((objstr=(TObjString*)next())){
    TString str;
    str.Form("%s.so ",(objstr->GetString()).Data()); 
    addlibs.Append(str.Data()); 
  }
  addlibs.Append(Form("%s %s", allheaders.Data(), allsources.Data() ));
  plugin->SetAdditionalLibs(addlibs.Data());

  // Include path
  plugin->AddIncludePath(includepath.Data());

  // Merging
  plugin->SetMergeViaJDL(kTRUE); 
  plugin->SetOneStageMerging(kFALSE);
  plugin->SetMaxMergeStages(2);
  plugin->SetSplitMaxInputFileNumber(30);


  return plugin;
}
