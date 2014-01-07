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

/* mailto: svallero@to.infn.it */

/* Class defining containers for the HF b-jets analysis */

#include "AliHFJetsContainer.h"
#include "AliCFContainer.h"
#include "THnSparse.h"
#include "TMath.h"
#include "TList.h"
#include "TObjString.h"
#include "TCollection.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "AliLog.h"
#include "TCanvas.h"
#include "TF1.h"
#include "AliTHn.h"
#include "THn.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//system( "color 5B" );

ClassImp(AliHFJetsContainer)

//const Int_t AliHFJetsContainer::fgkCFSteps = 6;
//const Int_t AliHFJetsContainer::fgkCFVars = 4;

//AliHFJetsContainer::AliHFJetsContainer(): 
//  TNamed("",""),
//  fCustomVarNames(0x0),
//  fContainer(0)
//{
//  // Dummy constructor for daughter classes
//  AliInfo("Creating dummy container."); 
//}

AliHFJetsContainer::AliHFJetsContainer(const char* name, Bool_t dummy): 
  TNamed(name,name),
  fCustomVarNames(0x0),
  fContainer(0)
{
  CreateDefaultBinning();
  
  if (!dummy){
  
    // Constructor
    AliInfo(MAG"Creating default container."B); 
  
  
    CreateContainer("fContainerStandard", "Standard container for corrections", fgkCFVars, fNbins, fBinning, fAxisTitle);  

    // Delete arrays 
    //for (Int_t k=0; k<nvars; k++){
      //delete binning[k];
      //delete axistitle[k];
    //}
  }
}

AliHFJetsContainer::AliHFJetsContainer(const char* name, const Int_t nvars, const char* varnames[], Int_t *nbins, Double_t *binning[], const char*  axistitle[]): 
  TNamed(name,name),
  fCustomVarNames(0x0),
  fContainer(0)
{

  CreateCustomContainer(nvars, varnames, nbins, binning, axistitle);

}

//----------------------------------------------------------------
AliHFJetsContainer::AliHFJetsContainer(const AliHFJetsContainer &c) :
  TNamed(),
  fContainer(0)
{

  // AliHFJetsContainer copy constructor

  ((AliHFJetsContainer &) c).Copy(*this);
}

//----------------------------------------------------------------
AliHFJetsContainer::~AliHFJetsContainer()
{
  // Destructor
     
  if (fContainer)
  {
    delete fContainer;
    fContainer = 0;
    }
  
  for (Int_t k=0; k<fgkCFVars; k++){
    //if (fBinning[k]){
      //delete fBinning[k];
        //fBinning[k] = 0;
    //}
    //if (fAxisTitle[k]){
      //delete fAxisTitle[k];
      //fAxisTitle[k] = 0;
    //}
  }
}


//----------------------------------------------------------------
AliHFJetsContainer &AliHFJetsContainer::operator=(const AliHFJetsContainer &c)
{
  // assigment operator

  if (this != &c)
    ((AliHFJetsContainer &) c).Copy(*this);

  return *this;
}

//----------------------------------------------------------------
void AliHFJetsContainer::Copy(TObject& c) const
{
  // copy function

  AliHFJetsContainer& target = (AliHFJetsContainer &) c;

  if (fContainer)
    target.fContainer = dynamic_cast<AliCFContainer*> (fContainer->Clone());
  if (fCustomVarNames)
    target.fCustomVarNames = dynamic_cast<TList*> (fCustomVarNames->Clone());
  if (fCustomVarNames)
    target.fCustomVarNames = dynamic_cast<TList*> (fCustomVarNames->Clone());
  
}

//----------------------------------------------------------------
void AliHFJetsContainer::CreateContainer(TString name, TString title, Int_t nvars, Int_t *nbins, Double_t *binning[], const char *axistitle[])
{

  fContainer = new AliCFContainer(name, title, fgkCFSteps,nvars,nbins);
  for (Int_t j=0; j<nvars; j++){
     fContainer->SetBinLimits(j, binning[j]);
     fContainer->SetVarTitle(j, axistitle[j]);
  }

  SetStepNames(fContainer);

}


