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

#include "AliHFJetContainerVertex.h"
#include "AliHFJetContainer.h"
#include "THnSparse.h"
#include "TMath.h"
#include "TList.h"
#include "TCollection.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "AliLog.h"
#include "TCanvas.h"
#include "TF1.h"
#include "AliTHn.h"
#include "THn.h"

ClassImp(AliHFJetContainerVertex)

AliHFJetContainerVertex::AliHFJetContainerVertex(ContType contType): 
  AliHFJetContainer(),
  fType(kJets)
  //fContainer(0)
{
  // Constructor
  CreateContainerVertex(contType);  
}

//----------------------------------------------------------------
AliHFJetContainerVertex::~AliHFJetContainerVertex()
{
  // Destructor
     
  //if (fContainer)
  //{
  //  delete fContainer;
  //  fContainer = 0;
  // }
  
}
//----------------------------------------------------------------
void AliHFJetContainerVertex::CreateContainerVertex(ContType contType)
{
  TString stype, vars;
  fType=contType;
  switch (contType) {
    case 0:
      stype.Form("Jets properties.");
      //AliInfo(Form("Creating AliHFJetContainerVertex of type: %s ", stype.Data()));
      // Relevant variables
      vars.Form("nTrk;nEle;partDP;partBP;partBH;partContr;ptDP;ptBP;ptBH;areaCh");
      break;
    case 1:
      stype.Form("B-jets properties.");
      //AliInfo(Form("Creating AliHFJetContainerVertex of type: %s ", stype.Data()));
      // Relevant variables
      vars.Form("nTrk;nRecoVtx;nEle;partDP;partContr;ptDP;AreaCh;partBP;partBH");
      break;
    case 2:
      stype.Form("Primary vertexes properties.");
      //AliInfo(Form("Creating AliHFJetContainerVertex of type: %s ", stype.Data()));
      // Relevant variables
      vars.Form("nTrk;ptVtx;mass;Lxy;chi2/ndf;partDP;nRealVtx;deltaX;deltaY;sigVtx;LxyJet;LxyJetPerp;cosTheta;nFromB;nFromBD;partBP;partBH");
      break;
    case 3:
      stype.Form("Jet vertexes proprties.");
      //AliInfo(Form("Creating AliHFJetContainerVertex of type: %s ", stype.Data()));
      // Relevant variables
      vars.Form("nRecoVtx;SumMass3MostDispl;Lxy1;Lxy2;Lxy3;mass1;mass2;mass3;nReal1;nReal2;nReal3;nFromB1;nFromB2;nFromB3;nFromPrD1;nFromPrD2;nFromPrD3;partDP;partBP;partBH");
      break;
    default:
      AliError("Not a valid container type!");
      return;
  }
  AliInfo(Form(mag"Container type set to %s: %s"B, strContType(contType), stype.Data())); 
  // Get binning for each variable
  TObjArray *arr;
  TObjString *objstr;
  arr = vars.Tokenize(";");
  //static const Int_t nvars = arr->GetEntriesFast();
  static const Int_t nvars = arr->GetEntriesFast();
  Int_t nbins[nvars];       // number of bins for each variable
  const char* axistitle[nvars]; // axis title for each variable
  const char* axisname[nvars]; // axis name for each variable
  Double_t *binning[nvars]; // array of bins for each variable
  TIter next(arr);
  Int_t i = 0;
  while ((objstr=(TObjString*)next())){
    binning[i]=GetBinningVertex(objstr->GetString(), nbins[i], axistitle[i]);
    axisname[i]=objstr->GetName();
    i++;
    }
  
  AliHFJetContainer::CreateCustomContainer(nvars,axisname,nbins, binning,axistitle);

}
//----------------------------------------------------------------
Double_t* AliHFJetContainerVertex::GetBinningVertex(TString var, Int_t& nBins, const char*& axistitle)
{
  // Assigns variable-specific bin edges 
  // (you can define array of bins "by hand" to allow for non uniform bin width) 
  //if (var.Contains("DP") || var.Contains("BP") || var.Contains("BH") ){
  //  if (var.Contains("part")) var.Form("idPart");
  //  else if (var.Contains("pt")) var.Form("ptPart");
  //}
 
  Float_t binmin=0., binmax=0.;    
  if (var.EqualTo("jetPt")){
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
  } else if (var.EqualTo("nTrk")){
      axistitle="N_{trk}";
      //nBins = 20; binmin= 0.; binmax= 20.; // changed SV
      nBins = 20; binmin= 0.99; binmax= 20.99; // changed SV
  } else if (var.EqualTo("nEle")){
      axistitle="N_{ele}";
      //(nBins = 5; binmin= -0.01; binmax= 4.99; // changed by SV
      nBins = 5; binmin= 0.; binmax= 4.99;
  } else if (var.EqualTo("partDP")){
      axistitle="ID_{parton} DP";
      nBins = 5; binmin= -0.5; binmax= 4.5;
  } else if (var.EqualTo("partBP")){
      axistitle="ID_{parton} BP";
      nBins = 5; binmin= -0.5; binmax= 4.5;
  } else if (var.EqualTo("partBH")){
      axistitle="ID_{parton} BH";
      nBins = 5; binmin= -0.5; binmax= 4.5;
  } else if (var.EqualTo("partContr")){
      axistitle="Contribution";
      nBins = 10; binmin= 0.; binmax= 2.;
  } else if (var.EqualTo("ptDP")){
      axistitle="p_{T,part} DP (GeV/c)";
      nBins = 60; binmin= 0.; binmax= 60.;
  } else if (var.EqualTo("ptBP")){
      axistitle="p_{T,part} BP (GeV/c)";
      nBins = 60; binmin= 0.; binmax= 60.;
  } else if (var.EqualTo("ptBH")){
      axistitle="p_{T,part} BH (GeV/c)";
      nBins = 60; binmin= 0.; binmax= 60.;
  } else if (var.EqualTo("AreaCh")){
      axistitle="Charged area";
      nBins = 50; binmin= 0.; binmax= 1.;
  } else if (var.EqualTo("ptVtx")){
      axistitle="p_{T,vtx} (GeV/c)";
      nBins = 20; binmin= 0.; binmax= 20.;
  } else if (var.EqualTo("mass")){
      axistitle="mass (GeV/c^{2})";
      nBins = 20; binmin= 0.; binmax= 5.;
  } else if (var.EqualTo("mass1")){
      axistitle="mass_{1} (GeV/c^{2})";
      nBins = 20; binmin= 0.; binmax= 5.;
  } else if (var.EqualTo("mass2")){
      axistitle="mass_{2} (GeV/c^{2})";
      nBins = 20; binmin= 0.; binmax= 5.;
  } else if (var.EqualTo("mass3")){
      axistitle="mass_{3} (GeV/c^{2})";
      nBins = 20; binmin= 0.; binmax= 5.;
  } else if (var.EqualTo("SumMass3MostDispl")){
      axistitle="#Sigma mass (GeV/c^{2})";
      nBins = 20; binmin= 0.; binmax= 20.;
  } else if (var.EqualTo("Lxy")){
      axistitle="L_{xy} (cm)";
      nBins = 200; binmin= -1.; binmax= 1.;
  } else if (var.EqualTo("Lxy1")){
      axistitle="L_{xy,1} (cm)";
      nBins = 50; binmin= 0.; binmax= 1.;
  } else if (var.EqualTo("Lxy2")){
      axistitle="L_{xy,2} (cm)";
      nBins = 50; binmin= 0.; binmax= 0.5;
  } else if (var.EqualTo("Lxy3")){
      axistitle="L_{xy,3} (cm)";
      nBins = 50; binmin= 0.; binmax= 0.5;
  } else if (var.EqualTo("chi2/ndf")){
      axistitle="#xi^{2}/NDF";
      nBins = 10; binmin= 0.; binmax= 10.;
  } else if (var.EqualTo("nRealVtx")){
      axistitle="N_{vtx,real}";
      nBins = 5; binmin= -0.5; binmax= 4.5;
  } else if (var.EqualTo("nRecoVtx")){
      axistitle="N_{vtx,reco}";
      //nBins = 20; binmin= 0.; binmax= 20.; // changed by SV
      nBins = 20; binmin= 0.99; binmax= 20.99;  // changed by SV
  } else if (var.EqualTo("deltaX")){
      axistitle="#Delta x (cm)";
      nBins = 15; binmin= -0.03; binmax= 0.03;
  } else if (var.EqualTo("deltaY")){
      axistitle="#Delta y (cm)";
      nBins = 15; binmin= -0.03; binmax= 0.03;
  } else if (var.EqualTo("sigVtx")){
      axistitle="#sigma_{vtx}";
      nBins = 20; binmin= 0.; binmax= 0.1;
  } else if (var.EqualTo("LxyJet")){
      axistitle="#L_{xy,jet} (cm)";
      nBins = 100; binmin= -1.; binmax= 1.;
  } else if (var.EqualTo("LxyJetPerp")){
      axistitle="#L_{xy,jet} #perp (cm)";
      nBins = 30; binmin= 0.; binmax= 0.2;
  } else if (var.EqualTo("cosTheta")){
      axistitle="cos#theta";
      nBins = 50; binmin= -1.; binmax= 1.;
  } else if (var.EqualTo("nFromB")){
      axistitle="N (from B)";
      nBins = 5; binmin= -0.5; binmax= 4.5;
  } else if (var.EqualTo("nFromB1")){
      axistitle="N (from B1)";
      nBins = 5; binmin= -0.5; binmax= 4.5;
  } else if (var.EqualTo("nFromB2")){
      axistitle="N (from B2)";
      nBins = 5; binmin= -0.5; binmax= 4.5;
  } else if (var.EqualTo("nFromB3")){
      axistitle="N (from B3)";
      nBins = 5; binmin= -0.5; binmax= 4.5;
  } else if (var.EqualTo("nFromBD")){
      axistitle="N (from D from B)";
      nBins = 5; binmin= -0.5; binmax= 4.5;
  } else if (var.EqualTo("nFromPrD1")){
      axistitle="N (from prompt D1)";
      nBins = 5; binmin= -0.5; binmax= 4.5;
  } else if (var.EqualTo("nFromPrD2")){
      axistitle="N (from prompt D2)";
      nBins = 5; binmin= -0.5; binmax= 4.5;
  } else if (var.EqualTo("nFromPrD3")){
      axistitle="N (from prompt D3)";
      nBins = 5; binmin= -0.5; binmax= 4.5;
  } else if (var.EqualTo("nReal1")){
      axistitle="N_{real,1}";
      nBins = 5; binmin= -0.5; binmax= 4.5;
  } else if (var.EqualTo("nReal2")){
      axistitle="N_{real,2}";
      nBins = 5; binmin= -0.5; binmax= 4.5;
  } else if (var.EqualTo("nReal3")){
      axistitle="N_{real,3}";
      nBins = 5; binmin= -0.5; binmax= 4.5;
  } else {
      //AliError(Form("Variable %s not defined!", var.Data()));
      }
  

  // Define regular binning
  Double_t binwidth = (binmax-binmin)/(1.*nBins);
  Double_t* bins = new Double_t[nBins+1];
  for (Int_t j=0; j<nBins+1; j++){
     if (j==0) bins[j]= binmin;
     else bins[j] = bins[j-1]+binwidth;
     }
  return bins;
}

