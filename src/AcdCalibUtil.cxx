#define AcdCalibUtil_cxx
#include "AcdCalibUtil.h"

#include "TString.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TPad.h"
#include "TH1.h"
#include "TF1.h"
#include <cmath>

#include <cmath>

#include "./AcdHistCalibMap.h"
#include "./AcdGainFit.h"
#include "./AcdVetoFit.h"
#include "./AcdCnoFit.h"
#include "./AcdPadMap.h"

ClassImp(AcdCalibUtil) ;

AcdCalibUtil::AcdCalibUtil(){;}

AcdCalibUtil::~AcdCalibUtil(){;}

void AcdCalibUtil::saveCanvases(TList& cl, const char* filePrefix, const char* suffix) {
  TString pr(filePrefix);
  UInt_t n = cl.GetSize();
  for ( UInt_t i(0); i < n; i++ ) {
    TCanvas* canvas = (TCanvas*)(cl.At(i));
    TString name = pr + canvas->GetName();
    name += suffix;
    canvas->SaveAs(name);
  }
}

void AcdCalibUtil::drawVetoPlot(TVirtualPad& vp, TH1& hVeto, TH1& hAll, AcdVetoFitResult* res) {
  vp.cd();
  hAll.Draw();
  hVeto.SetLineColor(4);
  hVeto.Draw("same");

  if ( res != 0 ) {
    TLine* lmean = new TLine( res->veto(), 0.,  res->veto(), hAll.GetMaximum());
    lmean->SetLineColor(2);
    lmean->Draw();
  }   
}

void AcdCalibUtil::drawCnoPlot(TVirtualPad& vp, TH1& hCno, TH1& hAll, AcdCnoFitResult* res) {
  vp.cd();
  hAll.Draw();
  hCno.SetLineColor(4);
  hCno.Draw("same");

  if ( res != 0 ) {
    TLine* lmean = new TLine( res->cno(), 0.,  res->cno(), hAll.GetMaximum());
    lmean->SetLineColor(2);
    lmean->Draw();
  }   
}

void AcdCalibUtil::drawMipPlot(TVirtualPad& vp, TH1& hist, AcdGainFitResult* res, Bool_t onLog) {

  // set plot limits & such
  Double_t ymin = onLog ? 1. : 0.;
  Double_t expand = onLog ? 0.5 : 0.;
  Double_t xmin, xmax, width;

  // get the function used to fit
  TF1* fA = hist.GetFunction("pol7");
  if ( fA == 0 ) {
    fA = hist.GetFunction("pol5");
  }
  
  // extract the limits from fit function
  if ( fA != 0 ) {
    fA->GetRange(xmin,xmax);
    width = xmax - xmin;
    hist.SetAxisRange(xmin - expand* width, xmax + expand * width);
  }
  
  hist.SetMinimum(ymin);
  vp.cd();
  if ( onLog ) vp.SetLogy();
  
  // draw the histogram
  hist.Draw();   

  // add a line showing the fitted value for MIP peak, if extant
  if ( res != 0 ) {
    TLine* lA = new TLine( res->peak(), hist.GetMinimum(),  res->peak(), hist.GetMaximum());
    lA->SetLineColor(2);
    lA->Draw();
  } 
}

AcdPadMap* AcdCalibUtil::drawCnos(AcdHistCalibMap& hCno, AcdHistCalibMap& hRaw,
				  AcdCnoFitMap& cnos, const char* prefix) {

  AcdPadMap* padMap = new AcdPadMap(hCno.config(),prefix);
  TList& hList = (TList&)hCno.histograms();
  UInt_t n = hList.GetEntries();
  for ( UInt_t i(0); i < n; i++ ) {
    TObject* obj = hList.At(i);
    if ( obj == 0 ) continue;
    UInt_t id = obj->GetUniqueID();
    TVirtualPad* pad = padMap->getPad(id);
    if ( pad == 0 ) continue;
    TH1* hv = (TH1*)(obj);
    TH1* hr = hRaw.getHist(id);
    if ( hv == 0 || hr == 0 ) continue;
    AcdCnoFitResult* res = (AcdCnoFitResult*)(cnos.get(id));
    drawCnoPlot(*pad,*hv,*hr,res);
  }
  return padMap;
}

AcdPadMap* AcdCalibUtil::drawVetos(AcdHistCalibMap& hVeto, AcdHistCalibMap& hRaw,
				   AcdVetoFitMap& vetos, const char* prefix) {

  AcdPadMap* padMap = new AcdPadMap(hVeto.config(),prefix);
  TList& hList = (TList&)hVeto.histograms();
  UInt_t n = hList.GetEntries();
  for ( UInt_t i(0); i < n; i++ ) {
    TObject* obj = hList.At(i);
    if ( obj == 0 ) continue;
    UInt_t id = obj->GetUniqueID();
    TVirtualPad* pad = padMap->getPad(id);
    if ( pad == 0 ) continue;
    TH1* hv = (TH1*)(obj);
    TH1* hr = hRaw.getHist(id);
    if ( hv == 0 || hr == 0 ) continue;
    AcdVetoFitResult* res = (AcdVetoFitResult*)(vetos.get(id));
    drawVetoPlot(*pad,*hv,*hr,res);
  }
  return padMap;
}

