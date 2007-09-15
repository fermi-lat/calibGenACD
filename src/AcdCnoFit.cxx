#define AcdCnoFit_cxx

#include "AcdCnoFit.h"
#include "AcdHistCalibMap.h"

#include "TF1.h"

ClassImp(AcdCnoFitDesc) ;

const std::string AcdCnoFitDesc::s_calibType("ACD_Cno");
const std::string AcdCnoFitDesc::s_txtFormat("TILE PMT CNO WIDTH STATUS");

const AcdCnoFitDesc& AcdCnoFitDesc::ins() {
  static const AcdCnoFitDesc desc;
  return desc;
}

AcdCnoFitDesc::AcdCnoFitDesc()
  :AcdCalibDescription(AcdCalib::CNO,s_calibType,s_txtFormat){
  addVarName("cno");
  addVarName("width");
}



ClassImp(AcdCnoFitLibrary) ;


Int_t AcdCnoFitLibrary::findFirstBinAboveVal(const TH1& hist, Float_t val) {
  UInt_t nB = hist.GetNbinsX();
  // skip the first bin...
  for ( UInt_t i(2); i <= nB; i++ ) {
    if ( hist.GetBinContent(i) > val ) {
      return (Int_t)i;
    }
  }
  return -1;   
}

Int_t AcdCnoFitLibrary::fit(AcdCalibResult& result, const AcdCalibHistHolder& holder) {

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

Int_t AcdCnoFitLibrary::counting(AcdCalibResult& result, const TH1& hist) {

  //TH1& theHist = const_cast<TH1&>(hist);
  
  Int_t i_10 = findFirstBinAboveVal(hist,0.1);
  Float_t f_10 = hist.GetBinCenter(i_10);
  Int_t i_50 = findFirstBinAboveVal(hist,0.5);
  Float_t f_50 = hist.GetBinCenter(i_50);
  Float_t width = f_50 - f_10;
  result.setVals(f_50,width,AcdCalibResult::OK);
  return AcdCalibResult::OK;
}


Int_t AcdCnoFitLibrary::fitErf(AcdCalibResult& /* result */, const TH1& /* hist */) {
  
  return AcdCalibResult::NOFIT;

}


