/**************************************************************************
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

/////////////////////////////////////////////////////////////
//
// Class AliAnalysisTaskSEHFjets
// AliAnalysisTaskSE for the extraction of the fraction of prompt charm
// using the charm hadron impact parameter to the primary vertex
//
// Author: Andrea Rossi, andrea.rossi@cern.ch, Elena Bruna, elena.bruna@to.infn.it
/////////////////////////////////////////////////////////////


#include <TH1F.h>
#include <TH2F.h>
#include <TAxis.h>
#include <THnSparse.h>
#include <TDatabasePDG.h>
#include <TMath.h>
#include <TROOT.h>
#include "AliAODEvent.h"
#include "AliAODRecoDecayHF2Prong.h"
#include "AliAODRecoDecayHF.h"
#include "AliAODRecoDecay.h"
#include "AliAnalysisDataSlot.h"
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
#include "AliAnalysisVertexingHF.h"
#include "AliAnalysisTaskSEHFJetsOrig.h"
#include "AliRDHFCutsD0toKpi.h"
#include "AliAODInputHandler.h"
#include "AliAnalysisManager.h"
#include "AliNormalizationCounter.h"
#include "AliRDHFJetsCuts.h"
#include "AliHFJetVertexTagging.h"
#include "AliAnalysisTaskJetCluster.h"
#include "AliCFContainer.h"

class TCanvas;
class TTree;
class TChain;
class AliAnalysisTaskSE;
class AliCFContainer;

ClassImp(AliAnalysisTaskSEHFJetsOrig)

AliAnalysisTaskSEHFJetsOrig::AliAnalysisTaskSEHFJetsOrig() 
: AliAnalysisTaskSE(),
  fReadMC(0),
  fCutsHFjets(0x0),
  fTagger(0x0),
  fbJetArray(0x0),
  fSparseRecoJets(0x0),
  fSparseVerticesProp(0x0),
  fSparseJetVerticesProp(0x0),
  fSparsebTaggedJets(0x0)

{// standard constructor
 
}



//________________________________________________________________________
AliAnalysisTaskSEHFJetsOrig::AliAnalysisTaskSEHFJetsOrig(const char *name) 
  : AliAnalysisTaskSE(name),
    fReadMC(0),
    fCutsHFjets(0x0),
    fTagger(0x0),
    fbJetArray(0x0),
    fSparseRecoJets(0x0),
    fSparseVerticesProp(0x0),
    fSparseJetVerticesProp(0x0),
    fSparsebTaggedJets(0x0)
{ // default constructor


  DefineOutput(1, TH1F::Class());
  DefineOutput(2, THnSparseD::Class());
  DefineOutput(3, THnSparseD::Class());
  DefineOutput(4, THnSparseD::Class());
  DefineOutput(5, THnSparseD::Class());

}

//________________________________________________________________________
AliAnalysisTaskSEHFJetsOrig::~AliAnalysisTaskSEHFJetsOrig(){
  // destructor
  delete fCutsHFjets;
  delete fTagger;
  fbJetArray->Delete();
  delete fbJetArray;
  delete fSparseRecoJets;
  delete fSparseVerticesProp;
  delete fSparseJetVerticesProp;
  delete fSparsebTaggedJets;

}

//________________________________________________________________________
void AliAnalysisTaskSEHFJetsOrig::Init()
{
  // Initialization
  
}


//________________________________________________________________________
void AliAnalysisTaskSEHFJetsOrig::UserCreateOutputObjects(){

  // Create the containers for jet and vetex properties
  fNentries=new TH1F("nentriesChFr", "Analyzed sample properties", 9,-0.5,8.5);

  fNentries->GetXaxis()->SetBinLabel(1,"nEventsAnal");

  fNentries->GetXaxis()->SetBinLabel(2,"nEvSel");
  fNentries->GetXaxis()->SetBinLabel(3,"nEvPile-up Rej");
  fNentries->GetXaxis()->SetBinLabel(4,"nEvGoodVtxS");
  fNentries->GetXaxis()->SetBinLabel(5,"nEvRejVtxZ");
  fNentries->GetXaxis()->SetBinLabel(6,"nTracksEv");
  fNentries->GetXaxis()->SetBinLabel(7,"nJetsCand");
  fNentries->GetXaxis()->SetBinLabel(8,"nJetsTagged");
  fNentries->GetXaxis()->SetBinLabel(9,"nUnexpError");
  
  // reconstructed jets: pt, eta, phi, n-tracks, n-electrons, nature (MC info if available),contribution,  pt parton (MC info if available)
  Int_t nbinsRecoJets[13]={50,20,20,20,5,5,5,5,10,60,60,60,50};
  Double_t binlowRecoJets[13]={5.,-1.,-TMath::Pi(),0.99,0.,-0.5,-0.5,-0.5,0.,0.,0.,0.,0.};
  Double_t binupRecoJets[13]={55.,1.,TMath::Pi(),20.99,4.99,4.5,4.5,4.5,2.,60.,60.,60.,1.};
  fSparseRecoJets=new THnSparseD("fSparseRecoJets","fSparseRecoJets;jetpt;eta;phi;ntrks;nEle;parton;parton2;parton3;partContr;ptPart;ptPart2;ptPart3;AreaCh;",13,nbinsRecoJets,binlowRecoJets,binupRecoJets);
  

  // vertex QA sparse histo: jet pt,ntrks,vtx pt,mass,distance to prim vertex in xy, chi2/ndf parton nature, number of real vertices, deltaX,deltaY for most probable Vtx 
  Int_t nbinsVerticesProp[18]={50,20,20,20,200,10,5,5,15,15,20,100,30,50,5,5,5,5};
  Double_t binlowVerticesProp[18]={5.,0.,0.,0.,-1.,0.,-0.5,-0.5,-0.03,-0.03,0.,-1,0.,-1.,-0.5,-0.5,-0.5,-0.5};
  Double_t binupVerticesProp[18]={55.,20.,20.,5.,1.,10.,4.5,4.5,0.03,0.03,0.1,1.,0.2,1.,4.5,4.5,4.5,4.5};
  fSparseVerticesProp=new THnSparseD("fSparseVerticesProp","fSparseVerticesProp;jetpt;ntrks;ptVtx;mass;Lxy;chi2/ndf;parton;nRealVtx;deltaX;deltaY;sigvert;LxyJet;LxyJetPerp;cosTheta;nfromB;nfromBD;parton2;parton3;",18,nbinsVerticesProp,binlowVerticesProp,binupVerticesProp);
  
  // jet pt, n-vertices, sum of masses of the 3 most displaced vertices, decay length of first 3 vertices, parton nat
  Int_t nbinsJetVerticesProp[21]={50,20,20,50,50,50,20,20,20,5,5,5,5,5,5,5,5,5,5,5,5};
  Double_t binlowJetVerticesProp[21]={5.,0.,0.,0.,0.,0.,0.,0.,0.,-0.5,-0.5,-0.5,-0.5,-0.5,-0.5,-0.5,-0.5,-0.5,-0.5,-0.5,-0.5};
  Double_t binupJetVerticesProp[21]={55.,20.,20.,1.,0.5,0.5,5.,5.,5.,4.5,4.5,4.5,4.5,4.5,4.5,4.5,4.5,4.5,4.5,4.5,4.5};
  fSparseJetVerticesProp=new THnSparseD("fSparseJetVerticesProp","fSparseJetVerticesProp;jetpt;nRecoVert;SumMass3MostDispl;Lxy1;Lxy2;Lxy3;mass1;mass2;mass3;nReal1;nReal2;nReal3;nFromB1;nFromB2;nFromB3;nFromPrD1;nFromPrD2;nFromPrD3;parton;parton2;parton3;",21,nbinsJetVerticesProp,binlowJetVerticesProp,binupJetVerticesProp);
  
  // selected jets: pt, eta, phi, n-tracks, n-vertices, n-electrons, nature (MC info if available),contribution,  pt parton (MC info if available)
  Int_t nbinsVerticesbJets[12]={50,20,20,20,20,5,5,10,60,50,5,5};
  Double_t binlowVerticesbJets[12]={5.,-1.,-TMath::Pi(),0.99,0.99,0.,-0.5,0.,0.,0.,-0.5,-0.5};
  Double_t binupVerticesbJets[12]={55.,1.,TMath::Pi(),20.99,20.99,4.99,4.5,2.,60.,1.,4.5,4.5};
  fSparsebTaggedJets=new THnSparseD("fSparsebTaggedJets","fSparsebTaggedJets;jetpt;eta;phi;ntrks;nRecoVtx;nEle;parton;partContr;ptPart;areaCh;parton2;parton3;",12,nbinsVerticesbJets,binlowVerticesbJets,binupVerticesbJets);

  PostData(1,fNentries);
  PostData(2,fSparseRecoJets);
  PostData(3,fSparseVerticesProp);
  PostData(4,fSparseJetVerticesProp);
  PostData(5,fSparsebTaggedJets);
}



//________________________________________________________________________
void AliAnalysisTaskSEHFJetsOrig::UserExec(Option_t */*option*/){
  
  if(!fbJetArray)fbJetArray=new TClonesArray("AliAODVertex",0);
  Double_t arrDispersion[1000];

  // Execute analysis for current event:
  // heavy flavor candidates association to MC truth
  
  AliAODEvent *aod = dynamic_cast<AliAODEvent*> (InputEvent());
  if (!aod) {
    Printf("ERROR: aod not available");
    return;
  }

  //  TClonesArray *arrayJets;
  TClonesArray *arrayJets = (TClonesArray*)(((AliAnalysisTaskJetCluster*)AliAnalysisManager::GetAnalysisManager()->GetTask("JetClusterAOD_ANTIKT04_B0_Filter00272_Cut00150_Skip00"))->fTCAJetsOut);
  if(!arrayJets) {
    Printf("AliAnalysisTaskSEHFjets::UserExec: Jets branch not found!\n");
    return;
  }

  if(!aod && AODEvent() && IsStandardAOD()) {
    // In case there is an AOD handler writing a standard AOD, use the AOD 
    // event in memory rather than the input (ESD) event.    
    aod = dynamic_cast<AliAODEvent*> (AODEvent());
    // in this case the braches in the deltaAOD (AliAOD.Jets.root)
    // have to taken from the AOD event hold by the AliAODExtension
    AliAODHandler* aodHandler = (AliAODHandler*) 
      ((AliAnalysisManager::GetAnalysisManager())->GetOutputEventHandler());

  }
 
  // SV: ???
  //fNentries->Fill(1);      
  fNentries->Fill(0); // EventsAnal      
  if(!fCutsHFjets->IsEventSelected(aod)){
    fNentries->Fill(1); // EvSel     
    PostData(1,fNentries);
    return;
  }

  if(!arrayJets) {
    printf("AliAnalysisTaskSEHFjets::UserExec: input branch not found!\n");
    return;
  }


  // AOD primary vertex
  AliAODVertex *vtx1 = (AliAODVertex*)aod->GetPrimaryVertex();
  TString primTitle = vtx1->GetTitle();
  if(primTitle.Contains("VertexerTracks") && vtx1->GetNContributors()>0) { 
    
    fNentries->Fill(3); // EvGoodVtxS  
  }
  else {
    PostData(1,fNentries);
    return;
  }


  // MC information
  TClonesArray *arrayMC=0x0;
  AliAODMCHeader *aodmcHeader=0x0;
  Double_t vtxTrue[3];
  
  if(fReadMC){
    // load MC particles
    arrayMC = 
      (TClonesArray*)aod->GetList()->FindObject(AliAODMCParticle::StdBranchName());
    if(!arrayMC) {
      Printf("AliAnalysisTaskSEHFjets::UserExec: MC particles branch not found!\n");
      return;
    }
    // load MC header
    aodmcHeader = 
      (AliAODMCHeader*)aod->GetList()->FindObject(AliAODMCHeader::StdBranchName());
    if(!aodmcHeader) {
      Printf("AliAnalysisTaskSEHFjets::UserExec: MC header branch not found!\n");
      return;
    }
    // MC primary vertex
    aodmcHeader->GetVertex(vtxTrue);
  }




  //AliRDHFCutsJets: contains cuts on: event selections, basic jet cuts (pt, accpetance, emcal, pttrack above given pt)
  //AliRDHFCuts 
  //  AliRDHFCutsJetsVertex: cuts to reconstruct vertices (single track, etc etc)
  // nprong, pt jet, eta, raggio, pt min tracce, electron ID, displacement, cut variables
  Int_t nJets=arrayJets->GetEntries();
  AliAODJet *jet;
  //AliVVertex *vprimary = (AliVVertex*)aod->GetPrimaryVertex();
  Double_t primvtx[3],primcov[6];
  vtx1->GetXYZ(primvtx);
  vtx1->GetCovarianceMatrix(primcov);
  Int_t nPrimContr=vtx1->GetNContributors();
  Double_t chi2=vtx1->GetChi2();

 
  AliESDVertex* v1 = new AliESDVertex(primvtx,primcov,chi2,nPrimContr);

  Double_t magzkG = (Double_t)aod->GetMagneticField(); 

  // Loop on jets
  Int_t nvtx=0;
  for(Int_t jetcand=0;jetcand<nJets;jetcand++){
    nvtx=0;
    jet=(AliAODJet*)arrayJets->UncheckedAt(jetcand);

    if(!fCutsHFjets->IsJetSelected(jet)){
      printf("--> Jet not selected in FindVertices, pt=%f, eta=%f \n", jet->Pt(),jet->Eta());
      continue;
    }

    // Double_t eovp=-99,dedx=-99,pttrack=-99;
    // Bool_t fFlagElec = kFALSE;
    // fCutsHFjets->IsDaugElectron(aod, jet, fFlagElec);

    // if (!fFlagElec){
    //   printf("--> Jet not selected --- no electron");
    //   continue;
    // }

    Double_t contribution=0,ptpart=-1,ptpart2=-1,ptpart3=-1;
    // Nature of the parton (methods 1,2)
    // 1 = gluon (pdg 21)
    // 2 = light quark (pdg < 4)
    // 3 = c (pdg 4)
    // 4 = b (pdg 5)
    // Nature of the meson/barion (method 3)
    // 2 = light
    // 3 = with c
    // 4 = with b
    Int_t partonnat=0;
    Int_t partonnat2=0;
    Int_t partonnat3=0;
    if(fReadMC){ // THIS MAKES SENSE ONLY FOR MC
      // 3 methods to get the parton that produced the jet
      // contribution = pT weight of mother parton (only method 1)
      AliAODMCParticle *parton=IsMCJet(arrayMC,jet,contribution);
      AliAODMCParticle *parton2=( AliAODMCParticle*)IsMCJetParton(arrayMC,jet);
      AliAODMCParticle *parton3=( AliAODMCParticle*)IsMCJetMeson(arrayMC,jet);

      if(parton){
	Int_t pdg=TMath::Abs(parton->PdgCode());
	if(pdg==4 || pdg==5) printf("track method -> pdg parton: %d, contribution =%f *******************=================== \n",pdg,contribution);
	if(pdg==21)partonnat=1;
	else if(pdg<4)partonnat=2;
	else if(pdg==4)partonnat=3;
	else if(pdg==5)partonnat=4;
	ptpart=parton->Pt();
      }

      if(parton2!=0){
	Int_t pdg=TMath::Abs(parton2->PdgCode());
	if(pdg==4 || pdg==5) printf("parton method -> pdg parton: %d *******************=================== \n",pdg);
	if(pdg==21)partonnat2=1;
	else if(pdg<4)partonnat2=2;
	else if(pdg==4)partonnat2=3;
	else if(pdg==5)partonnat2=4;
	ptpart2=parton2->Pt();
      }
     
      if(parton3!=0){
	Int_t pdg=TMath::Abs(parton3->PdgCode());
	if((pdg>=400 && pdg<=600) || (pdg>=4000 && pdg<=6000)) printf("meson method -> pdg parton: %d *******************=================== \n",pdg);
	if((pdg>=400 && pdg<=500) || (pdg>=4000 && pdg<=5000))partonnat3=3;
	else{
	  if((pdg>=500 && pdg<=600) || (pdg>=5000 && pdg<=6000))partonnat3=4;
	  else partonnat3=2;
	}

	ptpart3=parton3->Pt();
      }
  }
   
    TRefArray* reftracks=(TRefArray*)jet->GetRefTracks();
    Int_t ntrks=reftracks->GetEntriesFast();
    if(ntrks<3)continue;

    // Fill container jets
    FillJetRecoHisto(jet,partonnat,partonnat2,partonnat3,contribution,ptpart,ptpart2,ptpart3);
    // Run b-tagger
    // THIS MAKES SENSE ALSO FOR DATA
    nvtx=fTagger->FindVertices(jet,aod,v1,magzkG,fbJetArray,arrDispersion);    
    // printf(" %d vertices, %d array size\n",nvtx,fbJetArray->GetSize());
    if(nvtx>0){
          
      // QA vertici prima di selezione  --> selezione gia` fatta in FindVertices
      // Fill container vertices
      FillVerticesProperties(jet,fbJetArray,arrDispersion,nvtx,vtx1,arrayMC,partonnat,partonnat2,partonnat3);
      // SELECTION ??
      
      // Fill container tagger
      // fTagger->SelectVertices();
      FillJetTaggerHisto(jet,fbJetArray,nvtx,partonnat,partonnat2,partonnat3,contribution,ptpart);

      fbJetArray->Clear();
    }
    else printf(" ****************************************** nvtx=0 !! *****************************************   \n");
  }
  
  
  PostData(1,fNentries);
  PostData(2,fSparseRecoJets);
  PostData(3,fSparseVerticesProp);
  PostData(4,fSparseJetVerticesProp);
  PostData(5,fSparsebTaggedJets);

  delete v1;

}

