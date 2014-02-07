/*************************************************************************
 * Copyright(c) 1998-2009, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* Class AliAnalysisTaskSEHFJets:                                    *
 * AliAnalysisTaskSE for the extraction of the B-jet spectrum        *
 * using the properties of secondary verices as tagging observables. */

/* Mailto: andrea.rossi@cern.ch, elena.bruna@to.infn.it, svallero@to.infn.it*/

#include <TH1F.h>
#include <TH2F.h>
#include <TAxis.h>
#include <TArrayI.h>
#include <TArrayD.h>
#include <THnSparse.h>
#include <TDatabasePDG.h>
#include <TMath.h>
#include <TROOT.h>
#include "AliAODEvent.h"
#include "AliAODRecoDecayHF2Prong.h"
#include "AliAODRecoDecayHF.h"
#include "AliAODRecoDecay.h"
#include "AliAnalysisDataSlot.h"
#include "AliAnalysisDataContainer.h"
#include "AliAODTrack.h"
#include "AliAODHandler.h"
#include "AliESDtrack.h"
#include "AliAODVertex.h"
#include "AliESDVertex.h"
#include "AliVertexerTracks.h"
#include "AliAODMCParticle.h"
#include "AliAODPid.h"
#include "AliTPCPIDResponse.h"
#include "AliAODMCHeader.h"
#include "AliAnalysisTaskSEHFJets.h"
#include "AliRDHFCutsD0toKpi.h"
#include "AliAODInputHandler.h"
#include "AliAnalysisManager.h"
#include "AliNormalizationCounter.h"
#include "AliLog.h"
#include "AliAnalysisTaskJetCluster.h"
#include "AliHFJetsContainerVertex.h"
class TCanvas;
class TTree;
class TChain;
class AliAnalysisTaskSE;
class AliCFContainer;

ClassImp(AliAnalysisTaskSEHFJets)

AliAnalysisTaskSEHFJets::AliAnalysisTaskSEHFJets() 
  : AliAnalysisTaskSE(),
  fhJets(0),
  fhQaVtx(0),
  fhBJets(0),
  fhJetVtx(0),
  fCorrMode(kTRUE),
  fRecoJetsBranch(),
  fMcJetsBranch(),
  fNentries(0),
  fTagger(0),
  fCutsHFjets(0),
  fbJetArray(0),
  fArrayMC(0)
   
{

  // default constructor

}



//________________________________________________________________________
AliAnalysisTaskSEHFJets::AliAnalysisTaskSEHFJets(const char *name) 
  : AliAnalysisTaskSE(name),
  fhJets(0),
  fhQaVtx(0),
  fhBJets(0),
  fhJetVtx(0),
  fCorrMode(kTRUE),
  fRecoJetsBranch(),
  fMcJetsBranch(),
  fNentries(0),
  fTagger(0),
  fCutsHFjets(0),
  fbJetArray(0),
  fArrayMC(0)
{ 
  // standard constructor
  AliInfo("+++ Executing Constructor +++");

  DefineOutput(1, AliHFJetsContainerVertex::Class());
  DefineOutput(2, AliHFJetsContainerVertex::Class());
  DefineOutput(3, AliHFJetsContainerVertex::Class());
  DefineOutput(4, AliHFJetsContainerVertex::Class());
  DefineOutput(5, TH1F::Class());

}

//________________________________________________________________________
AliAnalysisTaskSEHFJets::~AliAnalysisTaskSEHFJets(){

  // destructor
  AliInfo("+++ Executing Destructor +++");

  delete fhJets;
  delete fhQaVtx;
  delete fhBJets;
  delete fhJetVtx;

  if (fTagger) delete fTagger;
  if (fCutsHFjets) delete fCutsHFjets;
}

//________________________________________________________________________
void AliAnalysisTaskSEHFJets::Init()
{
  // Initialization
  AliInfo("+++ Executing Init +++");

  AliLog::SetGlobalDebugLevel(AliLog::kInfo);

}


