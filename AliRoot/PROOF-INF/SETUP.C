/*
 * SETUP.C -- by Dario Berzano <dario.berzano@cern.ch>
 *
 * Configures environment for a certain AliRoot version, on client, master and
 * slaves.
 *
 * Usage of PARfiles is *required* with Dynamic Workers, which do not support
 * the replay of gProof->Exec() statements.
 *
 * List of variables supported for AAF "compatibility":
 *
 * [X] ALIROOT_EXTRA_INCLUDES
 * [X] ALIROOT_MODE
 * [X] ALIROOT_EXTRA_LIBS
 * [X] ALIROOT_ENABLE_ALIEN
 * [ ] ALIROOT_AAF_BAD_WORKER
 * [ ] ALIROOT_AAF_DEBUG
 * [ ] ALICE_PROOF_AAF_ALIEN_PACKAGES
 * [ ] ALIROOT_ALIEN_RETRY
 *
 */

#if !defined(__CINT__) || defined (__MAKECINT__)
#include <TError.h>
#include <TSystem.h>
#include <Riostream.h>
#include <TROOT.h>
#include <TList.h>
#include <TProof.h>
#include <TPRegexp.h>
#include <TObjString.h>
#include <TGrid.h>
#endif

TString gMessTag;

//______________________________________________________________________________
Bool_t SETUP_LoadLibraries(const TString &libs) {

  // Loads a list of colon-separated libraries. Returns kTRUE on success, kFALSE
  // if at least one library couldn't load properly. Does not check for double
  // loads (but ROOT does).

  TString l;
  Ssiz_t from;

  while ( libs.Tokenize(l, from, ":") ) {
    if (l.IsNull()) continue;
    if (!l.BeginsWith("lib")) l.Prepend("lib");
    if (l.EndsWith(".so")) l.Remove(l.Length()-3, l.Length());

    ::Info(gMessTag.Data(), ">> Loading library %s...", l.Data());

    if (gSystem->Load(l.Data()) < 0) {
       ::Error(gMessTag.Data(), "Error loading %s, aborting", l.Data());
       return kFALSE;  // failure
    }
  }

  return kTRUE;  // success

  return 0;
}

//______________________________________________________________________________
Bool_t SETUP_SetAliRootMode(TString &mode, const TString &extraLibs) {

  // Sets a certain AliRoot mode, defining a set of libraries to load. Extra
  // libraries to load can be specified as well. Returns kTRUE on success, or
  // kFALSE in case library loading failed.

  mode.ToLower();
  TString libs = extraLibs;
  Long_t rv = -9999;

  // Load needed ROOT libraries
  if (!SETUP_LoadLibraries("VMC:Tree:Physics:Matrix:Minuit:XMLParser:Gui")) {
    ::Error(gMessTag.Data(), "Loading of extra ROOT libraries failed");
    return kFALSE;
  }

  if (mode == "aliroot") {
    ::Info(gMessTag.Data(), "Loading libraries for AliRoot mode...");
    rv = gROOT->Macro(
      gSystem->ExpandPathName("$ALICE_ROOT/macros/loadlibs.C") );
  }
  else if (mode == "sim") {
    ::Info(gMessTag.Data(), "Loading libraries for simulation mode...");
    rv = gROOT->Macro(
      gSystem->ExpandPathName("$ALICE_ROOT/macros/loadlibssim.C") );
  }
  else if (mode == "rec") {
    ::Info(gMessTag.Data(), "Loading libraries for reconstruction mode...");
    rv = gROOT->Macro(
      gSystem->ExpandPathName("$ALICE_ROOT/macros/loadlibsrec.C") );
  }
  else {
    // No mode specified, or invalid mode: load standard libraries, and also
    // fix loading order
    TPMERegexp reLibs("(ANALYSISalice|OADB|ANALYSIS|STEERBase|ESD|AOD)(:|$)");
    while (reLibs.Substitute(libs, "")) {}
    libs.Prepend("STEERBase:ESD:AOD:ANALYSIS:OADB:ANALYSISalice:");
  }

  // Check status code
  if (rv == 0) {
    ::Info(gMessTag.Data(), "Successfully loaded AliRoot base libraries");
  }
  else if (rv != -9999) {
    ::Error(gMessTag.Data(), "Loading of base AliRoot libraries failed");
    return kFALSE;
  }

  // Load extra AliRoot libraries
  ::Info(gMessTag.Data(), "Loading extra AliRoot libraries...");
  if (!SETUP_LoadLibraries(libs)) {
    ::Error(gMessTag.Data(), "Loading of extra AliRoot libraries failed");
    return kFALSE;
  }
  else {
    ::Info(gMessTag.Data(), "Successfully loaded extra AliRoot libraries");
  }

  return kTRUE;
}

