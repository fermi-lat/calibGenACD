#define AcdCoherentNoiseFit_cxx

#include "AcdCoherentNoiseFit.h"
#include "AcdCalibEnum.h"

#include "AcdHistCalibMap.h"

#include <TF1.h>
#include <TMath.h>


Int_t AcdCoherentNoiseFitLibrary::fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder,
				      CalibData::AcdCalibObj* /* ref */ ) {

  TH1& in = *(holder.getHist(0));

  TF1 ring("ring","[0] * exp(-x/[1]) * sin(x*[2] + [3])");

  ring.SetParameter(0,40.);   // amplitude
  ring.SetParLimits(0,0.,100.);  

  ring.SetParameter(1,1000.); // decay const
  ring.SetParLimits(1,100.,2000.);  

  ring.FixParameter(2,0.0054); // frequency

  ring.SetParameter(3,1.5*TMath::Pi());     // phase
  ring.SetParLimits(3,0.,2.*TMath::Pi());

  TH1& nch = const_cast<TH1&>(in);
  Int_t status = nch.Fit(&ring,"","");
  
  // grab parameters
  Float_t amp = ring.GetParameter(0);
  Float_t decay = ring.GetParameter(1);
  Float_t freq = ring.GetParameter(2);
  Float_t phase = ring.GetParameter(3);

  // and stuff them into the result object
  result.setVals(amp,decay,freq,phase,(CalibData::AcdCalibObj::STATUS)status);
  return result.getStatus();
}