//________________________________________________________________________
void AliAnalysisTaskSEHFJets::UserCreateOutputObjects(){

  AliInfo("+++ Executing UserCreateOutputObjects +++");

  // Create the containers for jet and vertex properties
  // reconstructed jets
  fhJets= new AliHFJetsContainerVertex("kJets",AliHFJetsContainerVertex::kJets);
  // vertices QA
  fhQaVtx= new AliHFJetsContainerVertex("kQaVtx",AliHFJetsContainerVertex::kQaVtx);
  // tagged jets
  fhBJets= new AliHFJetsContainerVertex("kBJets",AliHFJetsContainerVertex::kBJets);
  // vertices within the jet
  fhJetVtx= new AliHFJetsContainerVertex("kJetsVtx",AliHFJetsContainerVertex::kJetVtx);

  // Control histogram
  fNentries=new TH1F("nentriesChFr", "Analyzed sample properties", 9,-0.5,8.5);
  fNentries->GetXaxis()->SetBinLabel(1,"nEventsAnal");
  fNentries->GetXaxis()->SetBinLabel(2,"nEvSel");
  fNentries->GetXaxis()->SetBinLabel(3,"nEvGoodVtx");
  fNentries->GetXaxis()->SetBinLabel(4,"nEvPile-up Rej");
  fNentries->GetXaxis()->SetBinLabel(5,"nEvRejVtxZ");
  fNentries->GetXaxis()->SetBinLabel(6,"nTracksEv");
  fNentries->GetXaxis()->SetBinLabel(7,"nJetsCand");
  fNentries->GetXaxis()->SetBinLabel(8,"nJetsTagged");
  fNentries->GetXaxis()->SetBinLabel(9,"nUnexpError");

  PostData(1,fhJets);
  PostData(2,fhQaVtx);
  PostData(3,fhBJets);
  PostData(4,fhJetVtx);
  PostData(5,fNentries);
}