//______________________________________________________________________________
void SETUP_MakePar() {

  TString tmp = gSystem->GetFromPipe("mktemp -d /tmp/AliRoot-MakePar-XXXXX");
  TString buf;
  buf.Form("rm -rf AliRoot.par");
  buf.Form("%s/AliRoot/PROOF-INF", tmp.Data());
  gSystem->mkdir(buf.Data(), kTRUE);
  buf.Form("%s/AliRoot/PROOF-INF/SETUP.C", tmp.Data());
  gSystem->CopyFile("AliRoot_SETUP.C", buf.Data());
  buf.Form("tar -C %s -cvzf AliRoot.par AliRoot/", tmp.Data());
  gSystem->Exec(buf.Data());
  buf.Form("rm -rf %s", tmp.Data());
  gSystem->Exec(buf.Data());
  if (gSystem->AccessPathName("AliRoot.par") == kFALSE) {
    ::Info(gSystem->HostName(), "AliRoot.par created successfully");
  }
  else {
    ::Error(gSystem->HostName(), "Problems creating AliRoot.par");
  }

}

//______________________________________________________________________________
Int_t SETUP(TList *inputList = NULL) {

  TString aliRootDir;

  if (gProof && !gProof->IsMaster()) {
    // On client
    gMessTag = "Client";
  }
  else {
    // Master/workers
    gMessTag = gSystem->HostName();
  }

  aliRootDir = gSystem->Getenv("ALICE_ROOT");  // NULL --> ""

  if (aliRootDir.IsNull()) {
    ::Error(gMessTag.Data(),
      "ALICE_ROOT environment variable not defined!");
    return -1;
  }

  ::Info(gMessTag.Data(), "Enabling AliRoot located at %s",
    aliRootDir.Data());

  //
  // Common operations on PROOF Client and Master/Workers
  //

  // AliRoot include path
  gSystem->AddIncludePath(Form("-I\"%s/include\"", aliRootDir.Data()));

  // AliRoot macro path
  gROOT->SetMacroPath(
    Form("%s:%s/macros", gROOT->GetMacroPath(), aliRootDir.Data()) );

  //
  // Process input parameters
  //

  TString extraIncs, extraLibs, mode;
  Bool_t enableAliEn = kFALSE;

  if (inputList) {
    TIter it(inputList);
    TNamed *pair;
    while ((pair = dynamic_cast<TNamed *>(it.Next()))) {
      if ( strcmp(pair->GetName(), "ALIROOT_EXTRA_INCLUDES") == 0 )
        extraIncs = pair->GetTitle();
      else if ( strcmp(pair->GetName(), "ALIROOT_EXTRA_LIBS") == 0 )
        extraLibs = pair->GetTitle();
      else if ( strcmp(pair->GetName(), "ALIROOT_ENABLE_ALIEN") == 0 )
        enableAliEn = ( *(pair->GetTitle()) != '\0' );
      else if ( strcmp(pair->GetName(), "ALIROOT_MODE") == 0 )
        mode = pair->GetTitle();
    }
  }

  //
  // Load extra libraries and set AliRoot mode
  //

  if (!SETUP_SetAliRootMode(mode, extraLibs)) {
    ::Error(gMessTag.Data(),
      "Error loading libraries while setting AliRoot mode.");
    ::Error(gMessTag.Data(),
      "Did you enable the right version of ROOT?");
    return -1;
  }

  //
  // Set extra includes
  //

  {
    TString inc;
    Ssiz_t from = 0;
    while ( extraIncs.Tokenize(inc, from, ":") ) {
      if (inc.IsNull()) continue;
      ::Info(gMessTag.Data(), ">> Adding include path %s", inc.Data());
      gSystem->AddIncludePath(
        Form("-I\"%s/%s\"", aliRootDir.Data(), inc.Data()) );
    }
  }

  //
  // Enable AliEn
  //

  if (enableAliEn) {
    ::Info(gMessTag.Data(), "Connecting to AliEn...");
    TGrid::Connect("alien:");
    if (!gGrid) {
      ::Error(gMessTag.Data(), "Cannot connect to AliEn");
      return -1;
    }
    else {
      ::Info(gMessTag.Data(), "Successfully connected to AliEn");
    }
  }

  return 0;
}
