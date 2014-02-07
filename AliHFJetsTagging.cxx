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

/* Manager class for HF jet analysis   */

/* Mailto: andrea.rossi@cern.ch,       *
 *         elena.bruna@cern.ch,        *
 *         min.jung.kweon@cern.ch,     *
 *         linus.feldkamp@cern.ch,     *
 *         svallero@to.infn.it         */

#include "AliAODJet.h"
#include "AliAODEvent.h"
#include "AliHFJetsTagging.h"
#include "AliLog.h"
#include <TNamed.h>

ClassImp(AliHFJetsTagging)

AliHFJetsTagging::AliHFJetsTagging():
    TNamed(),
    fSelectedTracks(0x0){
      // method non implemented
    }

AliHFJetsTagging::AliHFJetsTagging(const char* name): 
  TNamed(name,name),
  fSelectedTracks(0x0){

    // method non implemented

}

AliHFJetsTagging::~AliHFJetsTagging(){
  //delete fSelectedTracks;
}

AliHFJetsTagging &AliHFJetsTagging::operator=(const AliHFJetsTagging &c)
{
  // assigment operator

  if (this != &c)
    ((AliHFJetsTagging &) c).Copy(*this);

  return *this;
}

//Bool_t AliHFJetsTagging::IsHFJet(const AliAODJet *jet){
//  // METHOD NON IMPlEMENTED
//  return kFALSE;
//}

//Bool_t AliHFJetsTagging::IsHFJet(const AliAODJet *jet, Double_t &probHF,Double_t &probCharm,Double_t &probBeauty,Double_t &probLF){
//  // METHOD NON IMPlEMENTED
//  return kFALSE;
//}

//Bool_t AliHFJetsTagging::IsHFJet(const TClonesArray *aodTrackArray, Double_t &probHF,Double_t &probCharm,Double_t &probBeauty,Double_t &probLF){
//  // METHOD NON IMPlEMENTED
//  return kFALSE;
//}

//TClonesArray* AliHFJetsTagging::SelectTracks(){
//  // METHOD NON IMPlEMENTED
//  return 0x0;
//}

//TClonesArray* AliHFJetsTagging::GetSelectedTracks(){
//  // METHOD NON IMPlEMENTED
//  return 0x0;
//}

//TClonesArray* AliHFJetsTagging::GetReconstructedVertices(){
//  // METHOD NON IMPlEMENTED
//  return 0x0;
//}


//AliHFJetsTagging::AliHFJetsTagging();
//AliHFJetsTagging::AliHFJetsTagging(const char* name);

AliAODMCParticle* AliHFJetsTagging::IsMCJet(TClonesArray *arrayMC,const AliAODJet *jet, Double_t &contribution){

  // assignment of parton ID to jet (method by L. Feldkamp)

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
      if (!motherParton) AliDebug(AliLog::kDebug,"no mother");
      else {
        counter++;
        idx.push_back(label);           //! Label  of Mother
        idx2.push_back(label);
        weight.push_back(track->Pt());  //! Weight : P_t trak /  P_t jet ... the latter used at the end
        //AliDebug(AliLog::kDebug,Form("jet pT=%f,  mother=%d, track w=%f \n",jet->Pt(),motherParton->GetPdgCode(),track->Pt()));
      }
    }///END LOOP OVER REFERENCED TRACKS   
  }

  //! Remove duplicate indices for counting
  sort( idx2.begin(), idx2.end() );
  idx2.erase( unique( idx2.begin(), idx2.end() ), idx2.end() );
  
  if (idx2.size() == 0) return 0x0;
  Double_t* arrayOfWeights = new Double_t [(int)idx2.size()];
  //for(Int_t ii=0;ii<idx2.size();ii++)arrayOfWeights[ii]=0;
  for(unsigned long ii=0;ii<idx2.size();ii++)arrayOfWeights[ii]=0;

  //for (int idxloop =0 ;idxloop<idx2.size();idxloop++){
  //  for (int z=0; z< idx.size() ; ++z){
  for (unsigned long idxloop =0 ;idxloop<idx2.size();idxloop++){
    for (unsigned long z=0; z< idx.size() ; ++z){
      int     a = idx.at(z);
      double w = weight.at(z);
      if(a == idx2.at(idxloop))    {
        arrayOfWeights[idxloop] += w;
      }
    }
  }

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

  if(arrayOfWeights)    delete [] arrayOfWeights;


  return parton;

}

AliAODMCParticle *AliHFJetsTagging::GetMCPartonOrigin(TClonesArray *arrayMC,AliAODMCParticle *p, Int_t &idx)
{  
  //Follows chain of track mothers until q/g or idx = -1	
  //AliAODMCParticle *p3 = 0x0, *p2=0x0;
  AliAODMCParticle *p2=0x0;
  Int_t mlabel = TMath::Abs(p->GetMother()) ; 
  //Double_t pz=0.;
  while(mlabel > 1){
    p2 = (AliAODMCParticle*)arrayMC->At(mlabel);
    //pz=TMath::Abs(p2->Pz());

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

AliAODMCParticle*  AliHFJetsTagging::IsMCJetParton(TClonesArray *arrayMC,const AliAODJet *jet){

  Int_t MCentries=arrayMC->GetEntries();
  //Int_t val=0;
  Double_t ptpart=-1;
  //Int_t partonnat=0, partonmom=0, partongmom=0;

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
        //	cout<<"parton OUTSIDE dR --> "<<dR<<"  "<<ii<<" pdgcode="<<pdgcode<<endl;
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

Double_t AliHFJetsTagging::RelativePhi(Double_t mphi,Double_t vphi) {

  if (vphi < -1*TMath::Pi()) vphi += (2*TMath::Pi());
  else if (vphi > TMath::Pi()) vphi -= (2*TMath::Pi());
  if (mphi < -1*TMath::Pi()) mphi += (2*TMath::Pi());
  else if (mphi > TMath::Pi()) mphi -= (2*TMath::Pi());
  double dphi = mphi-vphi;
  if (dphi < -1*TMath::Pi()) dphi += (2*TMath::Pi());
  else if (dphi > TMath::Pi()) dphi -= (2*TMath::Pi());

  return dphi;//dphi in [-Pi, Pi]
}

AliAODMCParticle* AliHFJetsTagging::IsMCJetMeson(TClonesArray *arrayMC,const AliAODJet *jet){

  Int_t MCentries=arrayMC->GetEntries();
  Double_t ptpart=-1;
  //Int_t partonnat=0, partonmom=0, partongmom=0;

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

