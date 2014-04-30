/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
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

// ******************************************
// Manager class for HF jet analysis   
// Author: andrea.rossi@cern.ch, elena.bruna@cern.ch,min.jung.kweon@cern.ch,linus.feldkamp@cern.ch
// *******************************************

#include "TClonesArray.h"
#include "Riostream.h"
#include "AliAODJet.h"
#include "AliVParticle.h"
#include "AliKFVertex.h"
#include "AliESDVertex.h"
#include "AliESDtrack.h"
#include "AliESDtrackCuts.h"
#include "AliRDHFJetsCuts.h"
#include "AliRDHFJetsCutsVertex.h"
#include "AliAODVertex.h"
#include "AliAODEvent.h"
#include "AliVertexerTracks.h"
#include "AliVTrack.h"
#include "AliHFJetVertexTagging.h"

ClassImp(AliHFJetVertexTagging)

AliHFJetVertexTagging::AliHFJetVertexTagging():
fCutsHFjets(0x0){
 
  fTrackArray=new TObjArray();
 
}

AliHFJetVertexTagging::AliHFJetVertexTagging(const char* name):
  fCutsHFjets(0x0){
  
  fTrackArray=new TObjArray();
  
}

AliHFJetVertexTagging::~AliHFJetVertexTagging(){
  if(fTrackArray) {
    fTrackArray->Delete();
    delete fTrackArray;
  }
  delete fCutsHFjets;

}


//__________________________________________________________________________________________________________________
Int_t AliHFJetVertexTagging::FindVertices(const AliAODJet *jet, AliAODEvent* aod, AliESDVertex* v1, Double_t magzkG , TClonesArray *arrVertices, Double_t *arrDispersion){
 
  Int_t nprong=fCutsHFjets->GetNprongs();

  Int_t nvert=0;
  if(!fCutsHFjets->IsJetSelected(jet)){
    printf("--> Jet not selected in FindVertices, pt=%f, eta=%f \n", jet->Pt(),jet->Eta());
    return -1;
  }

  Double_t eovp=-99, dedx=-99, pttrack=-99;
    // Bool_t fFlagElec = kFALSE;
    // fCutsHFjets->IsDaugElectron(aod, jet, fFlagElec);
    // if (!fFlagElec){
    //   printf("--> Jet not selected --- no e trig");
    //   return -1;
    // }

  Int_t iVerticesHF=0;

  arrVertices->Clear();
  TClonesArray &verticesHF = *arrVertices;
  Double_t dispersion=0.;
  for(Int_t ii=0;ii<1000;ii++)arrDispersion[ii]=-999;

  TRefArray* reftracks=(TRefArray*)jet->GetRefTracks();
  fTrackArray->Clear();
  AliESDtrackCuts *esdtrcuts=fCutsHFjets->GetTrackCuts();
  Int_t ntrks=reftracks->GetEntriesFast();
  if(ntrks<2){
    // cout<<"need more than 1 track!"<<endl;
    return 0;
  }
  Int_t up,up2;
  if(nprong==2) {
    up=ntrks-1;
    up2=ntrks;
  }
  if(nprong==3) {
    up=ntrks-2;
    up2=ntrks-1;
  }
  // cout<<"ntrks in jet = "<<ntrks<<endl;


  //make array of ESD tracks, then needed for fTrackArray
  TObjArray* tarresd=new TObjArray();
  tarresd->SetOwner(kTRUE);
  for(Int_t ii=0;ii<ntrks;ii++){
     AliAODTrack* tmpTr = (AliAODTrack*)(jet->GetRefTracks()->At(ii));
     AliESDtrack *esdt = new AliESDtrack(tmpTr);
     tarresd->Add(esdt);
  }
  
  //Double_t 

  for(Int_t it=0;it<up;it++){
    
    AliAODTrack* tmpTr = (AliAODTrack*)(jet->GetRefTracks()->At(it));
    //    Int_t id=(Int_t)TMath::Abs(tmpTr->GetID());

    if(!fCutsHFjets->IsDaughterSelected(tmpTr,v1,esdtrcuts))continue;

    //AliESDtrack *esdt1 = new AliESDtrack(tmpTr);
    AliESDtrack *esdt1 =(AliESDtrack*)tarresd->At(it);

    if(nprong<2){
      //cannot find vertices w/ less than two tracks
      
    }
    else
      for(Int_t it2=it+1;it2<up2;it2++){
	
	AliAODTrack* tmpTr2 = (AliAODTrack*)(jet->GetRefTracks()->At(it2));
	if(!fCutsHFjets->IsDaughterSelected(tmpTr2,v1,esdtrcuts)) continue;

	AliESDtrack *esdt2 =(AliESDtrack*)tarresd->At(it2);
   	
	//fill TClonesArray of tracks
	fTrackArray->AddAt(esdt1,0);
	fTrackArray->AddAt(esdt2,1);

	
	AliAODVertex *vert = 0;
	
 	if(nprong==2) {
	  
 	  AliAODVertex* vert = ReconstructSecondaryVertex(fTrackArray,v1,magzkG,dispersion);

	  //cout<<"************* "<<vert<<endl;
	  //printf("vertex done \n");
	  if(vert){
	  vert->AddDaughter(tmpTr);
	  vert->AddDaughter(tmpTr2);
	  if(!fCutsHFjets->IsVertexSelected(vert,aod,magzkG,dispersion))continue;

	    //	    vert->Print();
	    nvert++;
 	    AliAODVertex* vert2prong= new(verticesHF[iVerticesHF])AliAODVertex(*vert);
	    arrDispersion[iVerticesHF]=dispersion;
	    iVerticesHF++;
 	    // cout<<"================================ vertex 2 prong -> "<<vert2prong->GetX()<<"  "<<vert2prong->GetY()<<"  "<<vert2prong->GetZ()<<"  "<<endl;
	    fTrackArray->Clear();
	  }
 	}
 	if(nprong>=3){
	  for(Int_t it3=it2+1;it3<ntrks;it3++){
	    
	    AliAODTrack* tmpTr3 = (AliAODTrack*)(jet->GetRefTracks()->At(it3));
	    if(!fCutsHFjets->IsDaughterSelected(tmpTr3,v1,esdtrcuts)) continue;
	    AliESDtrack *esdt3 =(AliESDtrack*)tarresd->At(it3);	  
	  
	    fTrackArray->AddAt(esdt3,2);
	    AliAODVertex* vert = ReconstructSecondaryVertex(fTrackArray,v1,magzkG,dispersion);
	    if(vert){
	      vert->AddDaughter(tmpTr);
	      vert->AddDaughter(tmpTr2);
	      vert->AddDaughter(tmpTr3);
	      if(!fCutsHFjets->IsVertexSelected(vert,aod,magzkG,dispersion))continue;
	      nvert++;
	      AliAODVertex* vert3prong= new(verticesHF[iVerticesHF])AliAODVertex(*vert);
	      arrDispersion[iVerticesHF]=dispersion;
	      // cout<<"================================ vertex 3 prong -> "<<vert3prong->GetX()<<"  "<<vert3prong->GetY()<<"  "<<vert3prong->GetZ()<<"  "<<endl;
	      iVerticesHF++;

	    }	    
	   
	  }

	}
	if(nprong>3){
	  //only 2- and 3- track vertices at the moment
	
	}

      }



  }
  fTrackArray->Clear();

  tarresd->Clear();
  delete tarresd;
  return nvert;
}