//________________________________________________________________________
void AliAnalysisTaskSEHFJets::UserExec(Option_t */*option*/){

  AliInfo("+++ Executing UserExec +++");

  // Execute analysis for current event

  if(!fbJetArray)fbJetArray=new TClonesArray("AliAODVertex",0);
  Double_t arrDispersion[1000];

  // AOD input event
  AliAODEvent *aod = dynamic_cast<AliAODEvent*> (InputEvent());
  if (!aod) {
    AliInfo("Input AOD not available, trying with output handler...");
    if(AODEvent() && IsStandardAOD()) {
    // In case there is an AOD filter writing a standard AOD, use the AOD 
    // event in memory rather than the input event.    
    aod = dynamic_cast<AliAODEvent*> (AODEvent());
    } else AliError(RED"No AOD handler found or no standard AOD!"B);
  }

  // AOD jets from JetCluster task output
  // (JetCluster task should be on the train before this one)
  // Get RECO-jets
  TClonesArray *arrayJets = (TClonesArray*)(((AliAnalysisTaskJetCluster*)AliAnalysisManager::GetAnalysisManager()->GetTask(fRecoJetsBranch))->fTCAJetsOut);
  if(!arrayJets)
    AliError(RED"Jets RECO branch not found!"B);
  // GetMC-jets
  TClonesArray *arrayMCJets;
  if (fCorrMode){
    arrayMCJets = (TClonesArray*)(((AliAnalysisTaskJetCluster*)AliAnalysisManager::GetAnalysisManager()->GetTask(fMcJetsBranch))->fTCAJetsOut);
    if(!arrayMCJets)
      AliError(RED"Jets MC branch not found!"B);
  }

  fNentries->Fill(0); // EventsAnal      
  if(!fCutsHFjets->IsEventSelected(aod)){
    AliDebug(AliLog::kDebug,"Event did not pass event selection from AliRDHFJetsCuts!");
    PostData(5,fNentries);
    //return;
  }else fNentries->Fill(1); // EvSel    


  Int_t mult = 1; // dummy value for the moment TODO
  AliHFJetsContainer::CFSteps step;
  step = AliHFJetsContainer::kCFStepAll;
  // We should fill the container at each step, so quantities are evaluated 
  // before event selection
  if (fCorrMode){

    // Get array of MC particles
    fArrayMC = (TClonesArray*)aod->GetList()->FindObject(AliAODMCParticle::StdBranchName());
    if(!fArrayMC) AliError(RED"MC particles branch not found!"B);
    // Count number of primary MC partcles
    Int_t nMC = fArrayMC->GetEntries();
    Int_t multMC = 0;
    for (Int_t i = 0; i<nMC; i++){
      AliAODMCParticle *part = (AliAODMCParticle*)fArrayMC->At(i);
      if (part->IsPhysicalPrimary()) multMC++;
      }
    //AliInfo(Form(cy"MC particles %d primaries %d"B,nMC, multMC ));

    // Loop on MC jets
    Int_t nMCJets=arrayMCJets->GetEntries();
    AliAODJet *jetMC;
    for(Int_t jetcand=0;jetcand<nMCJets;jetcand++){
      jetMC=(AliAODJet*)arrayMCJets->UncheckedAt(jetcand);
      // restrict jet eta and pT ranges
      if(!fCutsHFjets->IsJetSelected(jetMC)){
        AliDebug(AliLog::kDebug,Form("JetMC not selected: pT=%f, eta=%f!", jetMC->Pt(),jetMC->Eta()));
        continue;
      }
      // Asking for at least 2 tracks in the jet
      //TRefArray* reftracksMC=(TRefArray*)jetMC->GetRefTracks();
      //Int_t ntrksMC=reftracksMC->GetEntriesFast();
      //if(ntrksMC<3)continue;

      // Get jet flavour from 3 methods
      Double_t partonnatMC[3];
      Double_t ptpartMC[3];
      Double_t contributionMC=0;
      // contribution = pT weight of mother parton (only method 1)
      GetFlavour3Methods(jetMC, partonnatMC, ptpartMC, contributionMC);
      // Fill container tagger
      //fhBJets->FillStepBJets(step,multMC,jetMC,0,partonnatMC,contributionMC,ptpartMC[0]);
    } // end loop on jets

  } // end if (fCorrMode)

  PostData(1,fhJets);
  PostData(2,fhQaVtx);
  PostData(3,fhBJets);
  PostData(4,fhJetVtx);
  PostData(5,fNentries);
  //delete fArrayMC;
  //delete partonnatMC;
  //delete ptpartMC;
  fbJetArray->Clear();
  return;

  if (1){
  // AOD primary vertex (for event selection)
  AliAODVertex *vtx1 = (AliAODVertex*)aod->GetPrimaryVertex();
  TString primTitle = vtx1->GetTitle();
  // require "VertexerTracks" and at least 1 contributor
  if(primTitle.Contains("VertexerTracks") && vtx1->GetNContributors()>0) {
    fNentries->Fill(2); // EvGoodVtx  
  }else {
    AliDebug(AliLog::kDebug,"Event did not pass primary vertex selection!");
    PostData(5,fNentries);
    return;
  }

  // Convert to AliESDVertex 
  Double_t primvtx[3],primcov[6];
  vtx1->GetXYZ(primvtx);
  vtx1->GetCovarianceMatrix(primcov);
  Int_t nPrimContr=vtx1->GetNContributors();
  Double_t chi2=vtx1->GetChi2();
  AliESDVertex* v1 = new AliESDVertex(primvtx,primcov,chi2,nPrimContr);
  Double_t magzkG = (Double_t)aod->GetMagneticField();

  // MC information (vertex and particles)
  TClonesArray *arrayMC=0x0;
  AliAODMCHeader *aodmcHeader=0x0;
  Double_t vtxTrue[3];

  if(fCorrMode){
    // load MC particles
    arrayMC =
      (TClonesArray*)aod->GetList()->FindObject(AliAODMCParticle::StdBranchName());
    if(!arrayMC) AliError(RED"MC particles branch not found!"B);

    // load MC header
    aodmcHeader =
      (AliAODMCHeader*)aod->GetList()->FindObject(AliAODMCHeader::StdBranchName());
    if(!aodmcHeader) AliError(RED"MC header branch not found!"B);

    // MC primary vertex
    aodmcHeader->GetVertex(vtxTrue);
  }


  // Loop on jets
  Int_t nJets=arrayJets->GetEntries();
  AliAODJet *jet;

  Int_t nvtx=0;
  for(Int_t jetcand=0;jetcand<nJets;jetcand++){
    nvtx=0;
    jet=(AliAODJet*)arrayJets->UncheckedAt(jetcand);

    if(!fCutsHFjets->IsJetSelected(jet)){
      AliDebug(AliLog::kDebug,Form("Jet not selected: pT=%f, eta=%f!", jet->Pt(),jet->Eta()));
      continue;
    }

    Double_t partonnat[3]={0.,0.,0.};
    Double_t ptpart[3]={-1.,-1.,-1.};
    Double_t contribution=0.;
    // contribution = pT weight of mother parton (only method 1)

    if(fCorrMode){ // THIS MAKES SENSE ONLY FOR MC
      AliAODMCParticle *parton[3];
      parton[0]=fTagger->IsMCJet(arrayMC,jet,contribution); // method 1
      parton[1]=(AliAODMCParticle*)fTagger->IsMCJetParton(arrayMC,jet); // method 2
      parton[2]=(AliAODMCParticle*)fTagger->IsMCJetMeson(arrayMC,jet); // method 3

      if(parton[0]){
        Int_t pdg=TMath::Abs(parton[0]->PdgCode());
        if(pdg==4 || pdg==5) AliInfo(Form(cy"track method -> pdg parton: %d, contribution =%f"B,pdg,contribution));
        if(pdg==21)partonnat[0]=1;
        else if(pdg<4)partonnat[0]=2;
        else if(pdg==4)partonnat[0]=3;
        else if(pdg==5)partonnat[0]=4;
        ptpart[0]=parton[0]->Pt();
      }

      if(parton[1]!=0){
        Int_t pdg=TMath::Abs(parton[1]->PdgCode());
        if(pdg==4 || pdg==5) AliInfo(Form(cy"parton method -> pdg parton: %d"B,pdg));
        if(pdg==21)partonnat[1]=1;
        else if(pdg<4)partonnat[1]=2;
        else if(pdg==4)partonnat[1]=3;
        else if(pdg==5)partonnat[1]=4;
        ptpart[1]=parton[1]->Pt();
      }

      if(parton[2]!=0){
        Int_t pdg=TMath::Abs(parton[2]->PdgCode());
        if((pdg>=400 && pdg<=600) || (pdg>=4000 && pdg<=6000)) AliInfo(Form(cy"meson method -> pdg parton: %d"B,pdg));
        if((pdg>=400 && pdg<=500) || (pdg>=4000 && pdg<=5000))partonnat[2]=3;
        else{
          if((pdg>=500 && pdg<=600) || (pdg>=5000 && pdg<=6000))partonnat[2]=4;
          else partonnat[2]=2;
        }

        ptpart[2]=parton[2]->Pt();
      }
    }

    TRefArray* reftracks=(TRefArray*)jet->GetRefTracks();
    Double_t ntrks=reftracks->GetEntriesFast();
    // Asking for at leas 2 tracks in the jet
    if(ntrks<3)continue;

    // Fill container jets
    //Int_t mult = 1; // dummy value for the moment TODO
    //AliHFJetsContainer::CFSteps step;
    step = AliHFJetsContainer::kCFStepVertex;
    fhJets->FillStepJets(step,mult,jet,partonnat,contribution,ptpart);
    // Run b-tagger
    // THIS MAKES SENSE ALSO FOR DATA
    nvtx=fTagger->FindVertices(jet,aod,v1,magzkG,fbJetArray,arrDispersion);
    //printf(" %d vertices, %d array size\n",nvtx,fbJetArray->GetEntries());
    if(nvtx>0){

      // QA vertici prima di selezione  --> selezione gia` fatta in FindVertices
      // Fill container vertices
      fhQaVtx->FillStepQaVtx(step,mult,jet,fbJetArray,arrDispersion,nvtx,vtx1,arrayMC,partonnat);
      fhJetVtx->FillStepJetVtx(step,mult,jet,fbJetArray,nvtx,vtx1,arrayMC,partonnat);
      // SELECTION ??

      // Fill container tagger
      //fhBJets->FillStepBJets(step,mult,jet,nvtx,partonnat,contribution,ptpart[0]);

      fbJetArray->Clear();
    }
    else AliDebug(AliLog::kDebug,"*** nvtx=0 !! ***");
  }
  } // end if(0)
  PostData(1,fhJets);
  PostData(2,fhQaVtx);
  PostData(3,fhBJets);
  PostData(4,fhJetVtx);
  PostData(5,fNentries);

  //delete v1;
  //delete fArrayMC; 
  //delete fCutsHFjets;
  //delete fTagger;
  //fbJetArray->Delete();
  //delete fbJetArray;
  //delete fhJets;
  //delete fhQaVtx;
  //delete fhBJets;
  //delete fhJetVtx;
  //delete fArrayMC;
  //delete aod;

}


