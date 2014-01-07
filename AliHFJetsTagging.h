#ifndef ALIHFJETSTAGGING_H
#define ALIHFJETSTAGGING_H

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
#include "AliAODMCParticle.h"
#include "TClonesArray.h"
#include "AliAODJet.h"
#include <TNamed.h>

class AliHFJetsTagging : public TNamed {
  
 public:
  
  AliHFJetsTagging();
  AliHFJetsTagging(const char* name);
  ~AliHFJetsTagging();

  // NOT IMPLEMENTED
  //virtual Bool_t IsHFJet(const AliAODJet *jet);  
  //virtual Bool_t IsHFJet(const AliAODJet *jet, Double_t &probHF,Double_t &probCharm,Double_t &probBeauty,Double_t &probLF);
  //virtual Bool_t IsHFJet(const TClonesArray *aodTrackArray, Double_t &probHF,Double_t &probCharm,Double_t &probBeauty,Double_t &probLF);
  //virtual TClonesArray* SelectTracks();
  //virtual TClonesArray* GetSelectedTracks();
  //virtual TClonesArray* GetReconstructedVertices();
  
  // Moved here from AliAnalysisTaskSEHFJets
  // Methods to identify flavour of MC jets
  AliAODMCParticle* IsMCJet(TClonesArray *arrayMC,const AliAODJet *jet, Double_t &contribution);
  AliAODMCParticle* IsMCJetParton(TClonesArray *arrayMC,const AliAODJet *jet);
  AliAODMCParticle* IsMCJetMeson(TClonesArray *arrayMC,const AliAODJet *jet);

 protected:  
   Double_t RelativePhi(Double_t mphi,Double_t vphi);
   AliAODMCParticle* GetMCPartonOrigin(TClonesArray *arrayMC,AliAODMCParticle *p , Int_t &idx);
  
 private:
  TClonesArray *fSelectedTracks;   // array with selected tracks to be used for tagging
  
  ClassDef(AliHFJetsTagging,1);

};
#endif