AcdPadMap* AcdCalibUtil::drawMips(AcdHistCalibMap& h, AcdGainFitMap& gains, 
				  Bool_t onLog, const char* prefix) {
  AcdPadMap* padMap = new AcdPadMap(h.config(),prefix);
  TList& hList = (TList&)h.histograms();
  UInt_t n = hList.GetEntries();
  for ( UInt_t i(0); i < n; i++ ) {
    TObject* obj = hList.At(i);
    if ( obj == 0 ) continue;
    UInt_t id = obj->GetUniqueID();
    TVirtualPad* pad = padMap->getPad(id);
    if ( pad == 0 ) continue;
    TH1* hh = (TH1*)(obj);
    if ( hh == 0) continue;
    AcdGainFitResult* res = (AcdGainFitResult*)(gains.get(id));
    drawMipPlot(*pad,*hh,res,onLog);
  }
  return padMap;
}

AcdPadMap* AcdCalibUtil::drawStripCharts(AcdHistCalibMap& h, const char* prefix) {

  AcdPadMap* padMap = new AcdPadMap(h.config(),prefix);
  TList& hList = (TList&)h.histograms();
  UInt_t n = hList.GetEntries();
  for ( UInt_t i(0); i < n; i++ ) {
    TObject* obj = hList.At(i);
    if ( obj == 0 ) continue;
    UInt_t id = obj->GetUniqueID();
    TVirtualPad* pad = padMap->getPad(id);
    if ( pad == 0 ) continue;
    TH1* hh = (TH1*)(obj);
    if ( hh == 0 ) continue;
    hh->SetMinimum(-50.);
    hh->SetMaximum(50.);
    pad->cd();
    hh->Draw();
  }
  return padMap;
}

void AcdCalibUtil::chi2Dist(const TH1& input, TH1*& output, Int_t method, Float_t refVal, Float_t scale) {
  std::string theName(input.GetName());
  theName += "_chi2dist";
  Int_t nBin = input.GetNbinsX();
  if ( output == 0 ) {
    // make sure that we have somewhere to put the output
    output = new TH1F(theName.c_str(),theName.c_str(),100,-6.,6.);
  }
  Int_t i(0);
  Float_t ref = refVal;
  Float_t sum(0.);
  Bool_t calcMean(kFALSE);
  switch ( method ) {
  case PLAIN: break;
  case MEAN_ABSOLUTE: 
  case MEAN_RELATIVE: 
  case MEAN_SIGMA: 
    calcMean = kTRUE;
    break;
  }


  for ( i = 1; i <= nBin; i++ ) {
    Float_t val = input.GetBinContent(i);
    Float_t err = input.GetBinError(i);
    Float_t valDif = val - ref;
    switch ( method ) {
    case PLAIN:
    case MEAN_ABSOLUTE:
      break;
    case MEAN_RELATIVE:
      if ( ref > 1e-9 ) { valDif /= ref; }
      break;
    case MEAN_SIGMA:
      if ( err > 1e-9 ) { valDif /= err; }
      break;
    }
    valDif *= scale;
    output->Fill(valDif);
  }
}

Float_t AcdCalibUtil::efficDivide(TH1& out, const TH1& top, const TH1& bot, Bool_t inEffic, Float_t minBot) {

  Int_t nt = top.GetNbinsX();
  Int_t nb = bot.GetNbinsX();
  if ( nt != nb ) return -1.;

  Float_t iT(0.);
  Float_t iB(0.);

  for ( Int_t i(1); i <= nt; i++ ) {
    Float_t vT = top.GetBinContent(i);
    Float_t n = bot.GetBinContent(i);
    iT += vT;
    iB += n;
    if ( vT > n ) { 
      out.SetBinContent(i,0.0);
      out.SetBinError(i,0.0);
      continue;
    }
    if ( n < minBot )  {
      out.SetBinContent(i,0.0);
      out.SetBinError(i,0.0);
      continue;
    }
    // sneak in 1/10 an event to distinguish perfect channels from channels w/ no data.
    if ( vT < 0.1 ) {
      vT = 0.1;
    }
    Float_t p = vT / n;
    Float_t q = 1. - p;
    Float_t npq = n*p*q;
    Float_t err = TMath::Sqrt(npq) / n;
    out.SetBinContent(i, inEffic ? q : p);
    out.SetBinError(i,err);
  }

  out.SetMinimum(-0.05);
  out.SetMaximum(1.05);
  
  return fabs(iB) > 1e-9 ? iT / iB : 0.;
}


UShort_t AcdCalibUtil::gemId(UInt_t id) {
  UInt_t face = id / 100;
  UInt_t row = (id % 100 ) / 10;
  UInt_t col = (id % 10 );
  switch ( face ) {
  case 0:  return 64 + 5*row + col;  // top:      64 - 89
  case 1:  return 32 + 5*row + col;  // -x side:  32 - 47
  case 2:  return      5*row + col;  // -y side    0 - 15
  case 3:  return 48 + 5*row + col;  // +x side   48 - 63
  case 4:  return 16 + 5*row + col;  // +y side   16 - 31
  case 5:                                   // x ribbons 96 - 99
    return 96 + col;
  case 6:                                   // y ribbons 100- 103
    return 100 + col;
  }
  return 0xFFFF;
}


Float_t AcdCalibUtil::width(UInt_t id) {
  UInt_t face = id / 100;
  UInt_t row = (id % 100 ) / 10;
  //UInt_t col = (id % 10 );

  // stupid temp hack
  if ( id == 34 ) return 12.;

  switch ( face ) {
  case 0:
    return row == 2 ? 12. : 10.;
  case 1:
  case 2:
  case 3:
  case 4:
    return 10.;
  case 5:
  case 6:
    return 6.;
  }
  return -1.;
}
