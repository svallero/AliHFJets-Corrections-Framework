

#include <THnSparse.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TDirectoryFile.h>
#include <TAxis.h>
#include <TStyle.h>
#include <Riostream.h>



TH1D* makeProjection(TString name,THnSparse* hsparse,Int_t axparton,Int_t parton,Int_t axdecay,Double_t startdecay,Int_t col,Int_t wid,Int_t axvariable);
TH1D* makeProjectionpt(TString name,THnSparse* hsparse,Int_t axparton,Int_t parton,Int_t axvariable);
TH1D* makeProjectionMulti(TString name,THnSparse* hsparse,Int_t axparton,Int_t startparton,Int_t endparton,Int_t axdecay,Double_t startdecay,Int_t axmass,Double_t startmass,Int_t col,Int_t wid,Int_t axvariable);
TH1D* makeProjectionptall(TString name,THnSparse* hsparse,Int_t axparton,Int_t startparton,Int_t endparton,Int_t axdecay,Double_t startdecay,Double_t enddecay,Int_t col,Int_t wid,Int_t axvariable);
void comparePtJetQuark();
void loopForEff(Int_t partonmethod, Double_t minpt,Double_t maxpt);


THnSparseF* fSparseRecoJets;
THnSparseF* fSparseVerticesProp;
THnSparseF* fSparseJetVerticesProp;
THnSparseF* fSparsebTaggedJets;