//---------------------------------------------------------------
 AliAODMCParticle*  AliAnalysisTaskSEHFJetsOrig::IsMCJetParton(TClonesArray *arrayMC,const AliAODJet *jet){
  Int_t MCentries=arrayMC->GetEntries();
  Int_t val=0;
  Double_t ptpart=-1;
  Int_t partonnat=0, partonmom=0, partongmom=0;

  AliAODMCParticle* parton2 = 0;

  Double_t phiPart=-99, etaPart=-99;
  Double_t phiJet=-99, etaJet=-99;
  Double_t dEta=-99, dPhi=-99;
  Double_t dR=-99;
  Int_t countpart[15],countpartcode[15],maxInd=-1,count=0;
  Double_t maxPt=0;
  for(Int_t ii=0;ii<15;ii++){
    countpart[ii]=0;
    countpartcode[ii]=0;
  }
  for(Int_t ii=0;ii<MCentries;ii++){
    AliAODMCParticle* part =  (AliAODMCParticle*)  arrayMC->At(ii);
    if(!part)continue;
    Int_t pdgcode=part->GetPdgCode();
    if(TMath::Abs(pdgcode)==21 || ( TMath::Abs(pdgcode)>=1 && TMath::Abs(pdgcode)<=5)){
     
      ptpart=part->Pt();
  
      etaPart = part->Eta();
      phiPart = part->Phi();
      
      phiJet = jet->Phi();
      etaJet = jet->Eta();
       
      dEta = TMath::Abs(etaJet-etaPart);
      dPhi = TMath::Abs(RelativePhi(phiJet,phiPart));
      
      
      dR = TMath::Sqrt(dEta*dEta + dPhi*dPhi); 
  
      if(dR<0.7){
	if(TMath::Abs(pdgcode)==5) {
	  // cout<<"-> parton returning  Beauty ! "<<endl;
	  return part;
	  
	}
	else{
	  countpartcode[count]=pdgcode;
	  countpart[count]=ii;
	  if(ptpart>maxPt){
	    maxPt=ptpart;
	    maxInd=ii;
	  }
	  count++;
	}
      }
      else{
// 	cout<<"parton OUTSIDE dR --> "<<dR<<"  "<<ii<<" pdgcode="<<pdgcode<<endl;
// 	cout << "pt part = " << ptpart << endl << "pz = " << part->Pz() << endl;	
// 	cout << "eta jet = " << etaJet << endl << "eta part = " << etaPart << endl;	
// 	cout << "phi jet = " << phiJet << endl << "phi part = " << phiPart << endl;
// 	cout << "dPhi = " << dPhi << endl << "dEta2 = " << dEta << endl;

      }
    }
  }

  for(Int_t ij=0;ij<count;ij++){
    if(TMath::Abs(countpartcode[ij])==4) 
    return (AliAODMCParticle*)arrayMC->At(countpart[ij]);

  }
  if(maxInd>-1){
    AliAODMCParticle* partmax = (AliAODMCParticle*)arrayMC->At(maxInd);
   
    return partmax;
  }
  return parton2;
}
//---------------------------------------------------------------
AliAODMCParticle* AliAnalysisTaskSEHFJetsOrig::IsMCJetMeson(TClonesArray *arrayMC,const AliAODJet *jet){

  Int_t MCentries=arrayMC->GetEntries();
  Double_t ptpart=-1;
  Int_t partonnat=0, partonmom=0, partongmom=0;

  AliAODMCParticle* parton3 = 0;

  Double_t phiPart=-99, etaPart=-99;
  Double_t phiJet=-99, etaJet=-99;
  Double_t dEta=-99, dPhi=-99;
  Double_t dR=-99;
  Int_t countpart[15],countpartcode[15],maxInd=-1,count=0;
  Double_t maxPt=0;
  for(Int_t ii=0;ii<15;ii++){
    countpart[ii]=0;
    countpartcode[ii]=0;
  }
  for(Int_t ii=0;ii<MCentries;ii++){
    AliAODMCParticle* part =  (AliAODMCParticle*)  arrayMC->At(ii);
    if(!part)continue;
    Int_t pdgcode=part->GetPdgCode();
    if((TMath::Abs(pdgcode)>=400 && TMath::Abs(pdgcode)<=600)|| (TMath::Abs(pdgcode)>=4000 && TMath::Abs(pdgcode)<=6000)){
      ptpart=part->Pt();
  
      etaPart = part->Eta();
      phiPart = part->Phi();
      
      phiJet = jet->Phi();
      etaJet = jet->Eta();
       
      dEta = TMath::Abs(etaJet-etaPart);
      dPhi = TMath::Abs(RelativePhi(phiJet,phiPart));
      
      
      dR = TMath::Sqrt(dEta*dEta + dPhi*dPhi); 
  
      if(dR<0.7){
	if((TMath::Abs(pdgcode)>=500 && TMath::Abs(pdgcode)<=600)|| (TMath::Abs(pdgcode)>=5000 && TMath::Abs(pdgcode)<=6000)) {
	  // cout<<"-> meson returning  Beauty ! "<<endl;
	  return part;
	}
	else{
	  countpartcode[count]=pdgcode;
	  countpart[count]=ii;
	  if(ptpart>maxPt){
	    maxPt=ptpart;
	    maxInd=ii;
	  }
	  count++;
	}
      }

    }
  }

  for(Int_t ij=0;ij<count;ij++){
    if((TMath::Abs(countpartcode[ij])>=400 && TMath::Abs(countpartcode[ij])<=500) || (TMath::Abs(countpartcode[ij])>=4000 && TMath::Abs(countpartcode[ij])<=5000)) 
      	return (AliAODMCParticle*)arrayMC->At(countpart[ij]);
      
  }
  if(maxInd>-1){
    AliAODMCParticle* partmax = (AliAODMCParticle*)arrayMC->At(maxInd);
    return partmax;
  }
  return parton3;
}

