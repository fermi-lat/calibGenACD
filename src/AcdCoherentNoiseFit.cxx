#define AcdCoherentNoiseFit_cxx

#include "AcdCoherentNoiseFit.h"
#include "AcdCalibEnum.h"

#include "AcdHistCalibMap.h"

#include <TF1.h>


Int_t AcdCoherentNoiseFitLibrary::fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder) {

  TH1& in = *(holder.getHist(0));

  Float_t min = 1e9; 
  Float_t minTime = 0.;
  Float_t max = -1e9;
  Float_t maxTime = 0.;

  Int_t nBin = in.GetNbinsX();
  for ( Int_t i = 1; i <= nBin; i++ ) {
    Float_t val = in.GetBinContent(i);
    if ( val < min ) {
      min = val;
      minTime = in.GetBinCenter(i);
    }
    if ( val > max ) {
      max = val;
      maxTime = in.GetBinCenter(i);	
    }
  }

  TF1 ring("ring","[0] * exp(-x/[1]) * sin(x*[2] + [3])");

  ring.SetParameter(0,40.);   // amplitude
  ring.SetParLimits(0,0.,100.);  

  ring.SetParameter(1,1000.); // decay const
  ring.SetParLimits(1,100.,2000.);  

  ring.FixParameter(2,0.0054); // frequency

  ring.SetParameter(3,0);     // phase
  ring.SetParLimits(3,-5.,5.);

  TH1& nch = const_cast<TH1&>(in);
  Int_t status = nch.Fit(&ring,"","");
  

  //Float_t amp = ring.GetParameter(0);
  //Float_t decay = ring.GetParameter(1);
  //Float_t freq = ring.GetParameter(2);
  //Float_t phase = ring.GetParameter(3);

  //char fitPars[100];
  //sprintf(fitPars,"%3.1f %4.1f %5.4f %4.3f",amp,decay,freq,phase);
  
  //result.setVals(min,minTime,max,maxTime,fitPars,(AcdCoherentNoiseFitResult::STATUS)status);
  result.setVals(min,minTime,max,maxTime,(CalibData::AcdCalibObj::STATUS)status);
  return result.getStatus();
}


