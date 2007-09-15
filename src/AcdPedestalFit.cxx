#define AcdPedestalFit_cxx

#include "AcdPedestalFit.h"

#include "AcdHistCalibMap.h"

ClassImp(AcdPedestalFitDesc) ;

const std::string AcdPedestalFitDesc::s_calibType("ACD_Ped");
const std::string AcdPedestalFitDesc::s_txtFormat("TILE PMT MEAN WIDTH STATUS");

const AcdPedestalFitDesc& AcdPedestalFitDesc::ins() {
  static const AcdPedestalFitDesc desc;
  return desc;
}

AcdPedestalFitDesc::AcdPedestalFitDesc()
  :AcdCalibDescription(AcdCalib::PEDESTAL,s_calibType,s_txtFormat){
  addVarName("mean");
  addVarName("width");
}


ClassImp(AcdPedestalFitLibrary) ;

Int_t AcdPedestalFitLibrary::fit(AcdCalibResult& result, const AcdCalibHistHolder& holder) {
  
  Int_t returnCode(-1);
  switch ( _type ) {
  case None:
    result.setVals(0.,0.,AcdCalibResult::NOFIT);
    break;
  case PeakValue:
    returnCode = fitPeak(result,holder);
    break;
  case MeanValue:
    returnCode = fitMean(result,holder);  
    break;
  }
  
  return returnCode;
}

UInt_t AcdPedestalFitLibrary::fitMean(AcdCalibResult& result, const AcdCalibHistHolder& holder) {
  TH1& theHist = const_cast<TH1&>(*(holder.getHist(0)));
  float av = theHist.GetMean(); 
  float rms = theHist.GetRMS();
  theHist.SetAxisRange(av-5*rms,av+5*rms);
  av = theHist.GetMean(); rms = theHist.GetRMS();
  theHist.SetAxisRange(av-5*rms,av+5*rms);
  av = theHist.GetMean(); rms = theHist.GetRMS();
  result.setVals(av,rms,AcdCalibResult::OK);
  return 1;
}


UInt_t AcdPedestalFitLibrary::fitPeak(AcdCalibResult& result, const AcdCalibHistHolder& holder) {
  TH1& hist = const_cast<TH1&>(*(holder.getHist(0)));
  Long64_t maxbin = hist.GetMaximumBin(); //finds tallest bin
  float pedestal = hist.GetBinCenter(maxbin); 
  result.setVals(pedestal,0.,AcdCalibResult::OK);
  return 1;
}