Int_t rebin=2;
void efficiencySupraya(Float_t mass=0.,Int_t partonmethod=2, Float_t pt=10. ){

  //partonmethod
      // 0=weights
      // 1=parton in jet cone
      // 2=meson in jet cone

  TString outdir="plots_test";
  //TFile* f=new TFile("AnalysisResultsOldSet.root","r");
  //TFile* f=new TFile("AnalysisResultsSara.root","r");
  TFile* f=new TFile("AnalysisResultsHFCJ.root","r");

  TList *dhf = (TList*)f->Get("HFJets_containers");

  // Get the 4 containers
  //fSparseRecoJets=(THnSparseF*)dir->Get("fSparseRecoJets");
  AliHFJetsContainerVertex *hfcontJets = (AliHFJetsContainerVertex*)dhf->FindObject("kJets");
  //fSparseVerticesProp=(THnSparseF*)dir->Get("fSparseVerticesProp");
  AliHFJetsContainerVertex *hfcontQaVtx = (AliHFJetsContainerVertex*)dhf->FindObject("kQaVtx");
  //fSparseJetVerticesProp=(THnSparseF*)dir->Get("fSparseJetVerticesProp");
  AliHFJetsContainerVertex *hfcontJetsVtx = (AliHFJetsContainerVertex*)dhf->FindObject("kJetsVtx");
  //fSparsebTaggedJets=(THnSparseF*)dir->Get("fSparsebTaggedJets");
  AliHFJetsContainerVertex *hfcontBJets = (AliHFJetsContainerVertex*)dhf->FindObject("kBJets");

  // STEP
  AliHFJetsContainer::CFSteps step = AliHFJetsContainer::kCFStepRecoB;

  //gStyle->SetOptStat(0);
 
  // Restrict parton axis corresponding to chosen method
  TString axisname("");
  if(partonmethod==0)axisname=Form("partDP");
  if(partonmethod==1)axisname=Form("partBP");
  if(partonmethod==2)axisname=Form("partBH");

  // B
  hfcontJets->SetAxisRangeStep(axisname.Data(), 3.5, 4.5,step);
  TH1D* hallb=(TH1D*)hfcontJets->Project1D(step, "kCFJetPt");
  // C
  hfcontJets->SetAxisRangeStep(axisname.Data(), 2.5, 3.5,step);
  TH1D* hallc=(TH1D*)hfcontJets->Project1D(step, "kCFJetPt");
  // gluons
  hfcontJets->SetAxisRangeStep(axisname.Data(), 0.5, 1.5,step);
  TH1D* hallg=(TH1D*)hfcontJets->Project1D(step, "kCFJetPt");
  //hallg->DrawCopy();
  // light quarks
  TH1D *halllq;
  if(partonmethod!=2){
     hfcontJets->SetAxisRangeStep(axisname.Data(), 1.5, 2.5,step);
     halllq=(TH1D*)hfcontJets->Project1D(step, "kCFJetPt");
     }
  else{
     hfcontJets->SetAxisRangeStep(axisname.Data(), -0.5, 0.5,step);
     halllq=(TH1D*)hfcontJets->Project1D(step, "kCFJetPt");
    }
  //halllq->DrawCopy();

  // EFFICIENCY FOR B 
  
  Float_t yrange=0.00001;
  if(mass<1)yrange=0.05;

  // Set mass1 range
  hfcontJetsVtx->SetAxisRangeStep("mass1", mass, 10.,step);
  // Set Lxy1 range
  hfcontJetsVtx->SetAxisRangeStep("Lxy1", 0., 1.,step, kTRUE); // include overflow
  // Set parton range
  hfcontJetsVtx->SetAxisRangeStep(axisname.Data(), 3.5, 4.5,step); 
  // Project on jet pT
  TH1D *h0=(TH1D*)hfcontJetsVtx->Project1D(step, "kCFJetPt");
  //TH1D *h0=(TH1D*)hfcontJetsVtx->Project1D(step, "Lxy1");
  h0->Divide(hallb);
  h0->DrawCopy();
  //TH1D* h1=(TH1D*)makeProjection("h1",fSparseJetVerticesProp,axisparton,5,3,0.,1,2,0);
  //TH1D* h2=(TH1D*)makeProjection("h2",fSparseJetVerticesProp,axisparton,5,3,0.02,1,2,0);
  //TH1D* h3=(TH1D*)makeProjection("h3",fSparseJetVerticesProp,axisparton,5,3,0.04,13,2,0);
  //TH1D* h4=(TH1D*)makeProjection("h4",fSparseJetVerticesProp,axisparton,5,3,0.06,6,2,0);
  //TH1D* h5=(TH1D*)makeProjection("h5",fSparseJetVerticesProp,axisparton,5,3,0.08,1,2,0);

  //TH1D* h1a=new TH1D(*h1);
  //h1a->Divide(hallb);
  //TH1D* h2a=new TH1D(*h2);
  //h2a->Divide(hallb); 
  //TH1D* h3a=new TH1D(*h3);
  //h3a->Divide(hallb);
  //TH1D* h4a=new TH1D(*h4);
  //h4a->Divide(hallb); 
  //TH1D* h5a=new TH1D(*h5);
  //h5a->Divide(hallb);

  // ------------- c --------------

  //TH1D* h6=(TH1D*)makeProjection("h6",fSparseJetVerticesProp,axisparton,4,3,0.,1,2,0);
  //TH1D* h7=(TH1D*)makeProjection("h7",fSparseJetVerticesProp,axisparton,4,3,0.02,13,2,0);
  //TH1D* h8=(TH1D*)makeProjection("h8",fSparseJetVerticesProp,axisparton,4,3,0.04,6,2,0);
  //TH1D* h9=(TH1D*)makeProjection("h9",fSparseJetVerticesProp,axisparton,4,3,0.06,13,2,0);
  //TH1D* h10=(TH1D*)makeProjection("h10",fSparseJetVerticesProp,axisparton,4,3,0.08,6,2,0);
  //TCanvas* cc2=new TCanvas();
  //h6->Draw();

  //TH1D* h6a=new TH1D(*h6);
  //h6a->Divide(hallc);
  //TH1D* h7a=new TH1D(*h7);
  //h7a->Divide(hallc); 
  //TH1D* h8a=new TH1D(*h8);
  //h8a->Divide(hallc);
  //TH1D* h9a=new TH1D(*h9);
  //h9a->Divide(hallc); 
  //TH1D* h10a=new TH1D(*h10);
  //h10a->Divide(hallc);

  // ------------- g --------------

  //TH1D* h11=(TH1D*)makeProjection("h11",fSparseJetVerticesProp,axisparton,2,3,0.,1,2,0);
  //TH1D* h12=(TH1D*)makeProjection("h12",fSparseJetVerticesProp,axisparton,2,3,0.02,1,2,0);
  //TH1D* h13=(TH1D*)makeProjection("h13",fSparseJetVerticesProp,axisparton,2,3,0.04,13,2,0);
  //TH1D* h14=(TH1D*)makeProjection("h14",fSparseJetVerticesProp,axisparton,2,3,0.06,6,2,0);
  //TH1D* h15=(TH1D*)makeProjection("h15",fSparseJetVerticesProp,axisparton,2,3,0.08,1,2,0);

  //TH1D* h11a=new TH1D(*h11);
  //h11a->Divide(hallg);
  //TH1D* h12a=new TH1D(*h12);
  //h12a->Divide(hallg); 
  //TH1D* h13a=new TH1D(*h13);
  //h13a->Divide(hallg);
  //TH1D* h14a=new TH1D(*h14);
  //h14a->Divide(hallg); 
  //TH1D* h15a=new TH1D(*h15);
  //h15a->Divide(hallg);

  // ------------- lq --------------
  //if(partonmethod!=2){
  //  TH1D* h16=(TH1D*)makeProjection("h16",fSparseJetVerticesProp,axisparton,3,3,0.,1,2,0);
  //  TH1D* h17=(TH1D*)makeProjection("h17",fSparseJetVerticesProp,axisparton,3,3,0.02,13,2,0);
  //  TH1D* h18=(TH1D*)makeProjection("h18",fSparseJetVerticesProp,axisparton,3,3,0.04,6,2,0);
  //  TH1D* h19=(TH1D*)makeProjection("h19",fSparseJetVerticesProp,axisparton,3,3,0.06,13,2,0);
  //  TH1D* h20=(TH1D*)makeProjection("h20",fSparseJetVerticesProp,axisparton,3,3,0.08,6,2,0);
 // }
  //else
  //  {
  //    TH1D* h16=(TH1D*)makeProjection("h16",fSparseJetVerticesProp,axisparton,1,3,0.,1,2,0);
  //    TH1D* h17=(TH1D*)makeProjection("h17",fSparseJetVerticesProp,axisparton,1,3,0.02,13,2,0);
  //    TH1D* h18=(TH1D*)makeProjection("h18",fSparseJetVerticesProp,axisparton,1,3,0.04,6,2,0);
  //    TH1D* h19=(TH1D*)makeProjection("h19",fSparseJetVerticesProp,axisparton,1,3,0.06,13,2,0);
  //    TH1D* h20=(TH1D*)makeProjection("h20",fSparseJetVerticesProp,axisparton,1,3,0.08,6,2,0);
  //  }
  //TH1D* h16a=new TH1D(*h16);
  //h16a->Divide(halllq);
  //TH1D* h17a=new TH1D(*h17);
  //h17a->Divide(halllq); 
  //TH1D* h18a=new TH1D(*h18);
  //h18a->Divide(halllq);
  //TH1D* h19a=new TH1D(*h19);
  //h19a->Divide(halllq); 
  //TH1D* h20a=new TH1D(*h20);
  //h20a->Divide(halllq);

  // -------------- Canvases ---------------
  ////

  

  //Float_t lxy=0;
  //TCanvas* ca=new TCanvas("ca",Form("Efficiencies, Mass > %1.1f, Lxy > %1.1f",mass,lxy),800,800);
  //ca->cd()->SetLogy();

  //h1a->SetLineColor(2);
  //h6a->SetLineColor(1);
  //h11a->SetLineColor(8);
  //h16a->SetLineColor(9);
   
  //h1a->Draw();
  //h6a->Draw("sames");
  //if(partonmethod!=2) h11a->Draw("sames");
  //h16a->Draw("sames");
 
  //h1a->GetYaxis()->SetRangeUser(yrange,1.2);
  //h1a->GetXaxis()->SetRangeUser(10,50);

  //TLegend* leg=new TLegend(0.65,0.25,0.8,0.4);
  //leg->SetFillColor(0);
  //leg->SetBorderSize(0);
  //leg->AddEntry(h1a," b jets", "l");
  //leg->AddEntry(h6a," c jets", "l");
  //if(partonmethod!=2){
  //  leg->AddEntry(h11a," g jets", "l");
  //  leg->AddEntry(h16a," lq jets", "l");
 // }
  //else 
  //  leg->AddEntry(h16a," lq/g jets", "l");
//
//   TLegend* leg1=new TLegend(0.65,0.25,0.8,0.4);
//   leg1->AddEntry(h1a," b jets", "l");
//   leg1->AddEntry(h6a," c jets", "l");
//   leg1->AddEntry(h11a," g jets", "l");
//   leg1->AddEntry(h16a," lq jets", "l");
//   leg1->SetFillColor(0);
//   leg1->SetBorderSize(0);
 // leg->Draw();
 // h1a->SetTitle("");
//
 // ca->SaveAs(Form("%s/plot_BTagEff_Mass%1.1f_Lxy%1.1f_IdMethod%d.eps",outdir.Data(),mass,lxy,partonmethod));
 // ca->SaveAs(Form("%s/plot_BTagEff_Mass%1.1f_Lxy%1.1f_IdMethod%d.root",outdir.Data(),mass,lxy,partonmethod));

  ////

  //lxy=200;
  //TCanvas* cb=new TCanvas("cb",Form("Efficiencies, Mass > %1.1f, Lxy > %1.1f",mass,lxy),800,800);
  //cb->cd()->SetLogy();

  //h2a->SetLineColor(2);
  //h7a->SetLineColor(1);
  //h12a->SetLineColor(8);
  //h17a->SetLineColor(9);
   
  //h2a->Draw();
  //h7a->Draw("sames");
  //if(partonmethod!=2) h12a->Draw("sames");
  //h17a->Draw("sames");
  //h2a->SetTitle("");
  //h2a->GetYaxis()->SetRangeUser(yrange,1.2);
  //h2a->GetXaxis()->SetRangeUser(10,50);

  //leg->Draw();
  //cb->SaveAs(Form("%s/plot_BTagEff_Mass%1.1f_Lxy%1.1f_IdMethod%d.eps",outdir.Data(),mass,lxy,partonmethod));
  //cb->SaveAs(Form("%s/plot_BTagEff_Mass%1.1f_Lxy%1.1f_IdMethod%d.root",outdir.Data(),mass,lxy,partonmethod));

  ////
  //lxy=400;
  //TCanvas* cc=new TCanvas("cc",Form("Efficiencies, Mass > %1.1f, Lxy > %1.1f",mass,lxy),800,800);
  //cc->cd()->SetLogy();

  //h3a->SetLineColor(2);
  //h8a->SetLineColor(1);
  //h13a->SetLineColor(8);
  //h18a->SetLineColor(9);
 //  
  //h3a->Draw();
  //h8a->Draw("sames");
  //if(partonmethod!=2)  h13a->Draw("sames");
  //h18a->Draw("sames");
  //h3a->SetTitle("");
  //h3a->GetYaxis()->SetRangeUser(yrange,1.2);
  //h3a->GetXaxis()->SetRangeUser(10,50);

//  // TLegend* leg3=new TLegend(0.65,0.25,0.8,0.4);
////   leg3->AddEntry(h3a," b jets", "l");
////   leg3->AddEntry(h8a," c jets", "l");
////   leg3->AddEntry(h13a," g jets", "l");
////   leg3->AddEntry(h18a," lq jets", "l");
////   leg3->SetFillColor(0);
////   leg3->SetBorderSize(0);
//  leg->Draw();
//  cc->SaveAs(Form("%s/plot_BTagEff_Mass%1.1f_Lxy%1.1f_IdMethod%d.eps",outdir.Data(),mass,lxy,partonmethod));
//  cc->SaveAs(Form("%s/plot_BTagEff_Mass%1.1f_Lxy%1.1f_IdMethod%d.root",outdir.Data(),mass,lxy,partonmethod));

  ////
//  lxy=600;
//  TCanvas* cd=new TCanvas("cd",Form("Efficiencies, Mass > %1.1f, Lxy > %1.1f",mass,lxy),800,800);
//  cd->cd()->SetLogy();

//  h4a->SetLineColor(2);
//  h9a->SetLineColor(1);
//  h14a->SetLineColor(8);
//  h19a->SetLineColor(9);
   
//  h4a->Draw();
//  h9a->Draw("sames");
//  if(partonmethod!=2)h14a->Draw("sames");
//  h19a->Draw("sames");
 
//  h4a->GetYaxis()->SetRangeUser(yrange,1.2);
//  h4a->GetXaxis()->SetRangeUser(10,50);

//  h4a->SetTitle("");
//  // TLegend* leg4=new TLegend(0.65,0.25,0.8,0.4);
////   leg4->AddEntry(h4a," b jets", "l");
////   leg4->AddEntry(h9a," c jets", "l");
////   leg4->AddEntry(h14a," g jets", "l");
////   leg4->AddEntry(h19a," lq jets", "l");
////   leg4->SetFillColor(0);
////   leg4->SetBorderSize(0);
//  leg->Draw();
//  cd->SaveAs(Form("%s/plot_BTagEff_Mass%1.1f_Lxy%1.1f_IdMethod%d.eps",outdir.Data(),mass,lxy,partonmethod));
//  cd->SaveAs(Form("%s/plot_BTagEff_Mass%1.1f_Lxy%1.1f_IdMethod%d.root",outdir.Data(),mass,lxy,partonmethod));
//
  ////
//  lxy=800;
//  TCanvas* ce=new TCanvas("ce",Form("Efficiencies, Mass > %1.1f, Lxy > %1.1f",mass,lxy),800,800);
//  ce->cd()->SetLogy();
//
//  h5a->SetLineColor(2);
//  h10a->SetLineColor(1);
//  h15a->SetLineColor(8);
//  h20a->SetLineColor(9);
//   
//  h5a->Draw();
//  h10a->Draw("sames");
//  if(partonmethod!=2)h15a->Draw("sames");
//  h20a->Draw("sames");
// 
//  h5a->GetYaxis()->SetRangeUser(yrange,1.2);
//  h5a->GetXaxis()->SetRangeUser(10,50);
//  h5a->SetTitle("");
//  // TLegend* leg5=new TLegend(0.65,0.25,0.8,0.4);
////   leg5->AddEntry(h5a," b jets", "l");
////   leg5->AddEntry(h10a," c jets", "l");
////   leg5->AddEntry(h15a," g jets", "l");
////   leg5->AddEntry(h20a," lq jets", "l");
////   leg5->SetFillColor(0);
////   leg5->SetBorderSize(0);
//  leg->Draw();
//  ce->SaveAs(Form("%s/plot_BTagEff_Mass%1.1f_Lxy%1.1f_IdMethod%d.eps",outdir.Data(),mass,lxy,partonmethod));
//  ce->SaveAs(Form("%s/plot_BTagEff_Mass%1.1f_Lxy%1.1f_IdMethod%d.root",outdir.Data(),mass,lxy,partonmethod));
// 

//  loopForEff(2,20,22);
//
}

