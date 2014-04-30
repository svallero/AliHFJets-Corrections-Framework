AliAnalysisTaskSEHFJetsOrig* AddTaskSEHFJetsOrig(TString fileout="standard",Bool_t readmc=kTRUE,TString cutfile="HFJetVertexCuts.root",TString containerprefix="c", Float_t minC=0., Float_t maxC=7.5)
{  
  //
  // Configuration macro for the task to analyze the fraction of prompt charm
  // using the D0 impact parameter
  // andrea.rossi@ts.infn.it
  //
  //==========================================================================

  Int_t last=0;

  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr) {
    ::Error("AddTaskSEHFJetsOrig", "No analysis manager to connect to.");
    return NULL;
  }   
  
  TString str,containername;
  if(fileout=="standard"){
    fileout=AliAnalysisManager::GetCommonFileName();
    fileout+=":PWG3_HFCJ_";
    fileout+="HFjetVertex";
    if(containerprefix!="c")fileout+=containerprefix;
    str="HFjetVertex";
  }
  else {
    str=fileout;
    str.ReplaceAll(".root","");
  }
  str.Prepend("_");

  AliAnalysisTaskSEHFJetsOrig *hfTask;
  
  
  if(!gSystem->AccessPathName(cutfile.Data(),kFileExists)){
    TFile *f=TFile::Open(cutfile.Data());
    AliRDHFJetsCuts.cxx *cuts= (AliRDHFCutsD0toKpi*)f->Get("HFJetsCutsVertex");
    cuts->PrintAll();
    hfTask = new AliAnalysisTaskSEHFJetsOrig("AliAnalysisTaskSEHFJetsOrig");
    AliHFJetVertexTagging *tagger=new AliHFJetVertexTagging();
    hfTask->SetTagger(tagger);
    hfTask->SetCuts(cuts);
  }
  else {
    
    AliRDHFJetsCuts *cuts=new AliRDHFJetsCuts();
    hfTask = new AliAnalysisTaskSEHFJetsOrig("AliAnalysisTaskSEHFJetsOrig");
    hfTask->SetCuts(cuts);
    AliHFJetVertexTagging *tagger=new AliHFJetVertexTagging();

    AliRDHFJetsCutsVertex *cuts2=new AliRDHFJetsCutsVertex("jetCuts");

    //cuts for jets
    cuts->SetJetRadius(0.4);
    cuts->SetMaxEtaJet(0.5);//0.9-R
    cuts->SetMinPtJet(5);
    cuts->SetMaxPtJet(200);
    cuts2->SetJetRadius(0.4);
    cuts2->SetMaxEtaJet(0.5);//0.9-R
    cuts2->SetMinPtJet(5);
    cuts2->SetMaxPtJet(200);

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
    //cuts for vertexing
    cuts2->SetNprongs(3);
    cuts2->SetIsElec(kFALSE); // kTRUE to select e in jet vertex  

 //    cuts2->SetMinPtHardestTrack(1.5);//default 0.3
//     cuts2->SetSecVtxWithKF(kFALSE);//default with StrLinMinDist
//     cuts2->SetImpParCut(0.01);//default 0
//     cuts2->SetDistPrimSec(0.1);//default 0
//     cuts2->SetCospCut(0.7);//default -1

//   cuts2->SetMinPtHardestTrack(1.3);//default 0.3
//     cuts2->SetSecVtxWithKF(kFALSE);//default with StrLinMinDist
//     cuts2->SetImpParCut(0.01);//default 0
//     cuts2->SetDistPrimSec(0.05);//default 0
//     cuts2->SetCospCut(0.7);//default -1

    cuts2->SetMinPtHardestTrack(1.0);//default 0.3
    cuts2->SetSecVtxWithKF(kFALSE);//default with StrLinMinDist
    cuts2->SetImpParCut(0.);//default 0
    cuts2->SetDistPrimSec(0.);//default 0
    cuts2->SetCospCut(-1);//default -1
 

    tagger->SetCuts(cuts2);
    hfTask->SetCuts(cuts);
    hfTask->SetTagger(tagger);
    //    ::Error("AddTaskSEHFJets","No Cut Object");
  }

  cuts->SetMinCentrality(minC);
  cuts->SetMaxCentrality(maxC);
  hfTask->SetReadMC(readmc);
  
  
  mgr->AddTask(hfTask);
 
 
  
 // Create containers for input/output
  AliAnalysisDataContainer *cinput =   mgr->GetCommonInputContainer();
  //mgr->CreateContainer("cinput",TChain::Class(),AliAnalysisManager::kInputContainer);
  mgr->ConnectInput(hfTask,0,cinput);
  

  //Now container for general properties histograms
  containername="outputNentries";
  containername.Prepend(containerprefix.Data());
  containername.Append(str.Data());
  AliAnalysisDataContainer *coutputNentries = mgr->CreateContainer(containername.Data(),TH1F::Class(), AliAnalysisManager::kOutputContainer, fileout.Data());
  mgr->ConnectOutput(hfTask,1,coutputNentries);

  containername="RecoJetsProp";
  containername.Prepend(containerprefix.Data());
  containername.Append(str.Data());
  AliAnalysisDataContainer *cSparseVtxProp = mgr->CreateContainer(containername.Data(),THnSparse::Class(), AliAnalysisManager::kOutputContainer, fileout.Data());
  mgr->ConnectOutput(hfTask,2,cSparseVtxProp);

  containername="SparseVerticesProp";
  containername.Prepend(containerprefix.Data());
  containername.Append(str.Data());
  AliAnalysisDataContainer *cSparseVtxProp = mgr->CreateContainer(containername.Data(),THnSparse::Class(), AliAnalysisManager::kOutputContainer, fileout.Data());
  mgr->ConnectOutput(hfTask,3,cSparseVtxProp);

  containername="SparseJetVerticesProp";
  containername.Prepend(containerprefix.Data());
  containername.Append(str.Data());
  AliAnalysisDataContainer *cSparseVtxProp = mgr->CreateContainer(containername.Data(),THnSparse::Class(), AliAnalysisManager::kOutputContainer, fileout.Data());
  mgr->ConnectOutput(hfTask,4,cSparseVtxProp);


  containername="SparsebTaggedJets";
  containername.Prepend(containerprefix.Data());
  containername.Append(str.Data());
  AliAnalysisDataContainer *cSparsebJets = mgr->CreateContainer(containername.Data(),THnSparse::Class(), AliAnalysisManager::kOutputContainer, fileout.Data());
  mgr->ConnectOutput(hfTask,5,cSparsebJets);




  return hfTask;
}