//----------------------------------------------------------------
void AliHFJetsContainer::CreateCustomContainer(const Int_t nvars, const char* varnames[], Int_t *nbins, Double_t *binning[], const char*  axistitle[]){

  AliInfo(MAG"Creating custom container: standard variables will be added at positions 0,1,2,3!"B);

  CreateDefaultBinning();

  const Int_t totnvars = nvars + fgkCFVars; 
  Int_t totnbins[nvars + fgkCFVars];
  Double_t *totbinning[totnvars+1];  
  const char *totaxistitle[totnvars]; 
  for (Int_t i=0; i<fgkCFVars; i++){
    totnbins[i]=fNbins[i];
    totbinning[i]=fBinning[i];
    totaxistitle[i]=fAxisTitle[i];
    AliDebug(AliLog::kDebug,Form(mage"Standard vars: ID \"%d\" NAME \"%s\" TITLE \"%s\""B, i, GetVarName((CFVars)i), totaxistitle[i])) ;
  }

  // Write custom variables names in a global list,
  // to ease SetAxis and Project functions
  fCustomVarNames = new TList();
  fCustomVarNames->SetOwner(kTRUE);
  for (Int_t j=fgkCFVars; j<totnvars; j++){
    totnbins[j]=nbins[j-fgkCFVars];
    totbinning[j]=binning[j-fgkCFVars];
    totaxistitle[j]=axistitle[j-fgkCFVars];
    TObjString sobj(varnames[j-fgkCFVars]);
    fCustomVarNames->Add((TObjString*)sobj.Clone());
    AliDebug(AliLog::kDebug,Form(cy"Custom vars: ID \"%d\" NAME \"%s\" TITLE \"%s\""B, j, GetVarName((CFVars) j), totaxistitle[j]));
  }
  CreateContainer("fContainerCustom", "Custom container for corrections", totnvars, totnbins, totbinning, totaxistitle);  
  //delete fCustomVarNames;
}
//----------------------------------------------------------------
Double_t* AliHFJetsContainer::GetBinning(TString var, Int_t& nBins, const char*& axistitle)
{
  // Assigns variable-specific bin edges 
  // (you can define array of bins "by hand" to allow for non uniform bin width) 
  //if (var.Contains("DP") || var.Contains("BP") || var.Contains("BH") ){
  //  if (var.Contains("part")) var.Form("idPart");
  //  else if (var.Contains("pt")) var.Form("ptPart");
  //}
 
  Float_t binmin=0., binmax=0.;    
  if (var.EqualTo("mult")){
      axistitle="Multiplicity";
      nBins = 100; binmin= 0.5; binmax= 100.5;
  } else if (var.EqualTo("jetPt")){
      axistitle="p_{T,jet} (GeV/c)";
      nBins = 50; binmin= 5.; binmax= 55.;
      // Double_t *bins = {5.,10.,15., ...};
      // return bins;
  } else if (var.EqualTo("jetEta")){
      axistitle="#eta_{jet}";
      nBins = 20; binmin= -1.; binmax= 1.;
  } else if (var.EqualTo("jetPhi")){
      axistitle="#phi_{jet} (rad)";
      nBins = 20; binmin= -TMath::Pi(); binmax= TMath::Pi();
  } else {
      AliError(Form(RED"Variable %s not defined!"B, var.Data()));
      }
  

  // Define regular binning
  Double_t binwidth = (binmax-binmin)/(1.*nBins);
  Double_t* bins = new Double_t[nBins+1];
  for (Int_t j=0; j<nBins+1; j++){
     if (j==0) bins[j]= binmin;
     else bins[j] = bins[j-1]+binwidth;
     //Printf("*** Bin %d value %f",j, bins[j]);
     }
  return bins;
}

void AliHFJetsContainer::SetAxisRangeStep(const char* axisname, Double_t min, Double_t max, CFSteps step)
{
  
  AliInfo(Form(MAG"Setting range for axis: \"%s\" step: \"%s\""B, axisname, GetStepName(step)));
  Int_t axis = GetVarAxis(axisname); 
  //AliInfo(Form("Resetting axis %d", axis));
   
  fContainer->GetAxis(axis, step)->SetRangeUser(min,max);
   
}

void AliHFJetsContainer::SetAxisRangeAllSteps(const char* axisname, Double_t min, Double_t max)
{

  for (Int_t i=0; i<fgkCFSteps; i++){
     SetAxisRangeStep(axisname, min, max, (CFSteps) i);
  }  

}  

void AliHFJetsContainer::PrintVars()
{
  Int_t nvars=fContainer->GetNVar();
  for (Int_t i=0; i<nvars; i++){
     Printf(cy"Var %d: %s -> %s"B,i, GetVarName((CFVars) i), fContainer->GetVarTitle(i));
  }
}  