TH1D* makeProjection(TString name,THnSparse* hsparse,Int_t axparton,Int_t parton,Int_t axdecay,Double_t startdecay,Int_t col,Int_t wid,Int_t axvariable){

  TAxis* ax34=(TAxis*)hsparse->GetAxis(axparton);
  Int_t first=ax34->GetFirst(); 
  Int_t last=ax34->GetLast();
  Int_t nbins=ax34->GetNbins();
  ax34->SetRange(parton,parton);

  TAxis* ax=(TAxis*)hsparse->GetAxis(axdecay);
  Int_t firstd=ax->GetFirst();
  Int_t lastd=ax->GetLast();
  Int_t nbinsd=ax->GetNbins();
  Int_t findbin=ax->FindBin(startdecay);
  ax->SetRange(findbin,lastd+1);//include overflow in the decay length distribution which is filled only up to 1.

  TH1D* h34=(TH1D*)hsparse->Projection(axvariable);h34->SetName(name);
  h34->Sumw2();
  h34->Rebin(rebin);
  h34->SetLineColor(col);
  h34->SetLineWidth(wid);
  ax34->SetRange(first,last);
  ax->SetRange(firstd,lastd);
  
  return h34;


}

TH1D* makeProjectionpt(TString name,THnSparse* hsparse,Int_t axparton,Int_t parton,Int_t axvariable){
  
  // "axparton" corresponds to the methods: DP,BP or BH
  // restrict parton to type "parton"
  TAxis* ax34=(TAxis*)hsparse->GetAxis(axparton);
  Int_t first=ax34->GetFirst();
  Int_t last=ax34->GetLast();
  Int_t nbins=ax34->GetNbins();
  ax34->SetRange(parton,parton);
  // project on axvariable axis
  TH1D* h34=(TH1D*)hsparse->Projection(axvariable);h34->SetName(name);
  h34->Sumw2();
  h34->Rebin(rebin);
  ax34->SetRange(first,last);
 
  return h34;


}
TH1D* makeProjectionMulti(TString name,THnSparse* hsparse,Int_t axparton,Int_t startparton,Int_t endparton,Int_t axdecay,Double_t startdecay,Int_t axmass,Double_t startmass,Int_t col,Int_t wid,Int_t axvariable){

  TAxis* ax34=(TAxis*)hsparse->GetAxis(axparton);
  ax34->SetRange(startparton,endparton);
  TAxis* ax=(TAxis*)hsparse->GetAxis(axdecay);
  ax->SetRangeUser(startdecay,10.);
  TAxis* axa=(TAxis*)hsparse->GetAxis(axmass);
  axa->SetRangeUser(startmass,10.);
  TH1D* h34=(TH1D*)hsparse->Projection(axvariable);h34->SetName(name);
  h34->SetLineColor(col);
  h34->SetLineWidth(wid);

  ax->SetRange(0,ax->GetNbins()+1);
  axa->SetRange(0,axa->GetNbins()+1);
  ax34->SetRange(0,ax34->GetNbins()+1);
  return h34;


}