//---------------------------------------------------------------
AliAODMCParticle* AliAnalysisTaskSEHFJetsOrig::IsMCJet(TClonesArray *arrayMC,const AliAODJet *jet, Double_t &contribution){// assignment of parton ID to jet
  // method by L. Feldkamp
  std::vector< int >           idx;
  std::vector< int >           idx2;
  std::vector< double >     weight;

  int counter =0;
  int num = jet->GetRefTracks()->GetEntries();
  
  for(int k=0;k<num;++k){
    
    AliAODTrack * track = (AliAODTrack*)(jet->GetRefTracks()->At(k));
    if (track->GetLabel() >=0){
      AliAODMCParticle* part =  (AliAODMCParticle*)  arrayMC->At(track->GetLabel());
      if(!part)continue;

      int label =0 ;
      AliAODMCParticle* motherParton=GetMCPartonOrigin(arrayMC,part, label);
      if (!motherParton) Printf("no mother");
      else {
	counter++;
	idx.push_back(label);                       //! Label  of Mother
	idx2.push_back(label);        
	weight.push_back(track->Pt());  //! Weight : P_t trak /  P_t jet ... the latter used at the end
	printf(" jet pT=%f,  mother=%d, track w=%f \n",jet->Pt(),motherParton->GetPdgCode(),track->Pt());     
      }
    }///END LOOP OVER REFERENCED TRACKS   
  }
  //! Remove duplicate indices for counting
  sort( idx2.begin(), idx2.end() );
  idx2.erase( unique( idx2.begin(), idx2.end() ), idx2.end() );
  if (idx2.size() == 0) return 0x0;
  Double_t* arrayOfWeights = new Double_t [(int)idx2.size()];
  for(Int_t ii=0;ii<idx2.size();ii++)arrayOfWeights[ii]=0;

  for (int idxloop =0 ;idxloop<idx2.size();idxloop++){
    for (int z=0; z< idx.size() ; ++z){
      int     a = idx.at(z);
      double w = weight.at(z);
      if(a == idx2.at(idxloop))    {
	arrayOfWeights[idxloop] += w;
	//printf("------> arrayOfWeights[%d]=%f \n",idxloop,arrayOfWeights[idxloop]);
      }
    }
  }
  //   for (unsigned int idxloop =0 ;idxloop<idx2.size();idxloop++){
  //     for (unsigned int z=0; z< idx.size() ; ++z){
  //       int     a = idx.at(z);
  //       double w = weight.at(z);
  //       if(a == idx2.at(idxloop))    {
  // 	arrayOfWeights[idxloop] += w;
  // 	printf("------> arrayOfWeights[%d]=%f \n",a,arrayOfWeights[idxloop]);
  // 	  }
  //     }
  //   }
  
  int winner = -1;
  double c=-1.;
  for (unsigned int idxloop =0 ;idxloop<idx2.size();idxloop++){
    if(c < arrayOfWeights[idxloop]){
      winner =idxloop; 
      c=arrayOfWeights[idxloop];
    }
  }
  
  AliAODMCParticle *parton = 0x0;
  parton=(AliAODMCParticle*)arrayMC->At(idx.at(winner));
  contribution = arrayOfWeights[winner]/jet->Pt();
   
  if(arrayOfWeights)    delete arrayOfWeights;


  return parton;
  
}
//---------------------------------------------------------------
AliAODMCParticle *AliAnalysisTaskSEHFJetsOrig::GetMCPartonOrigin(TClonesArray *arrayMC,AliAODMCParticle *p, Int_t &idx)
{  //Follows chain of track mothers until q/g or idx = -1	
  AliAODMCParticle *p3 = 0x0, *p2=0x0;
  Int_t mlabel = TMath::Abs(p->GetMother()) ; 
  Double_t pz=0.;
  while(mlabel > 1){
    p2 = (AliAODMCParticle*)arrayMC->At(mlabel);
    pz=TMath::Abs(p2->Pz());
    //printf("Mother label %d, pdg %d, pz %f\n",mlabel,p2->PdgCode(),pz);
    if( p2->PdgCode() == 21 || (p2->PdgCode() != 0 && abs(p2->PdgCode()) <6) )
      {
	idx = mlabel; 
	return p2;
      }
    mlabel = TMath::Abs(p2->GetMother()); 
  }
  idx=-1;
  return 0x0;

} 
// --------------------------------------------------------------
void AliAnalysisTaskSEHFJetsOrig::GetNPrimAndSecTracksFromCommonVertex(AliAODVertex *vtx,const TClonesArray *mcPart,Int_t &nfromB,Int_t &nfromD){

  Int_t label;
  Int_t *fromB=new Int_t[vtx->GetNDaughters()];
  Int_t *fromDfromB=new Int_t[vtx->GetNDaughters()];
  Int_t *fromSameB=new Int_t[vtx->GetNDaughters()];
  Int_t *fromDfromSameB=new Int_t[vtx->GetNDaughters()];
  Int_t *labmoth=new Int_t[vtx->GetNDaughters()];
  Int_t *labgrmoth=new Int_t[vtx->GetNDaughters()];

  for(Int_t jp=0;jp<vtx->GetNDaughters();jp++){
    fromB[jp]=0;
    fromDfromB[jp]=0;
    fromSameB[jp]=0;
    fromDfromSameB[jp]=0;
  
    labmoth[jp]=0;
    labgrmoth[jp]=0;

    AliAODTrack *tr=(AliAODTrack*)vtx->GetDaughter(jp);  
    label=TMath::Abs(tr->GetLabel());
    if(label<1){
      Printf("Vertex with proton from beam ??? \n");
      continue;
    }

    AliAODMCParticle *mcp=(AliAODMCParticle*)mcPart->At(label);
    if(!mcp)continue;
    
    labmoth[jp]=mcp->GetMother();
    
    AliAODMCParticle* moth=(AliAODMCParticle*)mcPart->At(labmoth[jp]);
    if(!moth)continue;

    if((TMath::Abs(moth->GetPdgCode())>500 && TMath::Abs(moth->GetPdgCode())<600) || (TMath::Abs(moth->GetPdgCode())>5000 && TMath::Abs(moth->GetPdgCode())<6000)){
      fromB[jp]=1; 
      // cout<<"B found -> moth="<<moth->GetPdgCode()<<"  labmoth="<<labmoth[jp]<<endl;
    }
    
    if(TMath::Abs(moth->GetPdgCode())>400 && TMath::Abs(moth->GetPdgCode())<500){
      labgrmoth[jp]=moth->GetMother();
      AliAODMCParticle* grmoth=(AliAODMCParticle*)mcPart->At(labgrmoth[jp]);
      if(!grmoth)continue;

      if((TMath::Abs(grmoth->GetPdgCode())>500 && TMath::Abs(grmoth->GetPdgCode())<600) || (TMath::Abs(grmoth->GetPdgCode())>5000 && TMath::Abs(grmoth->GetPdgCode())<6000)){
	fromDfromB[jp]=1; 
	// cout<<"D found -> moth="<<moth->GetPdgCode()<<"  grmoth="<<grmoth->GetPdgCode()<<"  labgrmoth="<<labgrmoth[jp]<<endl;
      }

    }
    if (fromB[0]==1)fromSameB[0]++;
    if (fromDfromB[0]==1)fromDfromSameB[0]++;
    
    if(jp>=1){
      for (Int_t ii=0;ii<jp;ii++){
	if (fromB[ii]==fromB[jp] && fromB[jp]==1 && labmoth[ii]==labmoth[jp])fromSameB[jp]++;
	if ((fromDfromB[ii]==1 && fromB[jp]==1 && labgrmoth[ii]==labmoth[jp]) || (fromB[ii]==1 && fromDfromB[jp]==1 && labmoth[ii]==labgrmoth[jp]) || (fromDfromB[ii]==fromDfromB[jp] && fromDfromB[jp]==1 && labgrmoth[ii]==labgrmoth[jp]))fromDfromSameB[jp]++;
	
	// cout<<ii<<"  fromsameB and D --> "<<fromSameB[jp]<<"  "<<fromDfromSameB[jp]<<endl;
      }
    }

  }
  Int_t maxB,maxD=0;
  for(Int_t jp=0;jp<vtx->GetNDaughters();jp++){
    if(fromSameB[jp]>maxB)maxB=fromSameB[jp];
    if(fromDfromSameB[jp]>maxD)maxD=fromDfromSameB[jp];

  }
  nfromB=maxB;
  nfromD=maxD;

  // cout<<" MaxB="<<nfromB<<" and maxBD="<<nfromD<<endl;

  delete fromB;
  delete fromDfromB;
  delete fromSameB;
  delete fromDfromSameB;
  delete labmoth;
  delete labgrmoth;

}
// --------------------------------------------------------------
Int_t AliAnalysisTaskSEHFJetsOrig::GetNTracksFromCommonVertex(AliAODVertex *vtx,const TClonesArray *mcPart,Int_t &mcVert,Double_t &xVtxMC,Double_t &yVtxMC,Int_t &nfromBandD,Int_t &nfromD,Int_t &nfromPromptD){
  printf("---- in GetNTracksFromCommonVertex ---- \n");
  Int_t label;

  Double_t *x=new Double_t[vtx->GetNDaughters()];
  Double_t *y=new Double_t[vtx->GetNDaughters()];
  Double_t *z=new Double_t[vtx->GetNDaughters()];
  Int_t *ntrks=new Int_t[vtx->GetNDaughters()];
  Int_t *fromB=new Int_t[vtx->GetNDaughters()];
  Int_t *fromD=new Int_t[vtx->GetNDaughters()];
  Int_t *fromPromptD=new Int_t[vtx->GetNDaughters()];
  Int_t *fromDfromB=new Int_t[vtx->GetNDaughters()];
  Int_t *fromSameB=new Int_t[vtx->GetNDaughters()];
  Int_t *fromDfromSameB=new Int_t[vtx->GetNDaughters()];
  Int_t *labmoth=new Int_t[vtx->GetNDaughters()];
  Int_t *labmothD=new Int_t[vtx->GetNDaughters()];
  Int_t *labgrmoth=new Int_t[vtx->GetNDaughters()];
  Int_t *labgrmothD=new Int_t[vtx->GetNDaughters()];
  Int_t *labgrgrmothD=new Int_t[vtx->GetNDaughters()];
  Int_t *labgrgrmothB=new Int_t[vtx->GetNDaughters()];
  Int_t *labgrgrmothB2=new Int_t[vtx->GetNDaughters()];

  mcVert=0;
  Int_t maxTrks=0;
  Bool_t vtxfound=kFALSE;
  Bool_t vtxPromptDfound=kFALSE;
  Bool_t vtxBDfound=kFALSE;
  Int_t mcBDVert=-1;
  Int_t mcPromptDVert=-1;
  for(Int_t jp=0;jp<vtx->GetNDaughters();jp++){
    fromB[jp]=0;
    fromD[jp]=0;
    fromPromptD[jp]=0;
    fromDfromB[jp]=0;
    fromSameB[jp]=0;
    fromDfromSameB[jp]=0;
    labmoth[jp]=0;
    labgrmoth[jp]=0;
    labmothD[jp]=0;
    labgrmothD[jp]=0;
    labgrgrmothD[jp]=0;
    labgrgrmothB[jp]=0;
    labgrgrmothB2[jp]=0;
  }

  for(Int_t jp=0;jp<vtx->GetNDaughters();jp++){
    ntrks[jp]=0;

    vtxfound=kFALSE;
    vtxBDfound=kFALSE;
    AliAODTrack *tr=(AliAODTrack*)vtx->GetDaughter(jp);  
    label=TMath::Abs(tr->GetLabel());
    if(label<1){
      // Printf("Vertex with proton from beam ??? \n");
      continue;
    }

    AliAODMCParticle *mcp=(AliAODMCParticle*)mcPart->At(label);
    if(!mcp)continue;
    
    labmoth[jp]=mcp->GetMother();
    
    AliAODMCParticle* moth=(AliAODMCParticle*)mcPart->At(labmoth[jp]);
    if(!moth)continue;

    if(TMath::Abs(moth->GetPdgCode()==5) || (TMath::Abs(moth->GetPdgCode())>500 && TMath::Abs(moth->GetPdgCode())<600) || (TMath::Abs(moth->GetPdgCode())>5000 && TMath::Abs(moth->GetPdgCode())<6000)){
      fromB[jp]=1; 
      // cout<<"B found -> moth="<<moth->GetPdgCode()<<"  labmoth="<<labmoth[jp]<<endl;
    }
    
    if((TMath::Abs(moth->GetPdgCode())>400 && TMath::Abs(moth->GetPdgCode())<500)|| (TMath::Abs(moth->GetPdgCode())>4000 && TMath::Abs(moth->GetPdgCode())<5000)){
      labmothD[jp]=labmoth[jp];
      fromD[jp]=1;
      // cout<<"mother D found -> mothD="<<moth->GetPdgCode()<<"  labmothD="<<labmoth[jp]<<endl;
    }

    Int_t label=moth->GetMother();
    AliAODMCParticle* grmothD=(AliAODMCParticle*)mcPart->At(label);
    if(grmothD){
      if((TMath::Abs(grmothD->GetPdgCode())>400 && TMath::Abs(grmothD->GetPdgCode())<500) || (TMath::Abs(grmothD->GetPdgCode())>4000 && TMath::Abs(grmothD->GetPdgCode())<4000)){
	fromD[jp]=1;
	labgrmothD[jp]=label;
	// cout<<jp<<"  grandmother D found -> moth="<<moth->GetPdgCode()<<"  grmoth="<<grmothD->GetPdgCode()<<"  labgrmothD="<<labgrmothD[jp]<<endl;
      }

      Int_t label2=grmothD->GetMother();
      AliAODMCParticle* grgrmothD=(AliAODMCParticle*)mcPart->At(label2);
      if(grgrmothD){
	if((TMath::Abs(grgrmothD->GetPdgCode())>400 && TMath::Abs(grgrmothD->GetPdgCode())<500) || (TMath::Abs(grgrmothD->GetPdgCode())>4000 && TMath::Abs(grgrmothD->GetPdgCode())<4000)){
	  fromD[jp]=1;
	  labgrgrmothD[jp]=label2;
	  // cout<<jp<<"  -->grandgrandmother D found -> moth="<<moth->GetPdgCode()<<"  grgrmoth="<<grgrmothD->GetPdgCode()<<"  labgrgrmothD="<<labgrgrmothD[jp]<<endl;
	  
	}
      }
    }


    if((TMath::Abs(moth->GetPdgCode())>400 && TMath::Abs(moth->GetPdgCode())<500) || (TMath::Abs(moth->GetPdgCode())>4000 && TMath::Abs(moth->GetPdgCode())<5000)){
      labgrmoth[jp]=moth->GetMother();
      AliAODMCParticle* grmoth=(AliAODMCParticle*)mcPart->At(labgrmoth[jp]);
      if(grmoth){
	if(TMath::Abs(grmoth->GetPdgCode()==5) ||(TMath::Abs(grmoth->GetPdgCode())>500 && TMath::Abs(grmoth->GetPdgCode())<600) || (TMath::Abs(grmoth->GetPdgCode())>5000 && TMath::Abs(grmoth->GetPdgCode())<6000)){
	  fromDfromB[jp]=1; 
	  fromD[jp]=0;
	  // cout<<"D found -> moth="<<moth->GetPdgCode()<<"  grmoth="<<grmoth->GetPdgCode()<<"  labgrmoth="<<labgrmoth[jp]<<endl;
	}
      
	labgrgrmothB[jp]=grmoth->GetMother();
	AliAODMCParticle* grgrmoth=(AliAODMCParticle*)mcPart->At(labgrgrmothB[jp]);
	if(grgrmoth){
	  if((TMath::Abs(grgrmoth->GetPdgCode()==5) ||TMath::Abs(grgrmoth->GetPdgCode())>500 && TMath::Abs(grgrmoth->GetPdgCode())<600) || (TMath::Abs(grgrmoth->GetPdgCode())>5000 && TMath::Abs(grgrmoth->GetPdgCode())<6000)){
	    fromDfromB[jp]=1; 
	    fromD[jp]=0;
	    // cout<<"** D found -> moth="<<moth->GetPdgCode()<<"  gratgrmoth="<<grgrmoth->GetPdgCode()<<"  labgreatgrmoth="<<labgrgrmothB[jp]<<endl;
	  }
	  labgrgrmothB2[jp]=grgrmoth->GetMother();
	  AliAODMCParticle* grgrmoth2=(AliAODMCParticle*)mcPart->At(labgrgrmothB2[jp]);
	  if(grgrmoth2){
	    if((TMath::Abs(grgrmoth2->GetPdgCode()==5) ||TMath::Abs(grgrmoth2->GetPdgCode())>500 && TMath::Abs(grgrmoth2->GetPdgCode())<600) || (TMath::Abs(grgrmoth2->GetPdgCode())>5000 && TMath::Abs(grgrmoth2->GetPdgCode())<6000)){
	      fromDfromB[jp]=1; 
	      fromD[jp]=0;
	    }
	    
	  }
	}
      }
    }
    for(Int_t jv=0;jv<mcVert;jv++){
      if(TMath::Abs(x[jv]-mcp->Xv())+TMath::Abs(y[jv]-mcp->Yv())+TMath::Abs(z[jv]-mcp->Zv())<0.0001){
	ntrks[jv]++;
	vtxfound=kTRUE;
	break;
      }
	
    }

    for(Int_t jv=0;jv<jp;jv++){
      if ((fromB[jv]==fromB[jp] && fromB[jp]==1 && labmoth[jv]==labmoth[jp])||(fromDfromB[jv]==1 && fromB[jp]==1 && labgrmoth[jv]==labmoth[jp])){
	fromSameB[mcBDVert]++;
	vtxBDfound=kTRUE;
	break;
      }
      if ((fromB[jv]==1 && fromDfromB[jp]==1 && labmoth[jv]==labgrmoth[jp]) || (fromDfromB[jv]==fromDfromB[jp] && fromDfromB[jp]==1 && labgrmoth[jv]==labgrmoth[jp]) || (fromDfromB[jv]==fromDfromB[jp] && fromDfromB[jp]==1 && labgrmoth[jv]==labgrgrmothB[jp]) || (fromDfromB[jv]==fromDfromB[jp] && fromDfromB[jp]==1 && labgrgrmothB[jv]==labgrmoth[jp]) || (fromB[jv]==1 && fromDfromB[jp]==1 && labmoth[jv]==labgrgrmothB[jp]) || (fromDfromB[jv]==1  && fromB[jp]==1 && labgrgrmothB[jv]==labmoth[jp])   ||  (fromDfromB[jv]==fromDfromB[jp] && fromDfromB[jp]==1 && labgrmoth[jv]==labgrgrmothB2[jp]) || (fromDfromB[jv]==fromDfromB[jp] && fromDfromB[jp]==1 && labgrgrmothB2[jv]==labgrmoth[jp]) || (fromB[jv]==1 && fromDfromB[jp]==1 && labmoth[jv]==labgrgrmothB2[jp]) || (fromDfromB[jv]==1  && fromB[jp]==1 && labgrgrmothB2[jv]==labmoth[jp])){

	
	fromDfromSameB[mcBDVert]++;
	vtxBDfound=kTRUE;

	break;
      }
    }

    if(!vtxBDfound){
      if(fromB[jp]==1 || fromDfromB[jp]==1){
	mcBDVert++;
	if(fromB[jp]==1)fromSameB[mcBDVert]++;
	if(fromDfromB[jp]==1)fromDfromSameB[mcBDVert]++;
      }
    }

    for(Int_t jv=0;jv<jp;jv++){
      if(fromB[jv]==fromB[jp] && fromB[jp]==0 && fromDfromB[jv]==fromDfromB[jp] && fromDfromB[jv]==0 && fromD[jv]==fromD[jp] && fromD[jp]==1){
	if(labgrmothD[jv]==labmothD[jp] || labmothD[jv]==labgrmothD[jp] || labmothD[jv]==labmothD[jp] || labgrgrmothD[jv]==labmothD[jp] || labgrgrmothD[jp]==labmothD[jv] ||  labgrgrmothD[jv]==labgrmothD[jp] || labgrgrmothD[jp]==labgrmothD[jv]){
	  fromPromptD[mcPromptDVert]++;
	  vtxPromptDfound=kTRUE;
	  break;
	}
      }
    }

    if(!vtxfound){
      x[mcVert]=mcp->Xv();
      y[mcVert]=mcp->Yv();
      z[mcVert]=mcp->Zv();
	
      mcVert++;
    }
    if(!vtxPromptDfound){
      if(fromD[jp]==1 &&fromB[jp]==0 && fromDfromB[jp]==0){

	mcPromptDVert++;
	fromPromptD[mcPromptDVert]++;


      }
    }


  }

  for(Int_t jv=0;jv<mcBDVert+1;jv++){
    printf("vertexBD %d, ntracksB=%d, ntracksD=%d\n",jv,fromSameB[jv],fromDfromSameB[jv]);
  }

  for(Int_t jv=0;jv<mcPromptDVert+1;jv++){
    printf("vertexPromptD %d, ntracks=%d \n",jv,fromPromptD[jv]);
  }

  for(Int_t jv=0;jv<mcVert;jv++){
    if(ntrks[jv]>maxTrks){
      maxTrks=ntrks[jv];
      xVtxMC=x[jv];
      yVtxMC=y[jv];
    }
  }

  Int_t max=0;
  for(Int_t jv=0;jv<mcBDVert+1;jv++){
    Int_t sum=fromSameB[jv]+fromDfromSameB[jv];
    if (sum>max){
      max=sum;
      nfromBandD=max;
      nfromD=fromDfromSameB[jv];
    }
  }

  Int_t maxD=0;
  for(Int_t jv=0;jv<mcPromptDVert+1;jv++){
   
    if (fromPromptD[jv]>maxD){
      maxD=fromPromptD[jv];
    }
  }
  nfromPromptD=maxD;

  delete x;
  delete y;
  delete z;
  delete ntrks;
  delete fromB;
  delete fromDfromB;
  delete fromSameB;
  delete fromDfromSameB;
  delete labmoth;
  delete labgrmoth;
  return maxTrks;
}
// --------------------------------------------------------------
// Int_t AliAnalysisTaskSEHFJetsOrig::GetNTracksFromCommonVertex(AliAODVertex *vtx,const TClonesArray *mcPart,Int_t &mcVert,Double_t &xVtxMC,Double_t &yVtxMC){

