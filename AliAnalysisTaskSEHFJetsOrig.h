#ifndef ALIANALYSISTASKSEHFJETSORIG_H
#define ALIANALYSISTASKSEHFJETSORIG_H

/* Copyright(c) 1998-2009, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//*************************************************************************
// Class AliAnalysisTaskSECharmFraction
// AliAnalysisTask for the extraction of the fraction of prompt charm
// using the charm hadron impact parameter to the primary vertex
//
//
// Author: Andrea Rossi andrea.rossi@cern.ch, Elena Bruna, elena.bruna@to.infn.it
//*************************************************************************

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

#include "AliHFJetVertexTagging.h"

#include "AliAnalysisTaskSE.h"
#include "AliRDHFJetsCuts.h"

class AliAnalysisTaskSEHFJetsOrig : public AliAnalysisTaskSE {
 public:
  AliAnalysisTaskSEHFJetsOrig();
  AliAnalysisTaskSEHFJetsOrig(const char* name);
  virtual ~AliAnalysisTaskSEHFJetsOrig();

 // Implementation of interface methods
  virtual void UserCreateOutputObjects();
  virtual void Init();
  virtual void LocalInit() {Init();}
  virtual void UserExec(Option_t *option);
  virtual void Terminate(Option_t *option);  

  void FillVerticesProperties(const AliAODJet *jet,const TClonesArray *vertices,Double_t* arrDispersion,Int_t nvtx,const AliAODVertex *primVtx,const TClonesArray *mcPart=0x0,Int_t partonnat=-1,Int_t partonnat2=-1,Int_t partonnat3=-1);

  void FillJetTaggerHisto(const AliAODJet *jet,const TClonesArray *vertices,Int_t nvtx,Int_t partonnat,Int_t partonnat2,Int_t partonnat3,Double_t contribution,Double_t ptpart);
  //  void FillJetRecoHisto(const AliAODJet *jet,Int_t partonnat,Double_t contribution,Double_t ptpart);
  void FillJetRecoHisto(const AliAODJet *jet,Int_t partonnat,Int_t partonnat2,Int_t partonnat3,Double_t contribution,Double_t ptpart,Double_t ptpart2,Double_t ptpart3);

  Int_t TagJetMC();
  void SetReadMC(Int_t readMC){fReadMC=readMC;}
  void SetCuts(AliRDHFJetsCuts *cuts){delete fCutsHFjets; fCutsHFjets=new AliRDHFJetsCuts(*cuts);}
  void SetTagger(AliHFJetVertexTagging *tagger){fTagger=new AliHFJetVertexTagging(*tagger);}
  Double_t GetVertexInvariantMass(AliAODVertex *vtx,Double_t massParticle=0.138);
  AliAODMCParticle* GetMCPartonOrigin(TClonesArray *arrayMC,AliAODMCParticle *p , Int_t &idx);
  Int_t GetNTracksFromCommonVertex(AliAODVertex *vtx,const TClonesArray *mcPart,Int_t &mcVert,Double_t &xVtxMC,Double_t &yVtxMC,Int_t &nfromBandD,Int_t &nfromD,Int_t &nfromPromptD);
  void GetNPrimAndSecTracksFromCommonVertex(AliAODVertex *vtx,const TClonesArray *mcPart,Int_t &nfromB,Int_t &nfromD);

  void GetVtxPxy(AliAODVertex *vtx,Double_t *pxyzSum);
  AliAODMCParticle* IsMCJet(TClonesArray *arrayMC,const AliAODJet *jet, Double_t &contribution);
  AliAODMCParticle* IsMCJetParton(TClonesArray *arrayMC,const AliAODJet *jet);
  AliAODMCParticle* IsMCJetMeson(TClonesArray *arrayMC,const AliAODJet *jet);
  Double_t RelativePhi(Double_t mphi,Double_t vphi);

 private:
  AliAnalysisTaskSEHFJetsOrig(const AliAnalysisTaskSEHFJetsOrig&); // copy constructo not implemented yet
  AliAnalysisTaskSEHFJetsOrig& operator=(const AliAnalysisTaskSEHFJetsOrig&); // assignment operator not implemented yet
  Int_t fReadMC;                               // 0=no read mc, 1=is MC but analysis is data-like, 2=MC based analysis
  TH1F *fNentries;                             //! histo for event counting and checks
  // AliRDHFCutsJets *fCutsHFjets;            // jet cut object
  AliRDHFJetsCuts *fCutsHFjets;            // specific algo jet cut object (incude the above?)
  AliHFJetVertexTagging *fTagger;            // Jet Tagging object
  TClonesArray *fbJetArray;                  //! b-tagged jets
  THnSparseD *fSparseRecoJets;            //! sparse histo for reco jet properties
  THnSparseD *fSparseVerticesProp;            //! sparse histo for vertex properties analysis
  THnSparseD *fSparseJetVerticesProp;            //! sparse histo for vertex properties analysis
  THnSparseD *fSparsebTaggedJets;             //! sparse histo for b tagged jet properties


  ClassDef(AliAnalysisTaskSEHFJetsOrig,1); // analysis task for MC study
};

#endif