//-----------------------------------------------------------------------------
AliAODVertex* AliHFJetVertexTagging::ReconstructSecondaryVertex(TObjArray *trkArray, AliESDVertex* v1, Double_t magzkG ,Double_t &dispersion) const
{
  //cout<<"in AliHFJetVertexTagging::ReconstructSecondaryVertex"<<endl;

  // Secondary vertex reconstruction with AliVertexerTracks or AliKFParticle
  //AliCodeTimerAuto("",0);

  AliESDVertex *vertexESD = 0;
  AliAODVertex *vertexAOD = 0;
  
  AliVertexerTracks* vertexerTracks=new AliVertexerTracks(magzkG);
  Int_t nTrks = trkArray->GetEntriesFast();
  //cout<<nTrks<<endl;

  //  trkArray->At(1)->Print();
  //  printf("Mag %f\n",magzkG);
  Int_t secVtxWithKF=fCutsHFjets->GetSecVtxWithKF();
  if(!secVtxWithKF) { // AliVertexerTracks
  
    vertexerTracks->SetVtxStart(v1);//--> need primary vertex!!!
   
    vertexESD = (AliESDVertex*)vertexerTracks->VertexForSelectedESDTracks(trkArray);
   

    if(!vertexESD) {
      // printf("CANNOT BUILD THE VTX \n");
      return vertexAOD;
    }
    //    vertexESD->Print();
    //    cout<<"in AliHFJetVertexTagging::ReconstructSecondaryVertex, nContrib = "<<vertexESD->GetNContributors()<<endl;

    if(vertexESD->GetNContributors()!=trkArray->GetEntriesFast()) {

      //AliDebug(2,"vertexing failed"); 
      delete vertexESD; vertexESD=NULL;
      return vertexAOD;
    }
    
    Double_t vertRadius2=vertexESD->GetXv()*vertexESD->GetXv()+vertexESD->GetYv()*vertexESD->GetYv();
    if(vertRadius2>8.){
      // printf(" vertex outside beam pipe (%f), reject candidate to avoid propagation through material",vertRadius2);
      delete vertexESD; vertexESD=NULL;
      return vertexAOD;
    }

  } else { // Kalman Filter vertexer (AliKFParticle)

    AliKFParticle::SetField(magzkG);//-->  need magnetic field!!!!

    AliKFVertex vertexKF;

    Int_t nTrks = trkArray->GetEntriesFast();
    
    for(Int_t i=0; i<nTrks; i++) {
      AliESDtrack *esdTrack = (AliESDtrack*)trkArray->At(i);
     
      AliKFParticle daughterKF(*esdTrack,211);
      vertexKF.AddDaughter(daughterKF);
    }
    vertexESD = new AliESDVertex(vertexKF.Parameters(),
				 vertexKF.CovarianceMatrix(),
				 vertexKF.GetChi2(),
				 vertexKF.GetNContributors());

  }

  // convert to AliAODVertex
  Double_t pos[3],cov[6],chi2perNDF;
  vertexESD->GetXYZ(pos); // position
  vertexESD->GetCovMatrix(cov); //covariance matrix
  chi2perNDF = vertexESD->GetChi2toNDF();
  dispersion = vertexESD->GetDispersion();
  delete vertexESD; vertexESD=NULL;

  Int_t nprongs= trkArray->GetEntriesFast();
  //cout<<"in AliHFJetVertexTagging::ReconstructSecondaryVertex --> nprongs = "<<nprongs<<endl;
  vertexAOD = new AliAODVertex(pos,cov,chi2perNDF,0x0,-1,AliAODVertex::kUndef,nprongs);
  //cout<<"in AliHFJetVertexTagging::ReconstructSecondaryVertex --> vertexAOD = "<<vertexAOD<<endl;
  //  vertexAOD->Print();
  delete vertexerTracks;
  return vertexAOD;

}

