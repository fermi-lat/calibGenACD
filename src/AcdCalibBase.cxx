#include <fstream>
#include "TH1F.h"
#include "TF1.h"
#include "AcdCalibBase.h"

#include "AcdHistCalibMap.h"
#include "AcdPedestalFit.h"
#include "AcdGainFit.h"

#include "digiRootData/DigiEvent.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;

ClassImp(AcdCalibBase) ;

AcdCalibBase::AcdCalibBase()
  :m_calType(PEDESTAL),
   m_rawMap(0),m_peakMap(0),m_unpairedMap(0),
   m_pedestals(0),m_gains(0),
   m_histFile(0) {
}


AcdCalibBase::~AcdCalibBase() 
{
  delete m_rawMap;
  delete m_peakMap;
  delete m_unpairedMap;
  delete m_pedestals;
  delete m_gains;
}

void AcdCalibBase::fillPedestalHist(int face, int row, int col, int pmtId, int range, int pha)
{
  
  UInt_t histId = AcdMap::makeKey(range,pmtId,face,row,col);
  
  if ( ! AcdMap::channelExists(face,row,col) ) {
    cout << "Missing " << face << ' ' << row << ' ' << col << endl;
    return;
  }
  if ( range != 0 ) return;
  
  m_rawMap->getHist(histId)->Fill(pha);
}

Float_t AcdCalibBase::correctPhaForPedestal(int face, int row, int col, int pmtId, int pha) const {

  UInt_t histId_lowRange = AcdMap::makeKey(0,pmtId,face,row,col);
  
  const AcdPedestalFitResult* pedestal = m_pedestals == 0 ? 0 : m_pedestals->get(histId_lowRange);
  Float_t pedestalMean = pedestal == 0 ? 0. : pedestal->mean();

  Float_t corrValue = (Float_t)(pha) - pedestalMean;

  Float_t pedestalWidth = pedestal == 0 ? 0. : pedestal->rms();
  Float_t pedestalCutValue = 2. * pedestalWidth;

  return ( corrValue < pedestalCutValue ) ? -1.*corrValue : corrValue;
}

void AcdCalibBase::fillGainHist(int face, int row, int col, int pmtId, int range, int pha) {
  Float_t corrPha = correctPhaForPedestal(face,row,col,pmtId,pha);
  if ( corrPha < 0 ) return;
  corrPha *= reconCorrection(face);
  UInt_t histId = AcdMap::makeKey(range,pmtId,face,row,col);  
  TH1* aHist = m_peakMap->getHist(histId);
  if ( ! AcdMap::channelExists(face,row,col) ) {
    cout << "Missing " << face << ' ' << row << ' ' << col << endl;
  }
  if ( aHist == 0 ) {
    std::cout << "Missing channel " << face << ' ' << row << ' ' << col << ' ' << pmtId << ' ' << range << std::endl;
    return;
  }  
  aHist->Fill(corrPha);
}


void AcdCalibBase::fillUnpairedHist(int face, int row, int col, int pmtId, int range, int pha) {
  UInt_t histId = AcdMap::makeKey(range,pmtId,face,row,col);  
  TH1* aHist = m_unpairedMap->getHist(histId);
  if ( ! AcdMap::channelExists(face,row,col) ) {
    cout << "Missing " << face << ' ' << row << ' ' << col << endl;
  }
  if ( aHist == 0 ) {
    std::cout << "Missing channel " << face << ' ' << row << ' ' << col << ' ' << pmtId << ' ' << range << std::endl;
    return;
  }
  aHist->Fill((Float_t)pha);
}

Bool_t AcdCalibBase::bookHists(const char* fileName,
			       UInt_t nBinGain, Float_t lowGain, Float_t hiGain,
			       UInt_t nBinPed, Float_t lowPed, Float_t hiPed) {
  if ( fileName != 0 ) {
    m_histFile = TFile::Open(fileName, "RECREATE");
    if ( m_histFile == 0 ) return kFALSE;
  }
  m_rawMap = new AcdHistCalibMap("Raw",nBinPed,lowPed,hiPed);
  m_unpairedMap = new AcdHistCalibMap("Unpaired",nBinPed,lowPed,hiPed);
  m_peakMap = new AcdHistCalibMap("Peak",nBinGain,lowGain,hiGain);
  return kTRUE;
} 

AcdPedestalFitMap* AcdCalibBase::fitPedestals(AcdPedestalFit& fitter) {
  if ( m_pedestals != 0 ) {
    std::cout << "Warning: replacing old pedestal fits" << std::endl;
    delete m_pedestals;
  }
  m_pedestals = new AcdPedestalFitMap;
  fitter.fitAll(*m_pedestals,*m_rawMap);
  return m_pedestals;
}

AcdGainFitMap* AcdCalibBase::fitGains(AcdGainFit& fitter) {
  if ( m_gains != 0 ) {
    std::cout << "Warning: replacing old gain fits" << std::endl;
    delete m_gains;
  }
  m_gains = new AcdGainFitMap;
  fitter.fitAll(*m_gains,*m_peakMap);
  return m_gains;
}  

Bool_t AcdCalibBase::writeHistograms(CALTYPE type, const char* newFileName ) {
  if ( newFileName != 0 ) {
     m_histFile = TFile::Open(newFileName, "RECREATE");
     if ( m_histFile == 0 ) return kFALSE;
  }
  if(m_histFile == 0 ) return kFALSE;
  
  m_histFile->cd();
  
  switch ( type ) {
  case PEDESTAL:  
    if ( m_rawMap != 0 ) m_rawMap->histograms().Write();
    break;
  case GAIN:
    if ( m_peakMap != 0 ) m_peakMap->histograms().Write();
    break;
  case UNPAIRED:
    if ( m_unpairedMap != 0 ) m_unpairedMap->histograms().Write();
    break;
  } 

  return kTRUE;
}