//   Int_t label;

//   Double_t *x=new Double_t[vtx->GetNDaughters()];
//   Double_t *y=new Double_t[vtx->GetNDaughters()];
//   Double_t *z=new Double_t[vtx->GetNDaughters()];
//   Int_t *ntrks=new Int_t[vtx->GetNDaughters()];
//   mcVert=0;
//   Int_t maxTrks=0;
//   Bool_t vtxfound=kFALSE;
//   for(Int_t jp=0;jp<vtx->GetNDaughters();jp++){
//     ntrks[jp]=0;
//     vtxfound=kFALSE;
//     AliAODTrack *tr=(AliAODTrack*)vtx->GetDaughter(jp);  
//     label=TMath::Abs(tr->GetLabel());
//     if(label<1){
//       Printf("Vertex with proton from beam ??? \n");
//       continue;
//     }

//     AliAODMCParticle *mcp=(AliAODMCParticle*)mcPart->At(label);
//     if(!mcp)continue;
//     for(Int_t jv=0;jv<mcVert;jv++){
//       if(TMath::Abs(x[jv]-mcp->Xv())+TMath::Abs(y[jv]-mcp->Yv())+TMath::Abs(z[jv]-mcp->Zv())<0.0001){
// 	ntrks[jv]++;
// 	vtxfound=kTRUE;
// 	break;
//       }
//     }
//     if(!vtxfound){
// 	x[mcVert]=mcp->Xv();
// 	y[mcVert]=mcp->Yv();
// 	z[mcVert]=mcp->Zv();
//       	mcVert++;
//     }
//   }
//   for(Int_t jv=0;jv<mcVert;jv++){
//     if(ntrks[jv]>maxTrks){
//       maxTrks=ntrks[jv];
//       xVtxMC=x[jv];
//       yVtxMC=y[jv];
//     }
//   }