void AliHFJetsContainer::PrintSteps()
{
  Int_t nstep=fContainer->GetNStep();
  for (Int_t i=0; i<nstep; i++){
     Printf(mage"Step %d: %s -> %s"B,i, GetStepName((CFSteps) i), GetStepTitle((CFSteps) i));
  }
}

void AliHFJetsContainer::ResetAxisStep(const char* axisname, CFSteps step)
{
  
  AliInfo(Form(MAG"Resetting range for axis: \"%s\" step: \"%s\""B, axisname, GetStepName(step)));
  Int_t axis = GetVarAxis(axisname); 
  fContainer->GetAxis(axis, step)->SetRange(0, -1);

}

void AliHFJetsContainer::ResetAxisAllSteps(const char* axisname)
{

  for (Int_t i=0; i<fgkCFSteps; i++){
     ResetAxisStep(axisname,(CFSteps) i);
  }  

}  

TH1D *AliHFJetsContainer::Project1D(CFSteps step, const char* varname)
{
 
   AliInfo(Form(MAG"Projecting axis: \"%s\" step: \"%s\""B, varname, GetStepName(step)));
   Int_t var = GetVarAxis(varname);  
   TH1D* h1=(TH1D*)fContainer->Project(step, var);
    
   return h1;
}

TH2D *AliHFJetsContainer::Project2D(CFSteps step, const char* varname1, const char* varname2)
{
 
   AliInfo(Form(MAG"Projecting axis: \"%s\" and \"%s\"  step: \"%s\""B, varname1, varname2, GetStepName(step)));
  
   Int_t var1 = GetVarAxis(varname1);  
   Int_t var2 = GetVarAxis(varname2);

   TH2D* h2=(TH2D*)fContainer->Project(step, var1, var2);
    
   return h2;
}

void AliHFJetsContainer::ScaleStep(Double_t factor, CFSteps step)
{
   Double_t fact[2] = {factor,0};
    
   AliInfo(Form(MAG"Scaling container at step %d..."B, step));
   fContainer->GetGrid(step)->Scale(fact);

}


void AliHFJetsContainer::ScaleAllSteps(Double_t factor)
{
  for (Int_t i=0; i<fgkCFSteps; i++){
    ScaleStep(factor,(CFSteps) i);
  }
}  

void AliHFJetsContainer::SetStepNames(AliCFContainer* container)
{
    // sets the names of the correction steps

    for (Int_t i=0; i<fgkCFSteps; i++)
      container->SetStepTitle(i, GetStepTitle((CFSteps) i));
    }

const char* AliHFJetsContainer::GetStepTitle(CFSteps step)
{
    // returns the name of the given step
    switch (step){
      case kCFStepAll:
        return "All events";
      case kCFStepTriggered:
        return "Triggered";
      case kCFStepVertex:
        return "Primary vertex";
      case kCFStepMatchedB:
        return "True B-jets (reco B-jet matches true B-jet)";
      case kCFStepMatchedAny:
        return "True B-jets (reco B-jet matches any true jet)";
      case kCFStepRecoB:
        return "Reconstructed B-jets";
        }

      return 0;
}

const char* AliHFJetsContainer::GetStepName(CFSteps step)
{
  switch (step){
    case kCFStepAll:
      return "kCFStepAll";
    case kCFStepTriggered:
      return "kCFStepTriggered";
    case kCFStepVertex:
      return "kCFStepVertex";
    case kCFStepMatchedB:
      return "kCFStepMatchedB";
    case kCFStepMatchedAny:
      return "kCFStepMatchedAny";
    case kCFStepRecoB:
      return "kCFStepRecoB";
   }
  return 0;
}

const char* AliHFJetsContainer::GetVarName(CFVars var)
{ 

  switch (var){
  case kCFMult:
    return "kCFMult";
  case kCFJetPt:
    return "kCFJetPt";
  case kCFJetEta:
    return "kCFJetEta";
  case kCFJetPhi:
    return "kCFJetPhi";
  default:
    //const char* value=Form("var%d",var);
    TObjString *sname = (TObjString*)fCustomVarNames->At(var-fgkCFVars);
    const char* value = sname->GetName();
    return value;
  }
  return 0;
}  