void AliAnalysisTaskSEHFJets::Terminate(const Option_t*){

  //TERMINATE METHOD: NOTHING TO DO
  AliInfo("+++ Executing Terminate +++");

}

void AliAnalysisTaskSEHFJets::GetFlavour3Methods(AliAODJet *jet, Double_t (&partonnat)[3], Double_t (&ptpart)[3], Double_t &contribution){

    // 3 methods  to associate jet to mother parton

    /* Nature of the parton (methods 1,2)    *
     * 1 = gluon (pdg 21)                    *
     * 2 = light quark (pdg < 4)             *
     * 3 = c (pdg 4)                         *
     * 4 = b (pdg 5)                         *
     * Nature of the meson/barion (method 3) *
     * 2 = light                             *
     * 3 = with c                            *
     * 4 = with b                            */

     // Initialize output values
     for (Int_t i=0; i<3; i++){
        partonnat[i]=0;
        ptpart[i] = -1.;
      }
      AliAODMCParticle *parton[3];
      parton[0]=fTagger->IsMCJet(fArrayMC,jet,contribution); // method 1
      parton[1]=(AliAODMCParticle*)fTagger->IsMCJetParton(fArrayMC,jet); // method 2
      parton[2]=(AliAODMCParticle*)fTagger->IsMCJetMeson(fArrayMC,jet); // method 3

      if(parton[0]){
        Int_t pdg=TMath::Abs(parton[0]->PdgCode());
        if(pdg==4 || pdg==5) AliInfo(Form(cy"track method -> pdg parton: %d, contribution =%f"B,pdg,contribution));
        if(pdg==21)partonnat[0]=1;
        else if(pdg<4)partonnat[0]=2;
        else if(pdg==4)partonnat[0]=3;
        else if(pdg==5)partonnat[0]=4;
        ptpart[0]=parton[0]->Pt();
      }

      if(parton[1]!=0){
        Int_t pdg=TMath::Abs(parton[1]->PdgCode());
        if(pdg==4 || pdg==5) AliInfo(Form(cy"parton method -> pdg parton: %d"B,pdg));
        if(pdg==21)partonnat[1]=1;
        else if(pdg<4)partonnat[1]=2;
        else if(pdg==4)partonnat[1]=3;
        else if(pdg==5)partonnat[1]=4;
        ptpart[1]=parton[1]->Pt();
      }

      if(parton[2]!=0){
        Int_t pdg=TMath::Abs(parton[2]->PdgCode());
        if((pdg>=400 && pdg<=600) || (pdg>=4000 && pdg<=6000)) AliInfo(Form(cy"meson method -> pdg parton: %d"B,pdg));
        if((pdg>=400 && pdg<=500) || (pdg>=4000 && pdg<=5000))partonnat[2]=3;
        else{
          if((pdg>=500 && pdg<=600) || (pdg>=5000 && pdg<=6000))partonnat[2]=4;
          else partonnat[2]=2;
        }
        ptpart[2]=parton[2]->Pt();
      }

}