TH1D* makeProjectionptall(TString name,THnSparse* hsparse,Int_t axparton,Int_t startparton,Int_t endparton,Int_t axdecay,Double_t startdecay,Double_t enddecay,Int_t col,Int_t wid,Int_t axvariable){

  // parton id of type: DP, BP or BH
  TAxis* ax34=(TAxis*)hsparse->GetAxis(axparton);
  // select parton type (i.e. b=5)
  ax34->SetRange(startparton,endparton);
  // select some other variable (i.e. jet-pT=0) 
  TAxis* axb=(TAxis*)hsparse->GetAxis(axdecay);
  // restrict its axis 
  axb->SetRangeUser(startdecay,enddecay);
  // project on "axvariable"
  TH1D* h34=(TH1D*)hsparse->Projection(axvariable);h34->SetName(name);
  h34->SetLineColor(col);
  h34->SetLineWidth(wid);
  
  // reset container
  axb->SetRange(0,axb->GetNbins()+1);
  ax34->SetRange(0,ax34->GetNbins()+1);
  return h34;

}
void loopForEff(Int_t partonmethod, Double_t minpt,Double_t maxpt){
  cout<<"starting Loop on M and Lxy for 2D eff plot -- id jet method --> "<<partonmethod<<endl;
  TAxis* ax=(TAxis*)fSparseJetVerticesProp->GetAxis(0);
  ax->SetRangeUser(minpt,maxpt);
  TAxis* ax2=(TAxis*)fSparseRecoJets->GetAxis(0);
  ax2->SetRangeUser(minpt,maxpt);

  //  cout<<"axis min="<<ax1->GetXmin()<<"  axis max="<<ax1->GetXmax()<<endl;
  TAxis* ax2000=(TAxis*)fSparseRecoJets->GetAxis(1);
  ax2000->SetRangeUser(-0.5,0.5);
  TAxis* ax1011=(TAxis*)fSparseJetVerticesProp->GetAxis(6);
  ax1011->SetRangeUser(0,10);

  Int_t axispart=-1;
  if(partonmethod==0)axispart=5;
  if(partonmethod==1)axispart=6;
  if(partonmethod==2)axispart=7;

  TH1D* hball=(TH1D*)makeProjectionptall("hball",fSparseRecoJets,axispart,5,5,0,minpt,maxpt,1,2,0);
  TH1D* hcall=(TH1D*)makeProjectionptall("hcall",fSparseRecoJets,axispart,4,4,0,minpt,maxpt,1,2,0);
  if(partonmethod!=2) TH1D* hlqgall=(TH1D*)makeProjectionptall("hlqgall",fSparseRecoJets,axispart,2,3,0,minpt,maxpt,1,2,0);
  else TH1D* hlqgall=(TH1D*)makeProjectionptall("hlqgall",fSparseRecoJets,axispart,1,1,0,minpt,maxpt,1,2,0);

  // TH1D* h3all=(TH1D*)makeProjectionptall("h3all",fSparseRecoJets,6,2,3,0,minpt,maxpt,1,2,0);
  
  
  // Double_t sum=0;
  // for(Int_t ib=1;ib<h1all->GetNbinsX()+1;ib++){
  //   cout<<ib<<"  "<<h1all->GetBinContent(ib)<<endl;
  //   sum+=h1all->GetBinContent(ib);
  // }
  cout<<"sum of histogram contents = "<<hball->Integral(hball->FindBin(minpt),hball->FindBin(maxpt))<<endl;
  Double_t sumdenom=hball->Integral(hball->FindBin(minpt),hball->FindBin(maxpt));
  Double_t sumdenomC=hcall->Integral(hcall->FindBin(minpt),hcall->FindBin(maxpt));//charm
  Double_t sumdenomGL=hlqgall->Integral(hlqgall->FindBin(minpt),hlqgall->FindBin(maxpt));//gluons, light quarks

  //TH1D* makeProjectionpt(TString name,THnSparse* hsparse,Int_t axparton,Int_t parton,Int_t axdecay,Double_t startdecay,Double_t enddecay,Int_t axmass,Double_t startmass,Double_t endmass,Int_t col,Int_t wid,Int_t axvariable)


  const  Int_t nBinsMass=6;
  const  Int_t nBinsLxy=5;
  Double_t deltaM=0.5;
  Double_t deltaLxy=0.02;
  Double_t Masscut=0.5;
  Double_t Lxycut=0;

  TGraph** h2D=new TGraph*[nBinsMass];
  TGraph** h2Da=new TGraph*[nBinsMass];
  Int_t axisparton=-1;
  if(partonmethod==0)axisparton=18;
  if(partonmethod==1)axisparton=19;
  if(partonmethod==2)axisparton=20;

  for(Int_t ij=0;ij<nBinsMass;ij++){
    TString hName="hM_bc";
    TString hName2="hM_blqg";

    hName.Append(Form("_%0.1f",Masscut));
    h2D[ij]=new TGraph(0);
    h2D[ij]->SetName(hName.Data());
    h2D[ij]->SetTitle(hName.Data());
    hName2.Append(Form("_%0.1f",Masscut));
    h2Da[ij]=new TGraph(0);
    h2Da[ij]->SetName(hName2.Data());
    h2Da[ij]->SetTitle(hName2.Data());

    Lxycut=0;
    for(Int_t ii=0;ii<nBinsLxy;ii++){
      
      // for(Int_t ij=0;ij<15;ij++){
      //c, b, ud, g efficiecies
      TH1D* hb=(TH1D*)makeProjectionMulti("hb",fSparseJetVerticesProp,axisparton,5,5,3,Lxycut,6,Masscut,1,2,0);
      Double_t sumnumer=hb->Integral(hb->FindBin(minpt),hb->FindBin(maxpt));
      Double_t effB=sumnumer/sumdenom;
  
      TH1D* hc=(TH1D*)makeProjectionMulti("hc",fSparseJetVerticesProp,axisparton,4,4,3,Lxycut,6,Masscut,1,2,0);
      Double_t sumnumerC=hc->Integral(hc->FindBin(minpt),hc->FindBin(maxpt));
      Double_t effC=sumnumerC/sumdenomC;
    
      if(partonmethod!=2) TH1D* hlqg=(TH1D*)makeProjectionMulti("hlqg",fSparseJetVerticesProp,axisparton,2,3,3,Lxycut,6,Masscut,1,2,0);
      else  TH1D* hlqg=(TH1D*)makeProjectionMulti("hlqg",fSparseJetVerticesProp,axisparton,1,1,3,Lxycut,6,Masscut,1,2,0);

      Double_t sumnumerGL=hlqg->Integral(hlqg->FindBin(minpt),hlqg->FindBin(maxpt));
      Double_t effGL=sumnumerGL/sumdenomGL;
  
      h2D[ij]->SetPoint(ii,effB,effC);
      h2Da[ij]->SetPoint(ii,effB,effGL);

      cout<<"Lxy cut = "<<Lxycut<<" Mass cut = "<<Masscut<<"   effB="<<effB<<"  effC="<<effC<<"  effGL="<<effGL<<endl;
      Lxycut+=deltaLxy;
   
    }
  
    Masscut+=deltaM;
  }

  h2D[0]->SetLineColor(kRed-6);
  h2D[1]->SetLineColor(kRed-5);
  h2D[2]->SetLineColor(kRed-1);
  h2D[3]->SetLineColor(kMagenta-2);
  h2D[4]->SetLineColor(kRed+2);
  h2D[5]->SetLineColor(kRed);
  h2D[0]->SetLineWidth(2);
  h2D[1]->SetLineWidth(2);
  h2D[2]->SetLineWidth(2);
  h2D[3]->SetLineWidth(2);
  h2D[4]->SetLineWidth(2);
  h2D[5]->SetLineWidth(2);
 
  h2Da[0]->SetLineColor(kBlue);
  h2Da[1]->SetLineColor(kBlue-4);
  h2Da[2]->SetLineColor(kBlue-7);
  h2Da[3]->SetLineColor(kBlue-2);
  h2Da[4]->SetLineColor(kBlue+1);
  h2Da[5]->SetLineColor(kBlue+4);
  h2Da[0]->SetLineWidth(2);
  h2Da[1]->SetLineWidth(2);
  h2Da[2]->SetLineWidth(2);
  h2Da[3]->SetLineWidth(2);
  h2Da[4]->SetLineWidth(2);
  h2Da[5]->SetLineWidth(2);

  h2D[0]->GetXaxis()->SetTitle("Eff b");
  h2D[0]->GetYaxis()->SetTitle("Eff c");
  h2D[2]->GetXaxis()->SetTitle("Eff b");
  h2D[2]->GetYaxis()->SetTitle("Eff c");
  h2Da[0]->GetXaxis()->SetTitle("Eff b");
  h2Da[0]->GetYaxis()->SetTitle("Eff lq/g");

  h2D[2]->GetXaxis()->SetLimits(0,1);
  h2D[2]->GetYaxis()->SetRangeUser(0,1);
  h2D[0]->GetXaxis()->SetLimits(0,1);
  h2D[0]->GetYaxis()->SetRangeUser(0,1);
  h2Da[0]->GetXaxis()->SetLimits(0,1);
  h2Da[0]->GetYaxis()->SetRangeUser(0,1);
  h2D[0]->SetTitle("");
  h2D[2]->SetTitle("");
  h2Da[0]->SetTitle("");

  TCanvas*c1=new TCanvas();
  // c1->SetLogy();
  h2D[0]->Draw("al");
  h2D[1]->Draw("lsame");
  h2D[2]->Draw("lsame");
  h2D[3]->Draw("lsame");
  h2D[4]->Draw("lsame");
  h2D[5]->Draw("lsame");
  h2D[0]->GetXaxis()->SetLimits(0,1);
  h2D[0]->GetYaxis()->SetRangeUser(0,1);
  h2D[2]->GetXaxis()->SetLimits(0,1);
  h2D[2]->GetYaxis()->SetRangeUser(0,1);
 
  h2D[0]->Draw("al");
  h2D[1]->Draw("lsame");
  h2D[2]->Draw("lsame");
  h2D[3]->Draw("lsame");
  h2D[4]->Draw("lsame");
  h2D[5]->Draw("lsame");

 TCanvas*c2=new TCanvas();
  h2Da[0]->Draw("al");
  h2Da[1]->Draw("lsame");
  h2Da[2]->Draw("lsame");
  h2Da[3]->Draw("lsame");
  h2Da[4]->Draw("lsame");
  h2Da[5]->Draw("lsame");
  h2Da[0]->GetXaxis()->SetLimits(0,1);
  h2Da[0]->GetYaxis()->SetRangeUser(0,1);
  h2Da[0]->Draw("al");
  h2Da[1]->Draw("lsame");
  h2Da[2]->Draw("lsame");
  h2Da[3]->Draw("lsame");
  h2Da[4]->Draw("lsame");
  h2Da[5]->Draw("lsame");
 //  TCanvas*c=new TCanvas();
//   c->Divide(3,2);
//   c->cd(1);  h2D[0]->Draw("al");
//   c->cd(2);  h2D[1]->Draw("al");
//   c->cd(3);  h2D[2]->Draw("al");
//   c->cd(4);  h2D[3]->Draw("al");
//   c->cd(5);  h2D[4]->Draw("al");
//   c->cd(6);  h2D[5]->Draw("al");


}


