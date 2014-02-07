#ifndef ALIANALYSISTASKSEHFJETS_H
#define ALIANALYSISTASKSEHFJETS_H

/* Copyright(c) 1998-2009, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* Class AliAnalysisTaskSEHFJets:                                    *
 * AliAnalysisTaskSE for the extraction of the B-jet spectrum        *
 * using the properties of secondary verices as tagging observables. */

/* Mailto: andrea.rossi@cern.ch, elena.bruna@to.infn.it, svallero@to.infn.it */

class TH1F;
class TH2F;
class AliAODDEvent;
class AliAODMCHeader;
class AliAODRecoDecayHF2Prong;
class AliAODRecoDecayHF;
class AliAODMCParticle;
class AliAnalysisVertexingHF;
class AliNormalizationCounter;
class AliAODMCParticle;

#include "AliAnalysisTaskSE.h" 
#include "AliHFJetsContainerVertex.h"
#include "AliHFJetsTagging.h"
#include "AliHFJetsTaggingVertex.h"
#include "AliRDHFJetsCuts.h"
#include <TArrayI.h>
#include <TArrayD.h>

#define RED  "\033[22;31;1m"
#define MAG  "\033[22;35;1m"
#define mage  "\033[22;35m"
#define cy  "\033[22;36m"
#define B  "\033[22;30m"

class AliAnalysisTaskSEHFJets : public AliAnalysisTaskSE {
 public:
  AliAnalysisTaskSEHFJets();
  AliAnalysisTaskSEHFJets(const char* name);
  virtual ~AliAnalysisTaskSEHFJets();

  // Implementation of interface methods
  virtual void UserCreateOutputObjects();
  virtual void Init();
  virtual void LocalInit() {Init();}
  virtual void UserExec(Option_t *option);
  virtual void Terminate(Option_t *option);  
 
  // Setters/Getters
  void SetCorrectionsMode(Bool_t mode) {fCorrMode = mode;}
  void SetRecoJetsBranch(const char* branch) {fRecoJetsBranch = branch;}
  void SetMcJetsBranch(const char* branch) {fMcJetsBranch = branch;}

<<<<<<< HEAD
  void SetTagger(AliHFJetsTaggingVertex *tagger){fTagger=(AliHFJetsTaggingVertex*)tagger->Clone("fTagger");}
  void SetCuts(AliRDHFJetsCuts *cuts){delete fCutsHFjets; fCutsHFjets=(AliRDHFJetsCuts*)cuts->Clone("fCutsHFjets");}

=======
  void SetReadMC(Int_t readMC){fReadMC=readMC;}
  void SetCuts(AliRDHFJetsCuts *cuts){delete fCutsHFjets; fCutsHFjets=new AliRDHFJetsCuts(*cuts);}
  void SetTagger(AliHFJetsTaggingVertex *tagger){fTagger=new AliHFJetsTaggingVertex(*tagger);}
  //void SetTagger(AliHFJetsTaggingVertex *tagger){fTagger=tagger;}
>>>>>>> 1dee15b3f1681e0b28ab218380be81768f87986d

 private:
  AliAnalysisTaskSEHFJets(const AliAnalysisTaskSEHFJets&); // copy constructo not implemented yet
  AliAnalysisTaskSEHFJets& operator=(const AliAnalysisTaskSEHFJets&); // assignment operator not implemented yet

  // temporary
  void GetFlavour3Methods(AliAODJet *jet, Double_t (&partonnat)[3], Double_t (&ptpart)[3], Double_t &contribution);

  AliHFJetsContainerVertex *fhJets;    // reco jet properties
  AliHFJetsContainerVertex *fhQaVtx;   // vertices properties
  AliHFJetsContainerVertex *fhBJets;   // B-jet properties
  AliHFJetsContainerVertex *fhJetVtx;  // properties of vertices within the jet 

  Bool_t fCorrMode;                    // enable correction or data modes
  const char* fRecoJetsBranch;            // name of the AOD RECO-jets branch  
  const char* fMcJetsBranch;             // name of the AOD MC-jets branch  

  TH1F *fNentries;                     //! histo for event counting and checks
  AliHFJetsTaggingVertex *fTagger;     // Jet Tagging object
  AliRDHFJetsCuts *fCutsHFjets;        // specific algo jet cut object 

  TClonesArray *fbJetArray;            //! b-tagged jets
  TClonesArray *fArrayMC;              // array of MC particles for given event

  ClassDef(AliAnalysisTaskSEHFJets,1); // analysis task for MC study
};

#endif