//   delete x;
//   delete y;
//   delete z;
//   delete ntrks;

//   return maxTrks;
// }

//------------------------------------------------------------
void AliAnalysisTaskSEHFJetsOrig::GetVtxPxy(AliAODVertex *vtx,Double_t *pxyzSum){
  Double_t pxyz[3];
  pxyzSum[0]=0;
  pxyzSum[1]=0;
  pxyzSum[2]=0;
  for(Int_t jp=0;jp<vtx->GetNDaughters();jp++){
    AliAODTrack *tr=(AliAODTrack*)vtx->GetDaughter(jp);
    //      printf("The track %p \n",tr);
    tr->GetPxPyPz(pxyz);
    pxyzSum[0]+=pxyz[0];
    pxyzSum[1]+=pxyz[1];
    pxyzSum[2]+=pxyz[2];
  }
  
  return;
}
//---------------------------------------------------------------
void AliAnalysisTaskSEHFJetsOrig::FillVerticesProperties(const AliAODJet *jet,const TClonesArray *vertices,Double_t* arrDispersion,Int_t nvtx,const AliAODVertex *primVtx,const TClonesArray *mcPart,Int_t partonnat,Int_t partonnat2,Int_t partonnat3){

  Double_t xyz[3],vtxVect[3],jetP[3];
  Double_t xyzPrim[3];
  Double_t cosTheta;

  Double_t pxyzSum[4]={0.,0.,0.,0.};
  primVtx->GetXYZ(xyzPrim);
  jet->PxPyPz(jetP);

  Int_t *indexLxy=new Int_t[nvtx];
  Double_t point[21]={jet->Pt(),nvtx,0.,-1.,-1.,-1.,-1.,-1.,-1.,-1,-1,-1,-1,-1,-1,-1,-1,-1,partonnat,partonnat2,partonnat3};
  Double_t pointVtxProp[18]={jet->Pt(),jet->GetRefTracks()->GetEntriesFast(),0.,0.,0.,0.,partonnat,0,0,0,0,0,0,0,0,0,partonnat2,partonnat3};
    
  Double_t *decLengthXY=new Double_t[nvtx];
  Double_t *invMasses=new Double_t[nvtx];
  Double_t *nRealVtx=new Double_t[nvtx];
  Double_t *nFromBVtx=new Double_t[nvtx];
  Double_t *nFromPromptDVtx=new Double_t[nvtx];
  Double_t xMC,yMC;
  Double_t vtxP[3],vtxPt,signLxy;
  Int_t nvtxMC;
  //  printf("Filling Sparse Histo Vertices: %d vertices \n",vertices->GetSize());
  for(Int_t jj=0;jj<nvtx;jj++){
    //    printf("Getting Vtx %d\n",jj);
    xMC=-99999.;
    yMC=-99999.;
    AliAODVertex *vtx=(AliAODVertex*)vertices->UncheckedAt(jj);
    Double_t chi2ndf=vtx->GetChi2perNDF();
    invMasses[jj]=GetVertexInvariantMass(vtx); 
    Double_t sigvert=arrDispersion[jj];
    nRealVtx[jj]=-1;
    nFromBVtx[jj]=-1;

    vtx->GetXYZ(xyz);
    vtxVect[0]=xyz[0]-xyzPrim[0];
    vtxVect[1]=xyz[1]-xyzPrim[1];
    vtxVect[2]=xyz[2]-xyzPrim[2];
    signLxy=vtxVect[0]*jetP[0]+vtxVect[1]*jetP[1];

    Double_t absJetPt=TMath::Sqrt(jetP[0]*jetP[0]+jetP[1]*jetP[1]);
    Double_t absVtxVect=TMath::Sqrt(vtxVect[0]*vtxVect[0]+vtxVect[1]*vtxVect[1]);
    cosTheta=signLxy/(absJetPt*absVtxVect);//angle between jet and Lxy
    // printf("cosTheta=%f \n",cosTheta);

    decLengthXY[jj]=TMath::Sqrt((xyz[0]-xyzPrim[0])*(xyz[0]-xyzPrim[0])+(xyz[1]-xyzPrim[1])*(xyz[1]-xyzPrim[1]));   
    if(signLxy<0.){
      decLengthXY[jj]*=-1.;
    }
    GetVtxPxy(vtx,vtxP);
    vtxPt=TMath::Sqrt(vtxP[0]*vtxP[0]+vtxP[1]*vtxP[1]);
    pointVtxProp[2]=vtxPt;
    pointVtxProp[3]=invMasses[jj];
    pointVtxProp[4]=decLengthXY[jj];
    pointVtxProp[5]=chi2ndf;
    if(mcPart){
      Int_t nfromBandD=0,nfromD=0,nfromPromptD=0;
      GetNTracksFromCommonVertex(vtx,mcPart,nvtxMC,xMC,yMC,nfromBandD,nfromD,nfromPromptD);
      pointVtxProp[7]=nvtxMC;
      pointVtxProp[8]=xyz[0]-xMC;
      pointVtxProp[9]=xyz[1]-yMC;
      pointVtxProp[14]=nfromBandD;
      pointVtxProp[15]=nfromD;
      // printf("---------------- nfromBandD=%d, nfromD=%d \n",nfromBandD,nfromD);
      nRealVtx[jj]=nvtxMC;
      nFromBVtx[jj]=nfromBandD;
      nFromPromptDVtx[jj]=nfromPromptD;
      if(decLengthXY[jj]>0.2){
	// cout<<"investigate here -> nvtxMC="<<nvtxMC<<"  partonnat="<<partonnat<<endl;
      }
    }

    pointVtxProp[10]=sigvert;
    pointVtxProp[11]=cosTheta*TMath::Abs(decLengthXY[jj]);
    pointVtxProp[12]=TMath::Sqrt(decLengthXY[jj]*decLengthXY[jj]-pointVtxProp[11]*pointVtxProp[11]);
    pointVtxProp[13]=cosTheta;
  
    fSparseVerticesProp->Fill(pointVtxProp);
  }

  TMath::Sort(nvtx,decLengthXY,indexLxy);
  if(nvtx>0){
    point[3]=decLengthXY[indexLxy[0]];
    point[6]=invMasses[indexLxy[0]];
    point[9]=nRealVtx[indexLxy[0]];
    point[12]=nFromBVtx[indexLxy[0]];
    point[15]=nFromPromptDVtx[indexLxy[0]];

  }
  if(nvtx>1){
    point[4]=decLengthXY[indexLxy[1]];
    point[7]=invMasses[indexLxy[1]];
    point[10]=nRealVtx[indexLxy[1]];
    point[13]=nFromBVtx[indexLxy[1]];
    point[16]=nFromPromptDVtx[indexLxy[1]];
  }
  if(nvtx>2){
    point[5]=decLengthXY[indexLxy[2]];
    point[8]=invMasses[indexLxy[2]];
    point[11]=nRealVtx[indexLxy[2]];
    point[14]=nFromBVtx[indexLxy[2]];
    point[17]=nFromPromptDVtx[indexLxy[2]];
  }

  // Calculate sum of inv masses of the 3 more displaced vertices
  for(Int_t ivtx=0;ivtx<3;ivtx++){
    if(nvtx>ivtx) point[2]+=invMasses[indexLxy[ivtx]];    
  }

  //    printf(" mass:%f \n",point[3]);

  fSparseJetVerticesProp->Fill(point);

  delete indexLxy;
  delete decLengthXY;
  delete invMasses;
 
  return;
}

