
TChain *MakeAODInputChain(const char* collectionfileAOD,
			  const char* collectionfileAODfriend,Int_t nfiles=-1,
			  Bool_t stdoutCheck=kFALSE) {
  //
  // Check one-to-one correspondence of the two collections
  // Create AOD chain with friend AODJets chain
  // Origin: A.Rossi, andrea.rossi@ts.infn.it
  //

  TAlienCollection *collectionAOD       = TAlienCollection::Open(collectionfileAOD);
  TGridResult *tagResultAOD = collectionAOD->GetGridResult("",0,0);
  TChain *chainAOD = new TChain("aodTree");
  TChain *chainAODfriend = new TChain("aodTree");
  Int_t nmaxentr;
  if(collectionfileAODfriend!="none"){
    TAlienCollection *collectionAODfriend = TAlienCollection::Open(collectionfileAODfriend);
    TGridResult *tagResultAODFriend = collectionAODfriend->GetGridResult("",0,0);
    //  tagResultAOD->Print();
    TMap *mappa;
    TObjString *filename;
    Int_t event,dir,pos;
    string str;
    Int_t lastAOD[1000],lastAODFriend[1000];
    TArrayI ***arrAOD=new TArrayI**[1000];
    TArrayI ***arrAODFriend=new TArrayI**[1000];
    
    for(Int_t j=0;j<1000;j++) {
      arrAOD[j]=new TArrayI*[2];
      arrAOD[j][0]=new TArrayI(100);
      arrAOD[j][0]->Reset(0);
      arrAOD[j][1]=new TArrayI(100);
      arrAOD[j][1]->Reset(-1);
      
      arrAODFriend[j]=new TArrayI*[2];
      arrAODFriend[j][0]=new TArrayI(100);
      arrAODFriend[j][0]->Reset(0);
      arrAODFriend[j][1]=new TArrayI(100);
      arrAODFriend[j][1]->Reset(-1);
      
      lastAOD[j]=0;
      lastAODFriend[j]=0;
    }
   
    nmaxentr=tagResultAOD->GetEntries();
    if(nfiles>0&&nmaxentr>nfiles)nmaxentr=nfiles;
   
    for(Int_t j=0;j<tagResultAOD->GetEntries();j++) {  
      mappa=(TMap*)tagResultAOD->At(j);
      filename=(TObjString*)mappa->GetValue("turl");
      str=filename->GetString();
      pos=str.find_last_of("/");
      str.string::replace(pos,50,"");
      pos=str.find_last_of("/");
      str.string::replace(pos,1," ");
      sscanf(str.data(),"%*s %d",&event);
      pos=str.find_last_of("/");
      str.string::replace(pos,1," ");
      sscanf(str.data(),"%*s %d",&dir);
      arrAOD[event][0]->AddAt(dir,lastAOD[event]);
      arrAOD[event][1]->AddAt(j,lastAOD[event]);
      // printf("Adding AOD, event:%d,dir:%d,position:%d\n",event,dir,j);
      lastAOD[event]++;
    }
    
    for(Int_t j=0;j<tagResultAODFriend->GetEntries();j++) {  
      mappa=(TMap*)tagResultAODFriend->At(j);
      filename=(TObjString*)mappa->GetValue("turl");
      str=filename->GetString();
      pos=str.find_last_of("/");
      str.string::replace(pos,50,"");
      pos=str.find_last_of("/");
      str.string::replace(pos,1," ");
      sscanf(str.data(),"%*s %d",&event);
      pos=str.find_last_of("/");
      str.string::replace(pos,1," ");
      sscanf(str.data(),"%*s %d",&dir);
      arrAODFriend[event][0]->AddAt(dir,lastAODFriend[event]);
      arrAODFriend[event][1]->AddAt(j,lastAODFriend[event]);
      //printf("Adding AODFriend, event:%d,dir:%d,position:%d\n",event,dir,j);
      lastAODFriend[event]++;
    }
    
      
    for(Int_t ev=0;ev<1000;ev++) {
      for(Int_t j=0;j<lastAOD[ev];j++) {
	dir= arrAOD[ev][0]->At(j);
	for(Int_t k=0;k<lastAODFriend[ev];k++) {
	  if(arrAODFriend[ev][0]->At(k)==dir){
	    chainAOD->Add((((TObjString*)((TMap*)tagResultAOD->At(arrAOD[ev][1]->At(j)))->GetValue("turl"))->GetString()).Data());
	    chainAODfriend->Add((((TObjString*)((TMap*)tagResultAODFriend->At(arrAODFriend[ev][1]->At(k)))->GetValue("turl"))->GetString()).Data());
	    printf("Events: %d, adding AOD at position %d,dir:%d posarray:%d \n and Friend at %d, dir:%d , posarray: %d \n \n",ev,arrAOD[ev][1]->At(j),arrAOD[ev][0]->At(j),j,arrAODFriend[ev][1]->At(k),arrAODFriend[ev][0]->At(k),k);
	    break;
	  }
	}
      }
    }
  } else {
    nmaxentr=tagResultAOD->GetEntries();
    if(nfiles>0&&nmaxentr>nfiles)nmaxentr=nfiles;
    TString aodlfn,vertexlfn;
    for(Int_t ifile=0; ifile<nmaxentr; ifile++) {
      aodlfn=tagResultAOD->GetKey(ifile,"lfn");
      vertexlfn=aodlfn;
      vertexlfn.ReplaceAll("AliAOD.root","");
      TGridResult *r=gGrid->Query(vertexlfn.Data(),"AliAOD.Jets.root");
      if(r->GetEntries()!=1)continue;
      // check error in the stdout
      if(stdoutCheck) {
	if(!CheckStdout(aodlfn)) continue;
      }
      printf("Adding file : %s \n",tagResultAOD->GetKey(ifile,"turl"));
      chainAOD->Add(tagResultAOD->GetKey(ifile,"turl"));
      printf("Adding friend file : %s \n \n",r->GetKey(0,"turl"));
      chainAODfriend->Add(r->GetKey(0,"turl"));
    }
  }
  
  chainAOD->AddFriend(chainAODfriend);
  
  return chainAOD;
}
//----------------------------------------------------------------------------
TChain *MakeAODInputChain(const char* pathname="",
			  Int_t firstdir=1,Int_t lastdir=-1) {
  //
  // Create AOD chain with friend AODJets chain
  // Example path: "alien:///alice/cern.ch/user/r/rbala/analysis/out_lhcw/290001/"
  // Origin: A.Rossi, andrea.rossi@ts.infn.it
  //

 
  TChain *chainAOD = new TChain("aodTree");
  TChain *chainAODfriend = new TChain("aodTree");
 

  if(firstdir==-1) {
    // require the path is an alien path (!!!but without alien::// or /alien/ ), use lastdir as the requested number of files 
    TGridResult* resultAOD=gGrid->Query(pathname,"AliAOD.root","","");
    Int_t nfiles=resultAOD->GetEntries();
    TString aodlfn,vertexlfn;
    if(lastdir>nfiles)lastdir=nfiles;
    for(Int_t ifile=0; ifile<nfiles; ifile++) {
      aodlfn=resultAOD->GetKey(ifile,"lfn");
      vertexlfn=aodlfn;
      vertexlfn.ReplaceAll("AliAOD.root","");
      TGridResult *r=gGrid->Query(vertexlfn.Data(),"AliAOD.Jets.root");
      if(r->GetEntries()!=1)continue;
      chainAOD->Add(resultAOD->GetKey(ifile,"turl"));
      chainAODfriend->Add(r->GetKey(0,"turl"));
    }
  }
  else{
    if(lastdir==-1) { // only one pair of files
      chainAOD->Add("AliAOD.root");
      chainAODfriend->Add("AliAOD.Jets.root"); 
    } else {
  
      // set the path to the files (can be local or on alien)
      for(Int_t idir=firstdir; idir<=lastdir; idir++) {
	TString aodname=pathname;
	TString aodHFname=pathname;
	aodname+=idir;
	aodHFname+=idir;
	aodname.Append("/AliAOD.root");
	aodHFname.Append("/AliAOD.Jets.root");
	chainAOD->Add(aodname.Data());
	chainAODfriend->Add(aodHFname.Data());
      }      
    }
  }

  chainAOD->AddFriend(chainAODfriend);
 
  return chainAOD;
}
//----------------------------------------------------------------------------
TFileCollection* MakeRootArchFileCollection(const char* collectionfileAOD,
					    Int_t nfiles=-1,
					    Bool_t stdoutCheck=kFALSE) {

  // METHOD USEFUL FOR ANALYSIS ON THE CAF
  // Check the presence of both AliAOD.root and AODJets.root files
  // in the same path
  // returns a TFileCollection suitable for storing datasets in CAF 
  // Origin: A.Rossi, andrea.rossi@ts.infn.it
  //
  
  TAlienCollection *collectionAOD       = TAlienCollection::Open(collectionfileAOD);
  TGridResult *tagResultAOD = collectionAOD->GetGridResult("",0,0);

  Int_t nmaxentr;
  TFileCollection *proofColl=new TFileCollection("proofColl","proofColl");
  
  nmaxentr=tagResultAOD->GetEntries();
  printf("Number of files %d\n",nmaxentr);
    if(nfiles>0&&nmaxentr>nfiles)nmaxentr=nfiles;
    TString aodlfn;
    for(Int_t ifile=0; ifile<nmaxentr; ifile++) {
      printf("file %d\n",ifile);
      aodlfn=tagResultAOD->GetKey(ifile,"lfn");
      TString aodlfncopy=aodlfn;
      aodlfn.ReplaceAll("AliAOD.root","");
      aodlfn.ReplaceAll("AliAODs.root","");
      TGridResult *r=gGrid->Query(aodlfn.Data(),"AliAOD.Jets.root");
      if(r->GetEntries()!=1)continue;     
      r=gGrid->Query(aodlfn.Data(),"aod_archive.zip");
      if(r->GetEntries()!=1)continue;
      // check error in the stdout
      if(stdoutCheck) {
	if(!CheckStdout(aodlfncopy)) {
	  FILE *fout=fopen("errors.txt","a");
	  aodlfncopy.ReplaceAll("AliAOD.root","AliAOD.Jets.root");
	  fprintf(fout,"%s\n",aodlfncopy.Data());
	  fclose(fout);
	  continue;
	}
      }
      aodlfn.Append("aod_archive.zip");      
      printf("Adding file %s\n",aodlfn.Data());
      proofColl->Add(r->GetKey(0,"turl"));
    }
    return proofColl;
}
//----------------------------------------------------------------------------
void StageToCAF(TString xmlcoll="collAODLHC08x.xml",
		TString datasetname="AODJets_LHC08x_10files",
		Int_t nfiles=-1) {
  //
  // Staging a dataset to CAF
  // andrea.dainese@pd.infn.it
  //

  //gROOT->LoadMacro("MakeAODInputChain.C");

  TGrid::Connect("alien://");

  // find -x collAODLHC08x -z /alice/cern.ch/user/r/rbala/newtrain/out_lhc08x/* AliAOD.root > collAODLHC08x.xml

  TFileCollection *proofColl = MakeRootArchFileCollection(xmlcoll.Data(),nfiles,KTRUE);
  proofColl->SetAnchor("AliAOD.root");

  gEnv->SetValue("XSec.GSI.DelegProxy","2");
  TProof::Open("dainesea:PWG3@alicecaf"); 
  gProof->RegisterDataSet(datasetname.Data(),proofColl);
  gProof->ShowDataSets();

  return;
}

