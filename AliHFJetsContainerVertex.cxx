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

#include "AliHFJetsTaggingVertex.h"
#include "AliHFJetsContainerVertex.h"
#include "AliHFJetsContainer.h"
#include "AliAODJet.h"
#include "AliAODVertex.h"
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
#include "TClonesArray.h"

ClassImp(AliHFJetsContainerVertex)

AliHFJetsContainerVertex::AliHFJetsContainerVertex(): 
  AliHFJetsContainer("",kTRUE),
  fType(kJets)
{
  fTagger=new AliHFJetsTaggingVertex("tagger");
  // dummy
}

AliHFJetsContainerVertex::AliHFJetsContainerVertex(const char* name,ContType contType): 
  AliHFJetsContainer(name,kTRUE),
  fType(kJets),
  fTagger(0x0)
{

  // Constructor
  fTagger=new AliHFJetsTaggingVertex("tagger");
  CreateContainerVertex(contType); 
}

//----------------------------------------------------------------
AliHFJetsContainerVertex::AliHFJetsContainerVertex(const AliHFJetsContainerVertex &c) :
  AliHFJetsContainer("",kTRUE)
{

  // AliHFJetsContainerVertex copy constructor

  ((AliHFJetsContainerVertex &) c).Copy(*this);
}

//----------------------------------------------------------------
AliHFJetsContainerVertex &AliHFJetsContainerVertex::operator=(const AliHFJetsContainerVertex &c)
{
  // assigment operator

  if (this != &c)
    ((AliHFJetsContainerVertex &) c).Copy(*this);

  return *this;
}
//----------------------------------------------------------------
AliHFJetsContainerVertex::~AliHFJetsContainerVertex()
{

  // Destructor
  //if (fTagger)
    //delete fTagger;

}

//----------------------------------------------------------------
void AliHFJetsContainerVertex::Copy(TObject& c) const
{
  // copy function

  AliHFJetsContainerVertex& target = (AliHFJetsContainerVertex &) c;
  if (fType)
    target.fType = fType;
   
}
//----------------------------------------------------------------
void AliHFJetsContainerVertex::CreateContainerVertex(ContType contType)
{
  TString stype, vars;
  fType=contType;
  switch (contType) {
    case 0:
      stype.Form("Jets properties.");
      //AliInfo(Form("Creating AliHFJetsContainerVertex of type: %s ", stype.Data()));
      // Relevant variables
      vars.Form("nTrk;nEle;partDP;partBP;partBH;partContr;ptDP;ptBP;ptBH;areaCh");
      break;
    case 1:
      stype.Form("B-jets properties.");
      //AliInfo(Form("Creating AliHFJetsContainerVertex of type: %s ", stype.Data()));
      // Relevant variables
      vars.Form("nTrk;nRecoVtx;nEle;partDP;partContr;ptDP;areaCh;partBP;partBH");
      break;
    case 2:
      stype.Form("Primary vertexes properties.");
      //AliInfo(Form("Creating AliHFJetsContainerVertex of type: %s ", stype.Data()));
      // Relevant variables
      vars.Form("nTrk;ptVtx;mass;Lxy;chi2/ndf;partDP;nRealVtx;deltaX;deltaY;sigVtx;LxyJet;LxyJetPerp;cosTheta;nFromB;nFromBD;partBP;partBH");
      break;
    case 3:
      stype.Form("Jet vertexes proprties.");
      //AliInfo(Form("Creating AliHFJetsContainerVertex of type: %s ", stype.Data()));
      // Relevant variables
      vars.Form("nRecoVtx;SumMass3MostDispl;Lxy1;Lxy2;Lxy3;mass1;mass2;mass3;nReal1;nReal2;nReal3;nFromB1;nFromB2;nFromB3;nFromPrD1;nFromPrD2;nFromPrD3;partDP;partBP;partBH");
      break;
    default:
      AliError("Not a valid container type!");
      return;
  }

  AliInfo(Form(mage"Container type set to %s: %s"B, strContType(contType), stype.Data())); 
  // Get binning for each variable
  TObjArray *arr;
  //arr->SetOwner(kTRUE);
  TObjString *objstr;
  arr = vars.Tokenize(";");
  //static const Int_t nvars = arr->GetEntriesFast();
  Int_t nvars = arr->GetEntriesFast();
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
  
  AliHFJetsContainer::CreateCustomContainer(nvars,axisname,nbins, binning,axistitle);

  // Delete arrays 
  arr->Clear();
  //delete arr;
  for (Int_t k=0; k<nvars; k++){
    //delete [] binning[k];
    //delete axisname[k];
    //delete axistitle[k];
 }
}

