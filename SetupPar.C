Int_t setupPar(const char* pararchivename, TList *list) {
  ///////////////////
  // Setup PAR File//
  ///////////////////

  if (pararchivename) {
    char processline[1024];
    TString base = gSystem->BaseName(pararchivename);
    TString dir  = gSystem->DirName(pararchivename);
    TString ocwd = gSystem->WorkingDirectory();
    // Move to dir where the par files are and unpack 
    gSystem->ChangeDirectory(dir.Data());
    sprintf(processline,".! tar xvzf %s.par",base.Data());
    gROOT->ProcessLine(processline);
    // Move to par folder                           
    gSystem->ChangeDirectory(base.Data());

    // check for BUILD.sh and execute                
    if (!gSystem->AccessPathName("PROOF-INF/BUILD.sh")) {
      printf("*******************************\n");
      printf("*** Building PAR archive    ***\n");
      printf("*******************************\n");

      if (gSystem->Exec("PROOF-INF/BUILD.sh")) {
        Error("runAnalysis","Cannot Build the PAR Archive! - Abort!");
	gSystem->ChangeDirectory(ocwd.Data());
        return -1;
      }
    }
    // check for SETUP.C and execute                
    if (!gSystem->AccessPathName("PROOF-INF/SETUP.C")) {
      printf("*******************************\n");
      printf("*** Setup PAR archive       ***\n");
      printf("*******************************\n");
      // If dir not empty, set the full include path 
      if (dir.Length()) {
	sprintf(processline, ".include %s", pararchivename);
	gROOT->ProcessLine(processline);
      }
      gROOT->LoadMacro("PROOF-INF/SETUP.C");
      SETUP(list);
    }
    gSystem->ChangeDirectory(ocwd.Data());
  }
  return 1;
}