//---------------------------------------------------------------------------
Bool_t CheckStdout(TString aodlfn) {
  
  TString stderrlfn=aodlfn;
  stderrlfn.ReplaceAll("AliAOD.root","stdout");
  TString aliencp=".! alien_cp -t 10 alien://";
  aliencp.Append(stderrlfn.Data());
  aliencp.Append(" file:stdoutput");
  gROOT->ProcessLine(aliencp.Data());
  gROOT->ProcessLine(".! grep \"no debugging symbols found\" stdoutput > result");
  FILE *fileres=fopen("result","r");
  if(!fileres) return kFALSE;
  Char_t text[100];
  Int_t ncol = fscanf(fileres,"%s %s %s %s",&text,&text,&text,&text);
  //printf("%d  %s\n",ncol,text);
  fclose(fileres);
  gROOT->ProcessLine(".! rm stdoutput result");
  if(ncol>=0) return kFALSE;
  
  return kTRUE;
}

//---------------------------------------------------------------------------
TChain *CreateChain(const char *list, Bool_t addfriends = kFALSE, const char *mode="local", const char *plugin_mode="test")
{  
   Bool_t iAODanalysis=kTRUE;
   TString kLocalXMLDataset = "";//temporary
   TString kLocalDataList= list;
// Create the input chain
   Int_t imode = -1;
   if (!strcmp(mode, "local")) imode = 0;
   if (!strcmp(mode, "proof")) imode = 1;
   if (!strcmp(mode, "grid"))  imode = 2;
   TChain *chain = NULL;
   TChain *chain_friends = NULL;
   // Local chain
   switch (imode) {
      case 0:
         if (iAODanalysis) {
            if (!kLocalXMLDataset.Length()) {
               // Local AOD
               chain = new TChain("aodTree");
               chain_friends = new TChain("aodTree");
	       TString line;
	       ifstream in;
	       in.open(kLocalDataList.Data());
	       while (in.good()) {
		 		in >> line;
		 		if (line.Length() == 0) continue;
		 		//cout << " line = " << line << endl;
		 		chain->Add(line.Data());
         		if (addfriends){
					line.ReplaceAll("AliAOD.root", "AliAOD.Jets.root");
	    			chain_friends->Add(line.Data());
					}   
				}       
 			if(addfriends) chain->AddFriend(chain_friends);
            } else {
               // Interactive AOD
               chain = CreateChainSingle(kLocalXMLDataset, "aodTree");
            }
         } else {      
	   if (!kLocalXMLDataset.Length()) {
	     // Local ESD
	     chain = new TChain("esdTree");
	     TString line;
	     ifstream in;
	     in.open(kLocalDataList.Data());
	     while (in.good()) {
	       in >> line;
	       if (line.Length() == 0) continue;
	       cout << " line = " << line << endl;
	       chain->Add(line.Data());
	     }       
	   } else {
	     // Interactive ESD
               chain = CreateChainSingle(kLocalXMLDataset, "esdTree");
	   }   
         }
         break;
      case 1:
         break;
      case 2:
         if (kPluginUse) {
//            AliAnalysisGrid *alienHandler = CreateAlienHandler(plugin_mode);
//            AliAnalysisManager::GetAnalysisManager()->SetGridHandler(alienHandler);
         } else {
            TString           treeName = "esdTree";
            if (iAODanalysis) treeName = "aodTree";
            chain = CreateChainSingle("wn.xml", treeName);
         }
         break;      
      default:   
   }
   if (chain && chain->GetNtrees()) return chain;
   return NULL;
}   

//---------------------------------------------------------------------------
TChain* CreateChainSingle(const char* xmlfile, const char *treeName)
{
   printf("*******************************\n");
   printf("*** Getting the ESD Chain   ***\n");
   printf("*******************************\n");
   TAlienCollection * myCollection  = TAlienCollection::Open(xmlfile);

   if (!myCollection) {
      ::Error("AnalysisTrainPWG4Jets.C::CreateChainSingle", "Cannot create an AliEn collection from %s", xmlfile) ;
      return NULL ;
   }

   TChain* chain = new TChain(treeName);
   myCollection->Reset() ;
   while ( myCollection->Next() ) chain->Add(myCollection->GetTURL("")) ;
   chain->ls();
   return chain;
}