Int_t AliHFJetsContainer::GetVarAxis(const char* varname){
  if (!strncmp(varname,"kCFMult",50))
    return kCFMult;
  else if (!strncmp(varname,"kCFJetPt",50))
    return kCFJetPt;
  else if (!strncmp(varname,"kCFJetEta",50))
    return kCFJetEta;
  else if (!strncmp(varname,"kCFJetPhi",50))
    return kCFJetPhi;
  else {
    //const char* value=Form("var%d",var);
    TObjString *obj = (TObjString*)fCustomVarNames->FindObject(varname);
    if (!obj) {
      AliError(Form(RED"Variable \"%s\" does not exist!"B,varname));
      exit(0);
    }
    Int_t value = fCustomVarNames->IndexOf((TObjString*)obj);
    //Printf("%d", value);
    value+=fgkCFVars;
    return value;
    //return 1;
  }

}

void AliHFJetsContainer::CreateDefaultBinning()
{
  TString vars("mult;jetPt;jetEta;jetPhi");
  // Get binning for each variable
  TObjArray *arr;
  TObjString *objstr;
  arr = vars.Tokenize(";");
  //static const Int_t nvars = arr->GetEntriesFast();
  Int_t nvars = arr->GetEntriesFast();
  if (nvars != fgkCFVars)AliError(RED"Number of initialized variables is not fgkCFVars!"B);
  //Int_t nbins[4];       // number of bins for each variable
  //const char* axistitle[4]; // axis title for each variable
  //Double_t *binning[4]; // array of bins for each variable
  TIter next(arr);
  Int_t i = 0;
  while ((objstr=(TObjString*)next())){
    fBinning[i]=GetBinning(objstr->GetString(), fNbins[i], fAxisTitle[i]);
    i++;
    }
}
TH1* AliHFJetsContainer::StepsRatio(CFSteps num, CFSteps denom, Int_t var1, Int_t var2)
{
  TH1 *hnum=0;
  TH1 *hdenom=0;
  if (var2 >= 0){
    hnum=fContainer->Project(num, var1, var2);
    hdenom=fContainer->Project(denom, var1, var2);
  } else {
    hnum=fContainer->Project(num, var1);
    hdenom=fContainer->Project(denom, var1);
  }
  //hnum->Divide(hnum, hdenom, 1, 1, "B"); // capire opzione "B"
  hnum->Divide(hnum, hdenom);
  delete hdenom;
  return hnum;
}

TH1D* AliHFJetsContainer::GetEfficiencyPt()
{
  TH1 *h = StepsRatio(kCFStepMatchedB, kCFStepVertex, kCFJetPt);
  h->SetTitle("kCFStepMatchedB/kCFStepVertex");
  return dynamic_cast<TH1D*>(h);
}

TH1D* AliHFJetsContainer::GetEfficiencyEta()
{
  TH1 *h = StepsRatio(kCFStepMatchedB, kCFStepVertex, kCFJetEta);
  h->SetTitle("kCFStepMatchedB/kCFStepVertex");
  return dynamic_cast<TH1D*>(h);
}

TH2D* AliHFJetsContainer::GetEfficiency2D()
{
  TH1 *h = StepsRatio(kCFStepMatchedB, kCFStepVertex, kCFJetPt, kCFJetEta);
  h->SetTitle("kCFStepMatchedB/kCFStepVertex");
  return dynamic_cast<TH2D*>(h);
}

TH1D* AliHFJetsContainer::GetPurityPt()
{
  TH1 *h = StepsRatio(kCFStepMatchedAny, kCFStepMatchedB, kCFJetPt);
  h->SetTitle("kCFStepMatchedAny/kCFStepMatchedB");
  return dynamic_cast<TH1D*>(h);
}

TH1D* AliHFJetsContainer::GetPurityEta()
{
  TH1 *h = StepsRatio(kCFStepMatchedAny, kCFStepMatchedB, kCFJetEta);
  h->SetTitle("kCFStepMatchedAny/kCFStepMatchedB");
  return dynamic_cast<TH1D*>(h);
}

TH2D* AliHFJetsContainer::GetPurity2D()
{
  TH1 *h = StepsRatio(kCFStepMatchedAny, kCFStepMatchedB, kCFJetPt, kCFJetEta);
  h->SetTitle("kCFStepMatchedAny/kCFStepMatchedB");
  return dynamic_cast<TH2D*>(h);
}

void AliHFJetsContainer::FillStep(CFSteps step,const TArrayD *point){

    Int_t expectedvars=fCustomVarNames->GetEntries()+4;
    Int_t givenvars=point->GetSize();

    if (expectedvars!=givenvars){
      AliError(Form(RED"Wrong number of values: expected %d, provided %d!"B, expectedvars, givenvars));
    }

    fContainer->Fill(point->GetArray(), step, 1.);
}
