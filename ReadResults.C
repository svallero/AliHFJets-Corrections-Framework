class AliHFJetContainerVertex;

void ReadResults(){

  TFile *f = new TFile("AnalysisResults.root","r");

  // JETS
  if (0){
    TDirectoryFile *djets = (TDirectoryFile*)f->Get("PWGJE_cluster_AOD__ANTIKT04_B0_Filter00272_Cut00150_Skip00");
    TList *ljets = (TList*)djets->Get("cluster_AOD__ANTIKT04_B0_Filter00272_Cut00150_Skip00");
    TH1F *h1jets = (TH1F*)ljets->FindObject("fh1NJetsRec");
    h1jets->Draw();
  }

  // HF vertexes
  if (1){
    TDirectoryFile *dhf = (TDirectoryFile*)f->Get("PWGHF_HFCJ_HFJetsVertex");
    TList *l = (TList*)dhf->Get();
    //AliHFJetsContainerVertex *hfcont = (AliHFJetsContainerVertex*)dhf->Get("kJets");
    //TString vars("kCFJetPt;kCFJetEta;kCFJetPhi;nTrk;nEle;partDP;partBP;partBH;partContr;ptDP;ptBP;ptBH;areaCh");
    //AliHFJetsContainerVertex *hfcont = (AliHFJetsContainerVertex*)dhf->Get("kQaVtx");
    //TString vars("kCFJetPt;kCFJetEta;kCFJetPhi;nTrk;ptVtx;mass;Lxy;chi2/ndf;partDP;nRealVtx;deltaX;deltaY;sigVtx;LxyJet;LxyJetPerp;cosTheta;nFromB;nFromBD;partBP;partBH");
    AliHFJetsContainerVertex *hfcont = (AliHFJetsContainerVertex*)dhf->Get("kBJets");
    TString vars("kCFMult;kCFJetPt;kCFJetEta;kCFJetPhi;nTrk;nRecoVtx;nEle;partDP;partContr;ptDP;areaCh;partBP;partBH");
    //AliHFJetsContainerVertex *hfcont = (AliHFJetsContainerVertex*)dhf->Get("kJetsVtx");
    //TString vars("kCFJetPt;kCFJetEta;kCFJetPhi;nRecoVtx;SumMass3MostDispl;Lxy1;Lxy2;Lxy3;mass1;mass2;mass3;nReal1;nReal2;nReal3;nFromB1;nFromB2;nFromB3;nFromPrD1;nFromPrD2;nFromPrD3;partDP;partBP;partBH");
    //TString vars("nRecoVtx");
  if (1){
  TObjArray *arr;
  TObjString *objstr;
  arr = vars.Tokenize(";");
  TIter next(arr);
  Int_t i = 0;
  Int_t step = 0;
  while ((objstr=(TObjString*)next())){
    TString str(objstr->GetString());
    TH1 *h = hfcont->Project1D(0, str.Data());
    new TCanvas(str.Data(),str.Data());
    h->DrawCopy();
  }
  }
  }
  f->Close();
  delete hfcont;
}