//_____________________________________________________________
Double_t AliAnalysisTaskSEHFJetsOrig::GetVertexInvariantMass(AliAODVertex *vtx,Double_t massParticle){
  Double_t pxyz[3];
  Double_t pxyzSum[4]={0.,0.,0.,0.};
  
  for(Int_t jp=0;jp<vtx->GetNDaughters();jp++){
    AliAODTrack *tr=(AliAODTrack*)vtx->GetDaughter(jp);
    //      printf("The track %p \n",tr);
    tr->GetPxPyPz(pxyz);
    pxyzSum[1]+=pxyz[0];
    pxyzSum[2]+=pxyz[1];
    pxyzSum[3]+=pxyz[2];
    pxyzSum[0]+=TMath::Sqrt(massParticle*massParticle+pxyz[0]*pxyz[0]+pxyz[1]*pxyz[1]+pxyz[2]*pxyz[2]);//pion mass assumed
  }
  //    point[3]=0.;
  return TMath::Sqrt(pxyzSum[0]*pxyzSum[0]-pxyzSum[1]*pxyzSum[1]-pxyzSum[2]*pxyzSum[2]-pxyzSum[3]*pxyzSum[3]);
}

//_____________________________________________________________
void AliAnalysisTaskSEHFJetsOrig::FillJetTaggerHisto(const AliAODJet *jet,const TClonesArray *vertices,Int_t nvtx,Int_t partonnat,Int_t partonnat2,Int_t partonnat3,Double_t contribution,Double_t ptpart){
  //    pt, n-tracks, n-selected vertices , n electrons, nature, pt mother particle  
  //  printf("Jet Cand Found pt=%f, ntr %d\n",jet->Pt(),jet->GetRefTracks()->GetEntriesFast());
 
  Double_t point[12]={jet->Pt(),jet->Eta(),jet->Phi()-TMath::Pi(),jet->GetRefTracks()->GetEntriesFast(),nvtx,0,partonnat,contribution,ptpart,jet->EffectiveAreaCharged(),partonnat2,partonnat3};
  fSparsebTaggedJets->Fill(point);
}

