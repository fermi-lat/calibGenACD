#include <fstream>
#include "TH1F.h"
#include "TF1.h"
#include "AcdCalibBase.h"

#include "./AcdHistCalibMap.h"
#include "./AcdPedestalFit.h"
#include "./AcdGainFit.h"
#include "./AcdCalibMap.h"

#include "digiRootData/DigiEvent.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;

ClassImp(AcdCalibBase) ;

AcdCalibBase::AcdCalibBase(AcdMap::Config config)
  :m_config(config),
   m_calType(PEDESTAL){
  resetCounters();
}


AcdCalibBase::~AcdCalibBase()
{
  for ( std::map<int,AcdHistCalibMap*>::iterator itr = m_histMaps.begin(); itr != m_histMaps.end(); itr++ ) {
    AcdHistCalibMap* aMap = itr->second;
    delete aMap;
  }
  for ( std::map<int,AcdCalibMap*>::iterator itr2 = m_fitMaps.begin(); itr2 != m_fitMaps.end(); itr2++ ) {
    AcdCalibMap* aMap2 = itr2->second;
    delete aMap2;
  }
}


void AcdCalibBase::go(int numEvents, int startEvent) {

  m_startEvent = startEvent;
  int nTotal = getTotalEvents();
  m_last = numEvents < 1 ? nTotal : TMath::Min(numEvents+startEvent,nTotal);

  cout << "Number of events in the chain: " << nTotal << endl;
  cout << "Number of events used: " << m_last-startEvent << endl;
  cout << "Starting at event: " << startEvent << endl;

  for (Int_t ievent= startEvent; ievent!=m_last; ievent++ ) {
    
    Bool_t filtered(kFALSE);
    Bool_t ok = readEvent(ievent,filtered,m_runId,m_evtId);

    if ( !ok ) {
      cout << "Failed to read event " << ievent << " aborting" << endl;
      break;
    }

    Bool_t used(kFALSE);
    if ( !filtered ) {
      m_nFilter++;
      useEvent(used);
    }
    
    logEvent(ievent,used,m_runId,m_evtId);
  }

  lastEvent(m_runId,m_evtId);
}

void AcdCalibBase::fillHist(AcdHistCalibMap& histMap, int id, int pmtId, float val)
{
  UInt_t histId = AcdMap::makeKey(pmtId,id);
  
  if ( ! AcdMap::channelExists(id) ) {
    cout << "Missing " << id << endl;
    return;
  }
  TH1* hist = histMap.getHist(histId);
  if ( hist == 0 ) {
    cout << "No histogram " << id << endl;
  }
  hist->Fill(val);
}

void AcdCalibBase::fillHistBin(AcdHistCalibMap& histMap, int id, int pmtId, UInt_t binX, Float_t val, Float_t err)
{
  UInt_t histId = AcdMap::makeKey(pmtId,id);
  
  if ( ! AcdMap::channelExists(id) ) {
    cout << "Missing " << id << endl;
    return;
  }
  TH1* hist = histMap.getHist(histId);
  if ( hist == 0 ) {
    cout << "No histogram " << id << endl;
  }
  hist->SetBinContent(binX,val);
  hist->SetBinError(binX,err);
}



AcdHistCalibMap* AcdCalibBase::bookHists( int histType, UInt_t nBin, Float_t low, Float_t hi ) {
  AcdHistCalibMap* map = getHistMap(histType);
  if ( map != 0 ) {
    std::cout << "Warning: replacing old histograms" << std::endl;
    delete map;
  }
  TString name;
  switch (histType) {
  case PEDESTAL: name += "PED"; break;
  case GAIN: name += "GAIN"; break;
  case UNPAIRED: name += "UNPAIRED"; break;
  case RAW: name += "RAW"; break;
  case VETO: name += "VETO"; break;
  case VETO_FRAC: name += "VETO_FRAC"; break;
  case TIME_PROF_PHA: name += "TIME_PROFILE_PHA"; break;
  case TIME_PROF_HIT: name += "TIME_PROFILE_HIT"; break;
  case TIME_PROF_VETO: name += "TIME_PROFILE_VETO"; break;
  }

  map = new AcdHistCalibMap(name,nBin,low,hi,m_config);
  m_histMaps[histType] = map;
  return map;
} 

void AcdCalibBase::addCalibration(int calibKey, AcdCalibMap& newCal) {  
  AcdCalibMap* old = getCalibMap(calibKey);
  if ( old != 0 ) {
    std::cout << "Warning: replacing calibration" << std::endl;
    delete old;
  }
  m_fitMaps[calibKey] = &newCal;
}

Bool_t AcdCalibBase::writeHistograms(int histType, const char* newFileName ) {
  AcdHistCalibMap* map = getHistMap(histType);
  if ( map == 0 ) return kFALSE;
  return map->writeHistograms(newFileName);
}

Bool_t AcdCalibBase::readCalib(int calKey, const char* fileName) {
  AcdCalibMap* map = getCalibMap(calKey);
  if ( map != 0 ) {
    std::cout << "Warning: replacing old calibration" << std::endl;
    delete map;
  }
  switch ( calKey ) {
  case PEDESTAL:
    map = new AcdPedestalFitMap;
    break;
  case GAIN:
    map = new AcdGainFitMap;
    break;
  }
  if ( map == 0 ) return kFALSE;
  addCalibration(calKey,*map);
  return map->readTxtFile(fileName);
}


void AcdCalibBase::writeXmlHeader(ostream& os) const {
  os << "  <inputSample startTime=\"" << runId_first() << ':'  << evtId_first() 
     << "\" stopTime=\"" << runId_last() << ':'  << evtId_last() 
     << "\" triggers=\"" << nUsed() << '/' << nFilter() << '/' << nTrigger() 
     << "\" source=\"" << 0 
     << "\" mode=\"" << 0
     << "\">" << endl; 
  writeXmlSources(os);
  os << "</inputSample>" << endl;
}

void AcdCalibBase::writeTxtHeader(ostream& os) const {
  os << "#startTime = " << runId_first() << ':'  << evtId_first() << endl
     << "#stopTime = " << runId_last() << ':'  << evtId_last() << endl
     << "#triggers = " << nUsed() << '/' << nFilter() << '/' << nTrigger() << endl
     << "#source = " << endl
     << "#mode = " << 0 << endl;
  writeTxtSources(os);
}

void AcdCalibBase::logEvent(int ievent, Bool_t passedCut, int runId,int evtId) {

  m_evtId = evtId;
  m_runId = runId;
  m_nTrigger++;
  if ( passedCut ) {
    m_nUsed++;  
  }
  if ( ievent == m_startEvent ) { 
    firstEvent(runId,evtId);
  }
  else if ( ievent % 1000000 == 0 ) { std::cout << "x " << ievent / 1000000 << 'M' << std::endl; }
  else if ( ievent % 100000 == 0 ) { std::cout << 'x' << std::endl; }
  else if ( ievent % 10000 == 0 ) { std::cout << 'x' << std::flush; }
  else if ( ievent % 1000 == 0 ) { std::cout << '.' << std::flush; }
}

// get the maps of the histograms to be fit
AcdHistCalibMap* AcdCalibBase::getHistMap(int key) {
  std::map<int,AcdHistCalibMap*>::iterator itr = m_histMaps.find(key);
  return itr == m_histMaps.end() ? 0 : itr->second;
}

// get the results maps
AcdCalibMap* AcdCalibBase::getCalibMap(int key) {
  std::map<int,AcdCalibMap*>::iterator itr = m_fitMaps.find(key);
  return itr == m_fitMaps.end() ? 0 : itr->second;
}
