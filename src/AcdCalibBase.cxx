#include <fstream>
#include "TH1F.h"
#include "TF1.h"
#include "AcdCalibBase.h"

#include "./AcdHistCalibMap.h"
#include "./AcdPedestalFit.h"
#include "./AcdGainFit.h"
#include "./AcdRangeFit.h"
#include "./AcdCalibMap.h"

#include "AcdXmlUtil.h"
#include "DomElement.h"

#include "digiRootData/DigiEvent.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;

ClassImp(AcdCalibBase) ;

AcdCalibBase::AcdCalibBase(CALTYPE t, AcdMap::Config config)
  :m_config(config),
   m_calType(t){
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

  cout << endl;

}

void AcdCalibBase::fillHist(AcdHistCalibMap& histMap, int id, int pmtId, float val, UInt_t idx)
{
  UInt_t histId = AcdMap::makeKey(pmtId,id);
  
  if ( ! AcdMap::channelExists(id) ) {
    cout << "Missing " << id << endl;
    return;
  }
  TH1* hist = histMap.getHist(histId,idx);
  if ( hist == 0 ) {
    cout << "No histogram " << histId << ' ' << pmtId << ' ' << id << endl;
  }
  hist->Fill(val);
}

void AcdCalibBase::fillHistBin(AcdHistCalibMap& histMap, int id, int pmtId, UInt_t binX, Float_t val, Float_t err, UInt_t idx)
{
  UInt_t histId = AcdMap::makeKey(pmtId,id);
  
  if ( ! AcdMap::channelExists(id) ) {
    cout << "Missing " << id << endl;
    return;
  }
  TH1* hist = histMap.getHist(histId,idx);
  if ( hist == 0 ) {
    cout << "No histogram " << histId << ' ' << pmtId << ' ' << id << endl;
  }
  hist->SetBinContent(binX,val);
  hist->SetBinError(binX,err);
}



AcdHistCalibMap* AcdCalibBase::bookHists( int histType, UInt_t nBin, Float_t low, Float_t hi, UInt_t nHist ) {
  AcdHistCalibMap* map = getHistMap(histType);
  if ( map != 0 ) {
    std::cout << "Warning: replacing old histograms" << std::endl;
    delete map;
  }
  TString name;
  switch (histType) {
  case H_RAW: name += "RAW"; break;
  case H_GAIN: name += "GAIN"; break;
  case H_VETO: name += "VETO"; break;
  case H_UNPAIRED: name += "UNPAIRED"; break;
  case H_FRAC: name += "FRAC"; break;
  case H_RANGE: name += "RANGE"; break;
  case H_TIME_PHA: name += "TIME_PROFILE_PHA"; break;
  case H_TIME_HIT: name += "TIME_PROFILE_HIT"; break;
  case H_TIME_VETO: name += "TIME_PROFILE_VETO"; break;
  case H_COHERENT_NOISE: name += "DELTA_T_PROFILE"; break;
  case H_XOVER: name += "XOVER"; break;
  }

  map = new AcdHistCalibMap(name,nBin,low,hi,m_config,nHist);
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
  case RANGE:
    map = new AcdRangeFitMap;
    break;
  }
  if ( map == 0 ) return kFALSE;
  addCalibration(calKey,*map);
  return map->readTxtFile(fileName);
}


void AcdCalibBase::writeXmlHeader(DomElement& node) const {
  
  DomElement sourceNode = AcdXmlUtil::makeChildNode(node,"inputSample");
  
  AcdXmlUtil::addAttribute(sourceNode,"startTime","0");
  AcdXmlUtil::addAttribute(sourceNode,"stopTime","0");
  AcdXmlUtil::addAttribute(sourceNode,"triggers","0");
  AcdXmlUtil::addAttribute(sourceNode,"source","0");
  AcdXmlUtil::addAttribute(sourceNode,"mode","0"); 
  //os << "  <inputSample startTime=\"" << runId_first() << ':'  << evtId_first() 
  //   << "\" stopTime=\"" << runId_last() << ':'  << evtId_last() 
  //   << "\" triggers=\"" << nUsed() << '/' << nFilter() << '/' << nTrigger() 
  //  << "\" source=\"" << 0 
  //   << "\" mode=\"" << 0
  //  << "\">" << endl; 
  writeXmlSources(sourceNode);
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