//_____________________________________________________________
void AliAnalysisTaskSEHFJetsOrig::FillJetRecoHisto(const AliAODJet *jet,Int_t partonnat,Int_t partonnat2,Int_t partonnat3,Double_t contribution,Double_t ptpart,Double_t ptpart2,Double_t ptpart3){
  // FIll sparse with reco jets properties
  
  Double_t point[13]={jet->Pt(),jet->Eta(),jet->Phi()-TMath::Pi(),jet->GetRefTracks()->GetEntriesFast(),0,partonnat,partonnat2,partonnat3,contribution,ptpart,ptpart2,ptpart3,jet->EffectiveAreaCharged()};
  fSparseRecoJets->Fill(point);
}

//____________________________________________________________
Int_t AliAnalysisTaskSEHFJetsOrig::TagJetMC(){// METHOD NOT IMPLEMENTED YET
  return -1;
}

//_______________________________________________________________
void AliAnalysisTaskSEHFJetsOrig::Terminate(const Option_t*){
  //TERMINATE METHOD: NOTHING TO DO



}

Double_t AliAnalysisTaskSEHFJetsOrig::RelativePhi(Double_t mphi,Double_t vphi) {
 
  if (vphi < -1*TMath::Pi()) vphi += (2*TMath::Pi());
  else if (vphi > TMath::Pi()) vphi -= (2*TMath::Pi());
  if (mphi < -1*TMath::Pi()) mphi += (2*TMath::Pi());
  else if (mphi > TMath::Pi()) mphi -= (2*TMath::Pi());
  double dphi = mphi-vphi;
  if (dphi < -1*TMath::Pi()) dphi += (2*TMath::Pi());
  else if (dphi > TMath::Pi()) dphi -= (2*TMath::Pi());
 
  return dphi;//dphi in [-Pi, Pi]
}
