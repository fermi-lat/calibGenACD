#define AcdVetoFit_cxx

#include "AcdVetoFit.h"

#include "AcdHistCalibMap.h"
#include "TF1.h"

ClassImp(AcdVetoFitDesc) ;

const std::string AcdVetoFitDesc::s_calibType("ACD_Veto");
const std::string AcdVetoFitDesc::s_txtFormat("TILE PMT VETO WIDTH STATUS");

const AcdVetoFitDesc& AcdVetoFitDesc::ins() {
  static const AcdVetoFitDesc desc;
  return desc;
}

AcdVetoFitDesc::AcdVetoFitDesc()
  :AcdCalibDescription(AcdCalib::VETO,s_calibType,s_txtFormat){
  addVarName("veto");
  addVarName("width");
}

ClassImp(AcdVetoFitLibrary) ;


Int_t AcdVetoFitLibrary::findFirstBinAboveVal(const TH1& hist, Float_t val) {
  UInt_t nB = hist.GetNbinsX();
  // skip the first bin...
  for ( UInt_t i(2); i <= nB; i++ ) {
    if ( hist.GetBinContent(i) > val ) {
      return (Int_t)i;
    }
  }
  return -1;   
}

Int_t AcdVetoFitLibrary::fit(AcdCalibResult& result, const AcdCalibHistHolder& holder) {

  TH1& hist = const_cast<TH1&>(*(holder.getHist(0)));
  
  Int_t returnCode = AcdCalibResult::NOFIT;
  switch ( _type ) {
  case None:
    result.setVals(0.,0.,AcdCalibResult::NOFIT);
    break;
  case Counting:
    returnCode = counting(result,hist);
    break;
  case Erf:
    returnCode = fitErf(result,hist);
    break;
  }  
  return returnCode;
}

Int_t AcdVetoFitLibrary::counting(AcdCalibResult& result, const TH1& hist) {

  //TH1& theHist = const_cast<TH1&>(hist);
  
  Int_t i_10 = findFirstBinAboveVal(hist,0.1);
  Float_t f_10 = hist.GetBinCenter(i_10);
  Int_t i_50 = findFirstBinAboveVal(hist,0.5);
  Float_t f_50 = hist.GetBinCenter(i_50);
  Float_t width = f_50 - f_10;
  result.setVals(f_50,width,AcdCalibResult::OK);
  return AcdCalibResult::OK;
}


Int_t AcdVetoFitLibrary::fitErf(AcdCalibResult& /* result */, const TH1& /* hist */) {
  
  return AcdCalibResult::NOFIT;

}


