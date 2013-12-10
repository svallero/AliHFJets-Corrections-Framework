#ifndef AliHFJetContainerVertex_H
#define AliHFJetContainerVertex_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *  * See cxx source for full Copyright notice                               */

/* mailto: svallero@to.infn.it */

/* Class defining containers for the HF b-jets analysis */

#include "TObject.h"
#include "TNamed.h"
class AliCFContainer;
class TCollection;
class TH1;
class TH1D;
class TH2D;
class TList;
class AliHFJetContainer : public TNamed
{
 public:
  // Analysis steps
  static const Int_t fgkCFSteps = 6;
  enum CFSteps { kCFStepAll = 0, kCFStepTriggered, kCFStepVertex, kCFStepMatchedB, kCFStepMatchedAny, kCFStepRecoB };

  // Variables relevant for corrections
  static const Int_t fgkCFVars = 4;
  enum CFVars { kCFMult = 0, kCFJetPt, kCFJetEta, kCFJetPhi };
  
  // Constructors
  // standard container for spectrum corrections
  AliHFJetContainer(const char* name="AliHFJetContainerStandard");
  // add custom variables to standard, in this way the correction procedure is preserved
  AliHFJetContainer(const char* name, const Int_t nvars, const char* varnames[], Int_t *nbins, Double_t *binning[], const char* axistitle[]);
  AliHFJetContainer(const AliHFJetContainer &c);
  // Destructor
  virtual ~AliHFJetContainer();
  // Assignment operator
  AliHFJetContainer& operator=(const AliHFJetContainer& corr);
  
  // Useful tools
  virtual void Copy(TObject& c) const;

  void ScaleStep(Double_t factor=1., CFSteps step=kCFStepAll);
  void ScaleAllSteps(Double_t factor=1.);
 

  AliCFContainer* GetContainer() { return fContainer; }
 
  void SetContainer(AliCFContainer* hist) { fContainer = hist; }

  // Print variables and steps
  void PrintVars();
  void PrintSteps(); 

  // Set/reset axis ranges
  void SetAxisRangeStep(const char* axisname, Double_t min, Double_t max, CFSteps step);
  void SetAxisRangeAllSteps(const char* axisname, Double_t min, Double_t max);
  void ResetAxisStep(const char* axisname, CFSteps step);
  void ResetAxisAllSteps(const char* axisname);

  // Project 
  TH1D *Project1D(CFSteps step, const char* varname);
  TH2D *Project2D(CFSteps step, const char* varname1, const char* varname2);

  // Get efficiency
  TH1D *GetEfficiencyPt();
  TH1D *GetEfficiencyEta();
  TH2D *GetEfficiency2D();
  
  // Get purity
  TH1D *GetPurityPt();
  TH1D *GetPurityEta();
  TH2D *GetPurity2D();

 protected:
  // for custom container
  TList *fCustomVarNames;        // names of custom variables
  // for standard container
  AliCFContainer* fContainer;    // correction framework for B-jets 
  Int_t fNbins[fgkCFVars];           // number of bins for each variable
  Double_t *fBinning[fgkCFVars];     // set of bins for each variable
  const char *fAxisTitle[fgkCFVars]; // axis title for each variable
  void CreateDefaultBinning();       // sets default axis binning
  const char* GetStepName(CFSteps step); // returns name of enum
  const char* GetVarName(CFVars var);    // returns name of enum
  Int_t GetVarAxis(const char* varname); // returns axis ID given the variable name
  void CreateContainer(TString name, TString title, Int_t nvars, Int_t *nbins, Double_t *binning[], const char *axistitle[]); // create containers belonging to this class
  Double_t* GetBinning(TString var, Int_t& nBins, const char*& axistitle); // returns array of bin limts for relevant vars
  void SetStepNames(AliCFContainer* container); // sets analysis step names
  const char* GetStepTitle(CFSteps step); // gets analysis step names
  TH1* StepsRatio(CFSteps num, CFSteps denom, Int_t var1, Int_t var2=-1); // 1D/2D ratio between 2 steps 
  ClassDef(AliHFJetContainer, 13)    // containers for HF b-jets analysis
};

#endif
