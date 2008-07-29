#define AcdPedestalFit_cxx

#include "AcdPedestalFit.h"

#include "AcdHistCalibMap.h"



Int_t AcdPedestalFitLibrary::fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder,
				 CalibData::AcdCalibObj* /* ref */ ) {
  
  Int_t returnCode(-1);
  switch ( _type ) {
  case None:
    result.setVals(0.,0.,CalibData::AcdCalibObj::NOFIT);
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

UInt_t AcdPedestalFitLibrary::fitMean(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder) {
  TH1& theHist = const_cast<TH1&>(*(holder.getHist(0)));
  float av = theHist.GetMean(); 
  float rms = theHist.GetRMS();
  theHist.SetAxisRange(av-5*rms,av+5*rms);
  av = theHist.GetMean(); rms = theHist.GetRMS();
  theHist.SetAxisRange(av-5*rms,av+5*rms);
  av = theHist.GetMean(); rms = theHist.GetRMS();
  result.setVals(av,rms,CalibData::AcdCalibObj::OK);
  return 1;
}


UInt_t AcdPedestalFitLibrary::fitPeak(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder) {
  TH1& hist = const_cast<TH1&>(*(holder.getHist(0)));
  Long64_t maxbin = hist.GetMaximumBin(); //finds tallest bin
  float pedestal = hist.GetBinCenter(maxbin); 
  result.setVals(pedestal,0.,CalibData::AcdCalibObj::OK);
  return 1;
}
