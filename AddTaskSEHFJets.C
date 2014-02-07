AliAnalysisTaskSEHFJets* AddTaskSEHFJets(TString fileout="standard",Bool_t corrections_mode=kTRUE,TString cutfile="HFJetVertexCuts.root",TString containerprefix="c", Float_t minC=0., Float_t maxC=7.5)
{

  // Mailto: andrea.rossi@ts.infn.it, svallero@to.infn.it

  Int_t last=0;

  // Get the AnalysisManager
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr) {
    ::Error("AddTaskSEHFJets", "No analysis manager to connect to.");
    return NULL;
  }   
  
  TString str,containername;
  if(fileout=="standard"){
    fileout=AliAnalysisManager::GetCommonFileName();
    fileout+=":PWGHF_HFCJ_";
    fileout+="HFJetsVertex";
    if(containerprefix!="c")fileout+=containerprefix;
    str="HFJetsVertex";
  }
  else {
    str=fileout;
    str.ReplaceAll(".root","");
  }
  str.Prepend("_");

  // Configure analysis task
  AliAnalysisTaskSEHFJets *hfTask;
  hfTask = new AliAnalysisTaskSEHFJets("AliAnalysisTaskSEHFJets");
  
  // Set data or corrections mode
  hfTask->SetCorrectionsMode(corrections_mode); // kFALSE for real data

  //Set the jets branch
  hfTask->SetRecoJetsBranch("JetClusterAOD_ANTIKT04_B0_Filter00272_Cut00150_Skip00");
  hfTask->SetMcJetsBranch("JetClusterAODMC2_ANTIKT04_B0_Filter00272_Cut00150_Skip00");

  // Define the tagger
  AliHFJetsTaggingVertex *tagger=new AliHFJetsTaggingVertex();

  // Set analysis cuts
  if(!gSystem->AccessPathName(cutfile.Data(),kFileExists)){
<<<<<<< HEAD
     read cuts from file
     ::Info(Form("Reading cuts from file: %s", cutfile.Data()));
     TFile *f=TFile::Open(cutfile.Data());
     AliRDHFJetsCuts.cxx *cuts= (AliRDHFCutsD0toKpi*)f->Get("HFJetsCutsVertex");
=======
    // read cuts from file
    ::Info(Form("Reading cuts from file: %s", cutfile.Data()));
    TFile *f=TFile::Open(cutfile.Data());
    AliRDHFJetsCuts *cuts= (AliRDHFCutsD0toKpi*)f->Get("HFJetsCutsVertex");
>>>>>>> 1dee15b3f1681e0b28ab218380be81768f87986d
    // Set centrality 
    cuts->SetMinCentrality(minC);
    cuts->SetMaxCentrality(maxC);
    cuts->PrintAll();
    hfTask->SetCuts(cuts);
  } else {
    // define your cuts here
    DefineCutsTask(hfTask, minC,maxC);
    DefineCutsTagger(tagger);
  }


  // Set read MC
  //hfTask->SetReadMC(readmc); // deprecated
  
  // Add task to manager
  hfTask->SetTagger(tagger);
  mgr->AddTask(hfTask);
  
  // Create containers for input/output
  AliAnalysisDataContainer *cinput =   mgr->GetCommonInputContainer();
  mgr->ConnectInput(hfTask,0,cinput);
 
  // Simple histogram to count events
  containername="outputNentries";
  containername.Prepend(containerprefix.Data());
  containername.Append(str.Data());
  AliAnalysisDataContainer *coutputNentries = mgr->CreateContainer(containername.Data(), TH1F::Class(), AliAnalysisManager::kOutputContainer, fileout.Data());
  mgr->ConnectOutput(hfTask,5,coutputNentries);

  // Containeris for jets and vertices properties
  TString names("Jets;QaVtx;BJets;JetVtx");
  TObjArray *arr;
  TObjString *objstr;
  arr = names.Tokenize(";");
  TIter next(arr);
  Int_t i=0;
  AliAnalysisDataContainer *containerJets[4];
  while ((objstr=(TObjString*)next())){
    containername=objstr->GetString();
    containername.Prepend(containerprefix.Data());
    containername.Append(str.Data());
    containerJets[i] = mgr->CreateContainer(containername.Data(),AliHFJetsContainerVertex::Class(),AliAnalysisManager::kOutputContainer, fileout.Data());
    mgr->ConnectOutput(hfTask,i+1,containerJets[i]);
    i++;
  }
<<<<<<< HEAD
  
  delete tagger;

=======
  //delete tagger;
>>>>>>> 1dee15b3f1681e0b28ab218380be81768f87986d
  return hfTask;
}

//------------------------------------------------------
void DefineCutsTask(AliAnalysisTaskSEHFJets *task, Float_t minC, Float_t maxC){

    // define cuts for task
    AliRDHFJetsCuts *cuts=new AliRDHFJetsCuts();
    // jets
    cuts->SetJetRadius(0.4);
    cuts->SetMaxEtaJet(0.5);//0.9-R
    cuts->SetMinPtJet(5);
    cuts->SetMaxPtJet(200);
    // Set centrality 
    cuts->SetMinCentrality(minC);
    cuts->SetMaxCentrality(maxC);
    task->SetCuts(cuts);
    delete cuts; 
}

//------------------------------------------------------
void DefineCutsTagger(AliHFJetsTaggingVertex *tg){
   
    // define cuts for tagger:
    // 1) AliRDHFJetsCuts: event selections, basic jet cuts (pT jet, accpetance, emcal, pT leading-track)
    // 2) AliRDHFJetsCutsVertex: cuts to reconstruct vertices
    // (nprong, pT jet, eta, R, pTmin tracks, electron ID, displacement and other cut variables)
    AliRDHFJetsCutsVertex *cuts2=new AliRDHFJetsCutsVertex("jetCuts");

    // jets
    cuts2->SetJetRadius(0.4);
    cuts2->SetMaxEtaJet(0.5);//0.9-R
    cuts2->SetMinPtJet(5);
    cuts2->SetMaxPtJet(200);
    // tracks
    AliESDtrackCuts *esdTrackCuts = new AliESDtrackCuts("AliESDtrackCuts","default");
    esdTrackCuts->SetRequireSigmaToVertex(kFALSE);
    esdTrackCuts->SetMinNClustersTPC(70);
    esdTrackCuts->SetMaxChi2PerClusterTPC(4);
    esdTrackCuts->SetRequireTPCRefit(kTRUE);
    esdTrackCuts->SetRequireITSRefit(kTRUE);
    //esdTrackCuts->SetMinNClustersITS(4);
    esdTrackCuts->SetClusterRequirementITS(AliESDtrackCuts::kSPD,AliESDtrackCuts::kAny);
    esdTrackCuts->SetMinDCAToVertexXY(0.);
    esdTrackCuts->SetEtaRange(-0.8,0.8);
    esdTrackCuts->SetPtRange(0.3,1.e10);
  
    cuts2->AddTrackCuts(esdTrackCuts);
    
    // vertexing
    cuts2->SetNprongs(3);
    cuts2->SetIsElec(kFALSE); // kTRUE to select e in jet vertex  

    cuts2->SetMinPtHardestTrack(0.3);//default 0.3
    cuts2->SetSecVtxWithKF(kFALSE);//default with StrLinMinDist
    cuts2->SetImpParCut(0.);//default 0
    cuts2->SetDistPrimSec(0.);//default 0
    cuts2->SetCospCut(-1);//default -1
    tg->SetCuts(cuts2);

    delete esdTrackCuts;
    delete cuts2;
}
