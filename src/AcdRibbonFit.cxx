#define AcdRibbonFit_cxx

#include "AcdRibbonFit.h"
#include "AcdHistCalibMap.h"

#include "TF1.h"


Float_t AcdRibbonFitLibrary::getLocalX(int id, Float_t x, Float_t y, Float_t z) {
  //std::cout << id << ' ' << x << ' ' << y << ' ' << z << std::endl;
  static const Float_t zTop(720.);
  static const Float_t wTop(825.);
  int ribType = ( ( id % 1000 ) / 100 );
  Float_t top = ribType == 5 ? x : y;
  // check for top
  if ( z > zTop ) {
    return top;
  }
  Float_t absDist = wTop + ( zTop - z );
  // check for +X or +Y side
  if ( top < 0. ) absDist *= -1.;
  return absDist;
}

Int_t AcdRibbonFitLibrary::getBin(int /* id */, Float_t localY) {
  if ( localY < -1800. ) return -1;
  else if ( localY < -1200. ) return 0;
  else if ( localY < -720. ) return 1;
  else if ( localY < -300. ) return 2;
  else if ( localY < 300. ) return 3;
  else if ( localY < 720. ) return 4;
  else if ( localY < 1200. ) return 5;
  else if ( localY < 1800. ) return 6;
  else return -1;
}


Int_t AcdRibbonFitLibrary::fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder,
			       CalibData::AcdCalibObj* /* ref */ ) {

  Float_t peakVals[7];

  for (UInt_t i(0); i < 7; i++ ) {
    CalibData::AcdGain gainFit(0.,0.,CalibData::AcdCalibObj::NOFIT);
    TH1& hist = const_cast<TH1&>(*(holder.getHist(i)));
    Int_t returnCode = CalibData::AcdCalibObj::NOFIT;
    if ( hist.GetEntries() < 100 ) return returnCode;
    switch ( fitType() ) {
    case None:
      peakVals[i] = 0.;
      break;
    case Stats:
      returnCode = stats(gainFit,hist);
      break;
    case Fallback:
      returnCode = fallback(gainFit,hist);
      break;
    case Landau:
      returnCode = fitLandau(gainFit,hist);
      break;
    case P7:
      returnCode = fitP7(gainFit,hist);
      break;
    case P3:
      returnCode = fitP3(gainFit,hist);
      break;
    case LogNormal:
      returnCode = fitLogNormal(gainFit,hist);
      break;
    }  
    if ( returnCode != CalibData::AcdCalibObj::OK ) return returnCode;    
    peakVals[i] = gainFit.getPeak();
  }

  Float_t norm = peakVals[3];
  if ( norm < 15. ) return CalibData::AcdCalibObj::NOFIT;

  for ( UInt_t iNorm(0); iNorm < 7; iNorm++ ) {
    if ( iNorm == 3 ) continue;
    peakVals[iNorm] /= norm;
  }

  result.setVals(peakVals[0],peakVals[1],peakVals[2],norm,
		 peakVals[4],peakVals[5],peakVals[6],
		 CalibData::AcdCalibObj::OK);

  return CalibData::AcdCalibObj::OK;
}



