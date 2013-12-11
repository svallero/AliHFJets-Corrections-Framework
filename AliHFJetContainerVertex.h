#ifndef AliHFJetContainerVertex_H
#define AliHFJetContainerVertex_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* mailto: svallero@to.infn.it */

/* Class defining containers for the HF b-jets analysis */

#include "AliHFJetContainer.h"
//class AliHFJetContainer {};
//class AliHFJetContainer;
class AliCFContainer;
//class AliHFJetContainer {const char* name, const Int_t nvars, const char* varnames[], Int_t *nbins, Double_t *binning[], const char* axistitle[]};

class AliHFJetContainerVertex : public AliHFJetContainer
{
 public:
  // 4 types of containers: 1) reco jets 2) B jets 3)primary vertex and 4) secondary vertices in a jet
  static const Int_t fgkContTypes=4;
  enum ContType { kJets=0, kBJets, kPrimVtx, kJetVtx };
  const char *strContType(ContType e)
  {
    switch(e) {
      case kJets: return "kJets";
      case kBJets: return "kBJets";
      case kPrimVtx: return "kPrimVtx";
      case kJetVtx: return "kJetVtx";
    }
    return "invalid";
  }
  // Constructor
  AliHFJetContainerVertex(ContType contType=kJets);
  // Destructor
  virtual ~AliHFJetContainerVertex();
  
  //AliHFJetContainer* GetContainer() { return fContainer; }
 
  //void SetContainer(AliHFJetContainer* hist) { fContainer = hist; }

protected:
  ContType fType;                   // container type       
  //AliCFContainer* fContainer;    // custom container 
  void CreateContainerVertex(ContType contType=kJets); // create containers belonging to this class
  Double_t* GetBinningVertex(TString var, Int_t& nBins, const char*& axistitle); // returns array of bin limts for relevant vars
  
  ClassDef(AliHFJetContainerVertex, 1)    // containers for HF b-jets analysis
};

#endif
