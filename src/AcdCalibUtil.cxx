#define AcdCalibUtil_cxx

#include "AcdCalibUtil.h"

#include "AcdCalibEnum.h"
#include "AcdPadMap.h"

#include "AcdHistCalibMap.h"
#include "AcdCalibMap.h"

#include "CalibData/Acd/AcdCalibObj.h"

#include "TString.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TPad.h"
#include "TH1.h"
#include "TF1.h"
#include "TMath.h"
#include <cmath>


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

void AcdCalibUtil::drawPedPlot(TVirtualPad& vp,  TH1& hist, CalibData::AcdCalibObj* res) {
  TVirtualPad* p = vp.cd();
  p->SetLogy();  
  hist.Draw();
  if ( res != 0 ) {
    float ped = (*res)[0];
    TLine* lmean = new TLine( ped, 0., ped, hist.GetMaximum());
    lmean->SetLineColor(2);
    lmean->Draw();
  }   
}


void AcdCalibUtil::drawRangePlot(TVirtualPad& vp, TH1& lowHist, TH1& hiHist, CalibData::AcdCalibObj* res) {
  TVirtualPad* p = vp.cd();
  p->SetLogy();
  lowHist.Draw();
  hiHist.SetLineColor(4);
  hiHist.Draw("same");
  if ( res != 0 ) {
    float low_max = (*res)[0];
    float hi_min = (*res)[1];
    TLine* llow = new TLine( low_max, 0., low_max, lowHist.GetMaximum());
    TLine* lhi = new TLine( hi_min, 0., hi_min, lowHist.GetMaximum());
    llow->SetLineColor(2);
    llow->Draw();
    lhi->SetLineColor(2);
    lhi->Draw();
  }   
}


void AcdCalibUtil::drawVetoPlot(TVirtualPad& vp, TH1& hVeto, TH1& hAll, CalibData::AcdCalibObj* res) {
  vp.cd();
  hAll.Draw();
  hVeto.SetLineColor(4);
  hVeto.Draw("same");

  if ( res != 0 ) {
    float veto = (*res)[0];
    TLine* lmean = new TLine( veto, 0., veto, hAll.GetMaximum());
    lmean->SetLineColor(2);
    lmean->Draw();
  }   
}

void AcdCalibUtil::drawCnoPlot(TVirtualPad& vp, TH1& hCno, TH1& hAll, CalibData::AcdCalibObj* res) {
  vp.cd();
  //p->SetLogy(kTRUE);
  hAll.Draw();
  hCno.SetLineColor(4);
  hCno.Draw("same");

  if ( res != 0 ) {
    float cno = (*res)[0];
    TLine* lmean = new TLine( cno, 0.,  cno, hAll.GetMaximum());
    lmean->SetLineColor(2);
    lmean->Draw();
  }   
}

void AcdCalibUtil::drawMipPlot(TVirtualPad& vp, TH1& hist, CalibData::AcdCalibObj* res, Bool_t onLog) {

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
    float peak = (*res)[0];
    TLine* lA = new TLine( peak, hist.GetMinimum(),  peak, hist.GetMaximum());
    lA->SetLineColor(2);
    lA->Draw();
  } 
}

AcdPadMap* AcdCalibUtil::drawPeds(AcdHistCalibMap& hPeds,
				  AcdCalibMap& peds, const char* prefix) {
  AcdPadMap* padMap = new AcdPadMap(hPeds.config(),prefix);
  TList& hList = (TList&)hPeds.histograms();
  UInt_t n = hList.GetEntries();
  for ( UInt_t i(0); i < n; i++ ) {
    TObject* obj = hList.At(i);
    if ( obj == 0 ) continue;
    UInt_t id = obj->GetUniqueID();
    TVirtualPad* pad = padMap->getPad(id);
    if ( pad == 0 ) continue;
    TH1* hP = (TH1*)(obj);
    if ( hP == 0 ) continue;
    CalibData::AcdCalibObj* res = peds.get(id);
    drawPedPlot(*pad,*hP,res);
  }
  return padMap;
}