void comparePtJetQuark(){
  //Float_t mass=0.;
  Int_t partonmethod=1;  //--> 0=weights, 1=parton in jet cone, 2=meson in jet cone.

  //TString outdir="plot_LHC11a1bcde_norebin";
  // TFile* f1=new TFile("/Users/elenabruna/Documents/ALICE/Beauty/hfcj/hfcj_July30/upgrade_LHC11a1_b_Oct15/AnalysisResults_Merged_LHC11a1bcde.root");
    
  // TString outdir="plot_LHC10f7a_npr3_ptcu1_272";
  //TFile* f1=new TFile("/Users/elenabruna/Documents/ALICE/Beauty/hfcj/hfcj_March22/July12_2/AnalysisResults_ptcut1_npr3_July12.root");
    
    //TString outdir="plot_LHC13d18bcde_ITSup_768";
  //TFile* f1=new TFile("/Users/elenabruna/Documents/ALICE/Beauty/Upgrade/AnalysisResults-LHC13d18bcde-ITSup-768.root");

  //   TString outdir="plot_LHC13d18bcde_768";
   TFile* f1=new TFile("/Users/elenabruna/Documents/ALICE/Beauty/AR-LHC13d1bcde-768-merge.root");

  //TString outdir="plot_LHC10f6a_272";
  //TFile* f1=new TFile("/Users/elenabruna/Documents/ALICE/Beauty/Upgrade/AnalysisResults-LHC10f6a-ITSup-272.root");//note: this file is actually w/o upgrade.

//   TString outdir="plot_LHC10f7a_upgrade_npr3_ptcu1";
//   TFile* f1=new TFile("/Users/elenabruna/Documents/ALICE/Beauty/hfcj/hfcj_July30/upgrade3/AnalysisResults_upgrade_npr3_ptcu1.root");

  TDirectory *dir = (TDirectory*)f1->Get("PWG3_HFCJ_HFjetVertex");

  fSparseRecoJets=(THnSparseF*)dir->Get("fSparseRecoJets");
  fSparseVerticesProp=(THnSparseF*)dir->Get("fSparseVerticesProp");
  fSparseJetVerticesProp=(THnSparseF*)dir->Get("fSparseJetVerticesProp");
  fSparsebTaggedJets=(THnSparseF*)dir->Get("fSparsebTaggedJets");

  //fSparseVerticesProp->GetAxis(0)->SetRangeUser(10,60); 
  //fSparseRecoJets->GetAxis(0)->SetRangeUser(10,60); 
  //---------------------------------------------------------------------//
  
  //---------------------------------------------------------------------//

  //jetpt;eta;phi;ntrks;nEle;parton;parton2;parton3;partContr;ptPart;ptPart2;ptPart3;AreaCh;
  //jetpt;ntrks;ptVtx;mass;Lxy;chi2/ndf;parton;nRealVtx;deltaX;deltaY;sigvert;LxyJet;LxyJetPerp;cosTheta;nfromB;nfromBD;parton2;parton3;
  //jetpt;nRecoVert;SumMass3MostDispl;Lxy1;Lxy2;Lxy3;mass1;mass2;mass3;nReal1;nReal2;nReal3;nFromB1;nFromB2;nFromB3;nFromPrD1;nFromPrD2;nFromPrD3;parton;parton2;parton3;
  //jetpt;eta;phi;ntrks;nRecoVtx;nEle;parton;partContr;ptPart;areaCh;parton2;parton3;
  
  gStyle->SetOptStat(0);
 
  // pT of all Reco Jets - b,c,g,lq
  TAxis* axpt=(TAxis*)fSparseRecoJets->GetAxis(0);
  //axpt->SetRangeUser(10,60);
  TAxis* axptpart=(TAxis*)fSparseRecoJets->GetAxis(9);
  //axptpart->SetRangeUser(10,60);
  // eta of all Reco Jets - b,c,g,lq
  TAxis* axeta=(TAxis*)fSparseRecoJets->GetAxis(1);
  axeta->SetRangeUser(-0.5,0.5);
 
  Int_t axispart=-1;
  if(partonmethod==0)axispart=5;
  if(partonmethod==1)axispart=6;
  if(partonmethod==2)axispart=7;
  TAxis* ax1=(TAxis*)fSparseRecoJets->GetAxis(axispart);
  Int_t first=ax1->GetFirst(); 
  Int_t last=ax1->GetLast();
  Int_t nbins=ax1->GetNbins();

  ax1->SetRange(5,5);
  TH2D* hb=(TH2D*)fSparseRecoJets->Projection(0,9);
  TH1D* hbpt=(TH1D*)fSparseRecoJets->Projection(9);
  hb->SetTitle("Beauty"); 

  ax1->SetRange(4,4);
  TH2D* hc=(TH2D*)fSparseRecoJets->Projection(0,9);
  TH1D* hcpt=(TH1D*)fSparseRecoJets->Projection(9);
  hc->SetTitle("Charm"); 

  ax1->SetRange(3,3);
  TH2D* hlqg=(TH2D*)fSparseRecoJets->Projection(0,9);
  TH1D* hlqgpt=(TH1D*)fSparseRecoJets->Projection(9);
  hlqg->SetTitle("Light quarks/Gluons"); 

  TCanvas* c=new TCanvas();
  c->Divide(3,1);
  c->cd(1)->SetLogz();
  hb->Draw("colz");
  c->cd(2)->SetLogz();
  hc->Draw("colz");
  c->cd(3)->SetLogz();
  hlqg->Draw("colz");

 TCanvas* c2=new TCanvas();
 hbpt->SetLineColor(2);
 hcpt->SetLineColor(1);
 hlqgpt->SetLineColor(4);

 hlqgpt->Draw();
 hcpt->Draw("same");
 hbpt->Draw("same");
 TFile* fout=new TFile("mapBJetvsBquark.root","recreate");
 fout->cd();
 hb->Write("b");
 hc->Write("c");
 hlqg->Write("lq");
 fout->Close();

}
