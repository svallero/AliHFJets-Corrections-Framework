//-*- Mode: C++ -*-
#ifndef STYLESARA_H
#define STYLESARA_H


////////////////////////////////////////////////
//--------------------------------------------- 
// Functions to ease plotting
//---------------------------------------------
////////////////////////////////////////////////

// --- ROOT system ---
#include <TObject.h> 
#include <TCanvas.h> 
#include <TPad.h> 

class TObject;
class TCanvas;
class TROOT;
class TPad;

void PrepareRatioPlot(TH1 *dummy){
	dummy->GetXaxis()->SetLabelSize(0.1);
	dummy->GetYaxis()->SetLabelSize(0.08);
	dummy->GetXaxis()->SetTitleSize(0.1);
	dummy->GetXaxis()->SetTitleOffset(1.05);
	dummy->GetXaxis()->SetLabelOffset(0.02);
	dummy->GetYaxis()->SetTitleSize(0.1);
	dummy->GetYaxis()->SetTitleOffset(0.4);
}

void Color(TH1 *histo, Int_t color, Int_t marker = 20){
	histo->SetMarkerStyle(marker);
	histo->SetMarkerColor(color);
	histo->SetLineColor(color);
	histo->SetLineWidth(2);

	histo->GetXaxis()->SetTitleOffset(1.2);
	histo->GetYaxis()->SetTitleOffset(1.2);
}

void LineStyle(TH1 *histo, Int_t color){
	histo->SetLineWidth(3);
	//histo->SetMarkerStyle(0);
	histo->SetLineColor(color);
}

TCanvas *CreateCanvas(const char *title, Bool_t kPads = kTRUE){
	TCanvas *canvas = new TCanvas(title,title,0,22,919,691);
	canvas->Range(0, 0, 1, 1);
	//canvas->SetLogy();
	SetStyle();
	if(kPads) CreatePads();
	gPad->SetGrid();	
	return canvas;
}

TObjArray *CreatePads(){

	// TOP PAD
	TPad* pad1 = new TPad("pad1", "", 0, 0.3, 1.0, 1.0);
	pad1->SetTopMargin(0.02);
	pad1->SetRightMargin(0.02);
	pad1->SetLeftMargin(0.08);
	//gROOT->SetStyle("Plain");// Switches off the ROOT default style
	//gPad->UseCurrentStyle(); // this makes everything b/w, removing the default red border on images
	//gROOT->ForceStyle();     // forces the style chosen above to be used,
	//pad1->Draw();

	// BOTTOM PAD
	TPad* pad2 = new TPad("pad2", "", 0, 0.0, 1.0, 0.3);
	pad2->SetRightMargin(0.02);
	pad2->SetLeftMargin(0.08);
	pad2->SetBottomMargin(0.25);

	pad2->Draw();
	pad1->Draw();

	// no border between them
	pad1->SetBottomMargin(0);
	pad2->SetTopMargin(0);

	TObjArray *array = new TObjArray();
	array->AddLast(pad1);
	array->AddLast(pad2);

	return array;
}

TLegend *NewLegend(const char *header){
	//TLegend *legend = new TLegend(0.168306,0.04546434,0.4655738,0.3686705,NULL,"brNDC");
	TLegend *legend = new TLegend(0.4568306,0.5143288,0.8404372,0.8476621,NULL,"brNDC");
	legend->SetHeader(header);
	legend->SetFillColor(0);
	legend->SetLineColor(0);
	legend->SetTextFont(42);
	legend->SetTextSize(0.04);
	return legend;
}


void SetStyle(){
	Int_t font = 42;

	//No white margins in canvas
	//gPad->SetTopMargin(0.02);
	//gPad->SetRightMargin(0.02);
	//gPad->SetLeftMargin(0.15);
	//gPad->SetBottomMargin(0.1);
	// Set plotting style
	gPad->SetTickx();
	gPad->SetTicky();
	gPad->SetFrameFillColor(0);
	gPad->SetFillColor(0);
	gPad->SetBorderSize(2);
	//gPad->SetGridy();
	gStyle->SetOptTitle(0);
	gStyle->SetOptStat(111111);
	//gStyle->SetOptStat(0);
	gStyle->SetPalette(1);
	gStyle->SetTitleBorderSize(0);
	gStyle->SetTitleFillColor(0);
	gStyle->SetStatColor(0);
	gStyle->SetLabelFont(font,"xyz");
	//gStyle->SetTitleSize(0.06,"xy");
	//gStyle->SetTitleOffset(0.95,"y");
	//gStyle->SetTitleOffset(2.05,"x");
	gStyle->SetTitleFont(font,"xyz");
	gStyle->SetTextFont(font);

	// improved palette
	const Int_t NRGBs = 5;
	const Int_t NCont = 255;

	Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
	Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
	Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
	Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
	TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
	gStyle->SetNumberContours(NCont);
}

#endif