AcdPadMap* AcdCalibUtil::drawRanges(AcdHistCalibMap& hRanges,
				    AcdCalibMap& ranges, const char* prefix) {
  AcdPadMap* padMap = new AcdPadMap(hRanges.config(),prefix);
  TList& hList = (TList&)hRanges.histograms();
  UInt_t n = hList.GetEntries();
  for ( UInt_t i(0); i < n; i+=2 ) {
    TObject* obj = hList.At(i);
    if ( obj == 0 ) continue;
    UInt_t id = obj->GetUniqueID();
    TVirtualPad* pad = padMap->getPad(id);
    if ( pad == 0 ) continue;
    TH1* hL = (TH1*)(obj);
    TH1* hH = (TH1*)(hList.At(i+1));
    if ( hL == 0 || hH == 0) continue;
    CalibData::AcdCalibObj* res = ranges.get(id);
    drawRangePlot(*pad,*hL,*hH,res);
  }
  return padMap;
}

AcdPadMap* AcdCalibUtil::drawCnos(AcdHistCalibMap& hCno, AcdHistCalibMap& hRaw,
				  AcdCalibMap& cnos, const char* prefix) {

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
    CalibData::AcdCalibObj* res = cnos.get(id);
    drawCnoPlot(*pad,*hv,*hr,res);
  }
  return padMap;
}

AcdPadMap* AcdCalibUtil::drawVetos(AcdHistCalibMap& hVeto, AcdHistCalibMap& hRaw,
				   AcdCalibMap& vetos, const char* prefix) {

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
    CalibData::AcdCalibObj* res = vetos.get(id);
    drawVetoPlot(*pad,*hv,*hr,res);
  }
  return padMap;
}

AcdPadMap* AcdCalibUtil::drawMips(AcdHistCalibMap& h, AcdCalibMap& gains, 
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
    CalibData::AcdCalibObj* res = gains.get(id);
    drawMipPlot(*pad,*hh,res,onLog);
  }
  return padMap;
}

AcdPadMap* AcdCalibUtil::drawRibbons(AcdHistCalibMap& h, AcdCalibMap& /* gains */,
				     Bool_t onLog, const char* prefix) {

  AcdPadMap* padMap = new AcdPadMap(AcdMap::RIBBONS,prefix);
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
    //CalibData::AcdCalibObj* res = gains.get(id);
    CalibData::AcdCalibObj* res(0);
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
    TF1* fit = hh->GetFunction("ring");
    if ( fit != 0 ) {
      fit->SetLineWidth(1);
      fit->SetLineColor(4);
      hh->SetMinimum(-50.);
      hh->SetMaximum(50.);
    }    
    fit = hh->GetFunction("calib");
    if ( fit != 0 ) {
      fit->SetLineWidth(1);
      fit->SetLineColor(4);
    }
    
    pad->cd();
    hh->SetLineWidth(1);
    hh->SetLineColor(1);
    hh->SetMarkerColor(1);
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
  case AcdCalib::PLAIN: break;
  case AcdCalib::MEAN_ABSOLUTE: 
  case AcdCalib::MEAN_RELATIVE: 
  case AcdCalib::MEAN_SIGMA: 
    calcMean = kTRUE;
    break;
  }


  for ( i = 1; i <= nBin; i++ ) {
    Float_t val = input.GetBinContent(i);
    Float_t err = input.GetBinError(i);
    Float_t valDif = val - ref;
    switch ( method ) {
    case AcdCalib::PLAIN:
    case AcdCalib::MEAN_ABSOLUTE:
      break;
    case AcdCalib::MEAN_RELATIVE:
      if ( ref > 1e-9 ) { valDif /= ref; }
      break;
    case AcdCalib::MEAN_SIGMA:
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

  // FIXME stupid temp hack
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
