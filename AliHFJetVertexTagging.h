#ifndef ALIHFJETVERTEXTAGGING_H
#define ALIHFJETVERTEXTAGGING_H

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
#include "AliAODJet.h"
#include "AliAODEvent.h"
#include "AliRDHFJetsCutsVertex.h"

class AliAODVertex;
class AliESDVertex;
class AliVertexerTracks;
class AliESDtrack;
class AliVEvent;

class AliHFJetVertexTagging : public TObject {
 public: 

  AliHFJetVertexTagging();
  AliHFJetVertexTagging(const char* name);
  ~AliHFJetVertexTagging();

  Int_t FindVertices(const AliAODJet *jet,AliAODEvent* aod, AliESDVertex* v1, Double_t magzkG ,TClonesArray *arrVertices, Double_t *arrDispersion);
  AliAODVertex* ReconstructSecondaryVertex(TObjArray *trkArray, AliESDVertex* v1, Double_t magzkG ,Double_t &dispersion) const;

  void SetCuts(AliRDHFJetsCutsVertex *cuts){delete fCutsHFjets;fCutsHFjets=new AliRDHFJetsCutsVertex(*cuts);}
 

 private:
  AliRDHFJetsCutsVertex *fCutsHFjets;  // jet cut object 
  TObjArray *fTrackArray;// 
   
  
 private:
   
  ClassDef(AliHFJetVertexTagging,1);

};
#endif
