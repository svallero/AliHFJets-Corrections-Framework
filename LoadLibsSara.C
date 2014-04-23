void LoadLibsSara()
{

  // Load configuration
  gROOT->LoadMacro("ConfigurationVAF.C");


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
    //TString str=(objstr->GetString()).Append(".cxx++g");
    TString str=(objstr->GetString()).Append(".cxx+");
    gROOT->LoadMacro(str.Data());
  }

  return;
}