//----------------------------------------------------------------
Double_t* AliHFJetsContainerVertex::GetBinningVertex(TString var, Int_t& nBins, const char*& axistitle)
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
  } else if (var.EqualTo("areaCh")){
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
      axistitle="#chi^{2}/NDF";
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
      AliError(Form("Variable %s not defined!", var.Data()));
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

void AliHFJetsContainerVertex::FillStepJets(AliHFJetsContainer::CFSteps step,Int_t mult, const AliAODJet *jet,Int_t p[3],Double_t contr,Double_t pt[3]){
  
  if (fType != kJets){
    AliError(Form(RED"This method is available only for container type kJets: you are trying to fill %s!"B, strContType(fType)));
  }

  // here the number of electrons is not filled (after nTrk)
  Double_t point[14]={mult,jet->Pt(),jet->Eta(),jet->Phi()-TMath::Pi(),jet->GetRefTracks()->GetEntriesFast(),0,p[0],p[1],p[2],contr,pt[0],pt[1],pt[2],jet->EffectiveAreaCharged()};
  TArrayD *apoint=new TArrayD(14,point);
  AliHFJetsContainer::FillStep(step, apoint);
}

void AliHFJetsContainerVertex::FillStepQaVtx(AliHFJetsContainer::CFSteps step, Int_t mult, const AliAODJet *jet,const TClonesArray *vertices,Double_t* disp,Int_t nvtx,const AliAODVertex *primVtx,const TClonesArray *mcPart,Int_t p[3]){
  
  if (fType != kQaVtx){
    AliError(Form(RED"This method is available only for container type kQaVtx: you are trying to fill %s!"B, strContType(fType)));
  }
  
    Double_t xyz[3],vtxVect[3],jetP[3];
    Double_t xyzPrim[3];
    Double_t cosTheta;

    primVtx->GetXYZ(xyzPrim);
    jet->PxPyPz(jetP);

    Int_t *indexLxy=new Int_t[nvtx];
    Double_t pointVtxProp[21]={mult,jet->Pt(),jet->Eta(),jet->Phi(),jet->GetRefTracks()->GetEntriesFast(),0.,0.,0.,0.,p[0],0,0,0,0,0,0,0,0,0,p[1],p[2]};

     Double_t *decLengthXY=new Double_t[nvtx];
     Double_t *invMasses=new Double_t[nvtx];
     Double_t *nRealVtx=new Double_t[nvtx];
     Double_t *nFromBVtx=new Double_t[nvtx];
     Double_t *nFromPromptDVtx=new Double_t[nvtx];
     Double_t xMC,yMC;
     Double_t vtxP[3],vtxPt,signLxy;
     Int_t nvtxMC=0;

     for(Int_t jj=0;jj<nvtx;jj++){
       xMC=-99999.;
       yMC=-99999.;
       AliAODVertex *vtx=(AliAODVertex*)vertices->UncheckedAt(jj);
       //cout << vertices << endl;
       Double_t chi2ndf=vtx->GetChi2perNDF();
       invMasses[jj]=fTagger->GetVertexInvariantMass(vtx);
       Double_t sigvert=disp[jj];
       nRealVtx[jj]=-1;
       nFromBVtx[jj]=-1;

       vtx->GetXYZ(xyz);
       vtxVect[0]=xyz[0]-xyzPrim[0];
       vtxVect[1]=xyz[1]-xyzPrim[1];
       vtxVect[2]=xyz[2]-xyzPrim[2];
       signLxy=vtxVect[0]*jetP[0]+vtxVect[1]*jetP[1];

       Double_t absJetPt=TMath::Sqrt(jetP[0]*jetP[0]+jetP[1]*jetP[1]);
       Double_t absVtxVect=TMath::Sqrt(vtxVect[0]*vtxVect[0]+vtxVect[1]*vtxVect[1]);
       cosTheta=signLxy/(absJetPt*absVtxVect);//angle between jet and Lxy

       decLengthXY[jj]=TMath::Sqrt((xyz[0]-xyzPrim[0])*(xyz[0]-xyzPrim[0])+(xyz[1]-xyzPrim[1])*(xyz[1]-xyzPrim[1]));
       if(signLxy<0.){
         decLengthXY[jj]*=-1.;
       }
        
       fTagger->GetVtxPxy(vtx,vtxP);
       vtxPt=TMath::Sqrt(vtxP[0]*vtxP[0]+vtxP[1]*vtxP[1]);
       pointVtxProp[5]=vtxPt;
       pointVtxProp[6]=invMasses[jj];
       pointVtxProp[7]=decLengthXY[jj];
       pointVtxProp[8]=chi2ndf;
       
       if(mcPart){
         Int_t nfromBandD=0,nfromD=0,nfromPromptD=0;
         fTagger->GetNTracksFromCommonVertex(vtx,mcPart,nvtxMC,xMC,yMC,nfromBandD,nfromD,nfromPromptD);
         pointVtxProp[10]=nvtxMC;
         pointVtxProp[11]=xyz[0]-xMC;
         pointVtxProp[12]=xyz[1]-yMC;
         pointVtxProp[17]=nfromBandD;
         pointVtxProp[18]=nfromD;

         nRealVtx[jj]=nvtxMC;
         nFromBVtx[jj]=nfromBandD;
         nFromPromptDVtx[jj]=nfromPromptD;
         //if(decLengthXY[jj]>0.2){
         //  }
         }
        
       pointVtxProp[13]=sigvert;
       pointVtxProp[14]=cosTheta*TMath::Abs(decLengthXY[jj]);
       pointVtxProp[15]=TMath::Sqrt(decLengthXY[jj]*decLengthXY[jj]-pointVtxProp[14]*pointVtxProp[14]);
       pointVtxProp[16]=cosTheta;
       TArrayD *apointVtxProp=new TArrayD(21,pointVtxProp);
       AliHFJetsContainer::FillStep(step, apointVtxProp);
       }
          
     delete indexLxy;
     delete decLengthXY;
     delete invMasses;

     return;
}
  
void AliHFJetsContainerVertex::FillStepBJets(AliHFJetsContainer::CFSteps step, Int_t mult, const AliAODJet *jet, Int_t nvtx,Int_t partonnat[3], Double_t contribution,Double_t ptpart){
  
  AliInfo("Filling container kBJets.");

  if (fType != kBJets){
    AliError(Form(RED"This method is available only for container type kBJets: you are trying to fill %s!"B, strContType(fType)));
  }

  Double_t point[13]={mult, jet->Pt(),jet->Eta(),jet->Phi()-TMath::Pi(),jet->GetRefTracks()->GetEntriesFast(),nvtx,0,partonnat[0],contribution,ptpart,jet->EffectiveAreaCharged(), partonnat[1], partonnat[2]};
  //for (Int_t i=0; i<13; i++) Printf("Point %d %f", i, point[i]);
  TArrayD *apoint=new TArrayD(13,point);
  AliHFJetsContainer::FillStep(step, apoint);
}

void AliHFJetsContainerVertex::FillStepJetVtx(AliHFJetsContainer::CFSteps step, Int_t mult, const AliAODJet *jet,const TClonesArray *vertices,Int_t nvtx,const AliAODVertex *primVtx,const TClonesArray *mcPart,Int_t p[3]){
  
  if (fType != kJetVtx){
    AliError(Form(RED"This method is available only for container type kJetVtx: you are trying to fill %s!"B, strContType(fType)));
  }
  
    Double_t xyz[3],vtxVect[3],jetP[3];
    Double_t xyzPrim[3];
    Double_t cosTheta;

    primVtx->GetXYZ(xyzPrim);
    jet->PxPyPz(jetP);

    Int_t *indexLxy=new Int_t[nvtx];
    Double_t point[24]={mult,jet->Pt(),jet->Eta(),jet->Phi(),nvtx,0.,-1.,-1.,-1.,-1.,-1.,-1.,-1,-1,-1,-1,-1,-1,-1,-1,-1,p[0],p[1],p[2]};

     Double_t *decLengthXY=new Double_t[nvtx];
     Double_t *invMasses=new Double_t[nvtx];
     Double_t *nRealVtx=new Double_t[nvtx];
     Double_t *nFromBVtx=new Double_t[nvtx];
     Double_t *nFromPromptDVtx=new Double_t[nvtx];
     Double_t xMC,yMC;
     Double_t vtxP[3],vtxPt,signLxy;
     Int_t nvtxMC=0;

     for(Int_t jj=0;jj<nvtx;jj++){
       xMC=-99999.;
       yMC=-99999.;
       AliAODVertex *vtx=(AliAODVertex*)vertices->UncheckedAt(jj);
       invMasses[jj]=fTagger->GetVertexInvariantMass(vtx);
       nRealVtx[jj]=-1;
       nFromBVtx[jj]=-1;

       vtx->GetXYZ(xyz);
       vtxVect[0]=xyz[0]-xyzPrim[0];
       vtxVect[1]=xyz[1]-xyzPrim[1];
       vtxVect[2]=xyz[2]-xyzPrim[2];
       signLxy=vtxVect[0]*jetP[0]+vtxVect[1]*jetP[1];

       Double_t absJetPt=TMath::Sqrt(jetP[0]*jetP[0]+jetP[1]*jetP[1]);
       Double_t absVtxVect=TMath::Sqrt(vtxVect[0]*vtxVect[0]+vtxVect[1]*vtxVect[1]);
       cosTheta=signLxy/(absJetPt*absVtxVect);//angle between jet and Lxy

       decLengthXY[jj]=TMath::Sqrt((xyz[0]-xyzPrim[0])*(xyz[0]-xyzPrim[0])+(xyz[1]-xyzPrim[1])*(xyz[1]-xyzPrim[1]));
       if(signLxy<0.){
         decLengthXY[jj]*=-1.;
       }
        
       fTagger->GetVtxPxy(vtx,vtxP);
       vtxPt=TMath::Sqrt(vtxP[0]*vtxP[0]+vtxP[1]*vtxP[1]);
       
       if(mcPart){
         Int_t nfromBandD=0,nfromD=0,nfromPromptD=0;
         fTagger->GetNTracksFromCommonVertex(vtx,mcPart,nvtxMC,xMC,yMC,nfromBandD,nfromD,nfromPromptD);

         nRealVtx[jj]=nvtxMC;
         nFromBVtx[jj]=nfromBandD;
         nFromPromptDVtx[jj]=nfromPromptD;
         //if(decLengthXY[jj]>0.2){
         //  }
         }
        
       }
          
     TMath::Sort(nvtx,decLengthXY,indexLxy);
     if(nvtx>0){
       point[6]=decLengthXY[indexLxy[0]];
       point[9]=invMasses[indexLxy[0]];
       point[12]=nRealVtx[indexLxy[0]];
       point[15]=nFromBVtx[indexLxy[0]];
       point[18]=nFromPromptDVtx[indexLxy[0]];
       }

     if(nvtx>1){
       point[7]=decLengthXY[indexLxy[1]];
       point[10]=invMasses[indexLxy[1]];
       point[13]=nRealVtx[indexLxy[1]];
       point[16]=nFromBVtx[indexLxy[1]];
       point[19]=nFromPromptDVtx[indexLxy[1]];
       }

     if(nvtx>2){
       point[8]=decLengthXY[indexLxy[2]];
       point[11]=invMasses[indexLxy[2]];
       point[14]=nRealVtx[indexLxy[2]];
       point[17]=nFromBVtx[indexLxy[2]];
       point[20]=nFromPromptDVtx[indexLxy[2]];
       }

     // Calculate sum of inv masses of the 3 more displaced vertices
     for(Int_t ivtx=0;ivtx<3;ivtx++){
       if(nvtx>ivtx) point[5]+=invMasses[indexLxy[ivtx]];
       }

     TArrayD *apoint=new TArrayD(24,point);
     AliHFJetsContainer::FillStep(step, apoint);

     delete indexLxy;
     delete decLengthXY;
     delete invMasses;

     return;
}

