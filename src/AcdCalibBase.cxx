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
  resetCounters();
}


AcdCalibBase::~AcdCalibBase() 
{
  delete m_rawMap;
  delete m_peakMap;
  delete m_unpairedMap;
  delete m_pedestals;
  delete m_gains;
}

void AcdCalibBase::go(int numEvents, int startEvent) {

  m_startEvent = startEvent;
  int nTotal = getTotalEvents();
  int nLast = numEvents < 1 ? nTotal : TMath::Min(numEvents+startEvent,nTotal);

  cout << "Number of events in the chain: " << nTotal << endl;
  cout << "Number of events used: " << nLast-startEvent << endl;
  cout << "Starting at event: " << startEvent << endl;

  for (Int_t ievent= startEvent; ievent!=nLast; ievent++ ) {
    
    Bool_t filtered(kFALSE);
    Bool_t ok = readEvent(ievent,filtered,m_runId,m_evtId);

    if ( !ok ) {
      cout << "Failed to read event " << ievent << " aborting" << endl;
      break;
    }

    Bool_t used(kFALSE);
    if ( !filtered ) {
      useEvent(used);
    }
    
    logEvent(ievent,used,m_runId,m_evtId);
  }

  lastEvent(m_runId,m_evtId);

}

void AcdCalibBase::fillPedestalHist(int id, int pmtId, int pha)
{
  
  UInt_t histId = AcdMap::makeKey(pmtId,id);
  
  if ( ! AcdMap::channelExists(id) ) {
    cout << "Missing " << id << endl;
    return;
  }
  m_rawMap->getHist(histId)->Fill(pha);
}


void AcdCalibBase::fillGainHist(int id, int pmtId, float phaCorrect) {

  if ( !  AcdMap::channelExists(id) ) {
    cout << "Missing " << id << endl;
    return;
  }

  UInt_t histId = AcdMap::makeKey(pmtId,id);
  TH1* aHist = m_peakMap->getHist(histId);
  if ( aHist == 0 ) {
    std::cout << "Missing channel " << id << ' ' << pmtId << std::endl;
    return;
  }  
  aHist->Fill(phaCorrect);
}


void AcdCalibBase::fillUnpairedHist(int id, int pmtId, int pha) {

  if ( ! AcdMap::channelExists(id) ) {
    cout << "Missing " << id << endl;
  }
  UInt_t histId = AcdMap::makeKey(pmtId,id);  
  TH1* aHist = m_unpairedMap->getHist(histId);
  if ( aHist == 0 ) {
    std::cout << "Missing channel " << id << ' ' << pmtId << std::endl;
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

Bool_t AcdCalibBase::readPedestals(const char* fileName) {
  if ( m_pedestals != 0 ) {
    std::cout << "Warning: replacing old pedestal fits" << std::endl;
    delete m_pedestals;
  }
  m_pedestals = new AcdPedestalFitMap;
  return m_pedestals->readTxtFile(fileName);
}

void AcdCalibBase::logEvent(int ievent, Bool_t passedCut, int runId,int evtId) {

  m_evtId = evtId;
  m_runId = runId;
  m_nTrigger++;
  if ( passedCut ) m_nUsed++;  
  if ( ievent == m_startEvent ) { 
    firstEvent(runId,evtId);
  }
  else if ( ievent % 100000 == 0 ) { std::cout << 'x' << std::endl; }
  else if ( ievent % 10000 == 0 ) { std::cout << 'x' << std::flush; }
  else if ( ievent % 1000 == 0 ) { std::cout << '.' << std::flush; }
}
