// base class
#include "AcdCalibBase.h"

// stl
#include <fstream>

// ROOT
#include "TH1F.h"
#include "TF1.h"
#include "TChain.h"
#include <TFile.h>

// local headers
#include "AcdHistCalibMap.h"
#include "AcdPedestalFit.h"
#include "AcdGainFit.h"
#include "AcdRangeFit.h"
#include "AcdCalibMap.h"
#include "AcdCalibFit.h"
#include "AcdXmlUtil.h"
#include "DomElement.h"

// other packages
#include "digiRootData/DigiEvent.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;

AcdCalibEventStats::AcdCalibEventStats(){
  resetCounters();
}

void AcdCalibEventStats::logEvent(int ievent, Bool_t passedCut, Bool_t filtered, int runId,int evtId, Double_t timeStamp) {
  m_evtId = evtId;
  m_runId = runId;
  m_evtTime = timeStamp;
  m_nTrigger++;
  if ( passedCut ) {
    m_nUsed++;  
  }
  if ( !filtered) {
    m_nFilter++;
  }
  if ( ievent == m_startEvent ) { 
    firstEvent();
  }
  else if ( ievent % 1000000 == 0 ) { std::cout << "x " << ievent / 1000000 << 'M' << std::endl; }
  else if ( ievent % 100000 == 0 ) { std::cout << 'x' << std::endl; }
  else if ( ievent % 10000 == 0 ) { std::cout << 'x' << std::flush; }
  else if ( ievent % 1000 == 0 ) { std::cout << '.' << std::flush; }
}


AcdCalibBase::AcdCalibBase(AcdCalibData::CALTYPE t, AcdMap::Config config)
  :m_config(config),
   m_calType(t),
   m_histMaps(AcdCalib::H_NHIST,0),
   m_fitMaps(AcdCalibData::NDESC,0),
   m_chains(AcdCalib::NCHAIN,0){  
}


AcdCalibBase::~AcdCalibBase()
{
  for ( std::vector<AcdHistCalibMap*>::iterator itr = m_histMaps.begin(); itr != m_histMaps.end(); itr++ ) {
    AcdHistCalibMap* aMap = *itr;
    delete aMap;
  }
  for ( std::vector<AcdCalibMap*>::iterator itr2 = m_fitMaps.begin(); itr2 != m_fitMaps.end(); itr2++ ) {
    AcdCalibMap* aMap2 = *itr2;
    delete aMap2;
  }
  //for ( std::vector<TChain*>::iterator itr3 = m_chains.begin(); itr3 != m_chains.end(); itr3++ ) {
  //  TChain* aChain = *itr3;
  //  delete aChain;
  //}  
}


void AcdCalibBase::go(int numEvents, int startEvent) {

  int nTotal = getTotalEvents();
  int last = numEvents < 1 ? nTotal : TMath::Min(numEvents+startEvent,nTotal);
  m_eventStats.setRange(startEvent,last);

  cout << "Number of events in the chain: " << nTotal << endl;
  cout << "Number of events used: " << last-startEvent << endl;
  cout << "Starting at event: " << startEvent << endl;

  for (Int_t ievent= startEvent; ievent < last; ievent++ ) {
    
    Bool_t filtered(kFALSE);
    Int_t runId, evtId;
    Double_t timeStamp;
    Bool_t ok = readEvent(ievent,filtered, runId, evtId,timeStamp);

    if ( !ok ) {
      cout << "Failed to read event " << ievent << " aborting" << endl;
      break;
    }

    Bool_t used(kFALSE);
    if ( !filtered ) {
      useEvent(used);
    }
    
    m_eventStats.logEvent(ievent,used,filtered,runId,evtId,timeStamp);
  }

  m_eventStats.lastEvent();

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



AcdHistCalibMap* AcdCalibBase::bookHists(AcdCalib::HISTTYPE histType, UInt_t nBin, Float_t low, Float_t hi, UInt_t nHist ) {
  AcdHistCalibMap* map = getHistMap(histType);
  if ( map != 0 ) {
    std::cout << "Warning: replacing old histograms" << std::endl;
    delete map;
  }
  TString name;

  switch (histType) {
  case AcdCalib::H_RAW: name += "RAW"; break;
  case AcdCalib::H_GAIN: name += "GAIN"; break;
  case AcdCalib::H_VETO: name += "VETO"; break;
  case AcdCalib::H_UNPAIRED: name += "UNPAIRED"; break;
  case AcdCalib::H_FRAC: name += "FRAC"; break;
  case AcdCalib::H_RIBBONS: name += "RIBBONS"; break;
  case AcdCalib::H_RANGE: name += "RANGE"; break;
  case AcdCalib::H_TIME_PHA: name += "TIME_PROFILE_PHA"; break;
  case AcdCalib::H_TIME_HIT: name += "TIME_PROFILE_HIT"; break;
  case AcdCalib::H_TIME_VETO: name += "TIME_PROFILE_VETO"; break;
  case AcdCalib::H_COHERENT_NOISE: name += "DELTA_T_PROFILE"; break;
  case AcdCalib::H_HIGH_RANGE: name += "HIGH_RANGE"; break;    
  case AcdCalib::H_NONE:
  default:
    return 0;
  }

  map = new AcdHistCalibMap(name,nBin,low,hi,m_config,nHist);
  m_histMaps[histType] = map;
  return map;
} 

void AcdCalibBase::addCalibration(AcdCalibData::CALTYPE calibKey, AcdCalibMap& newCal) {  
  AcdCalibMap* old = getCalibMap(calibKey);
  if ( old != 0 ) {
    std::cout << "Warning: replacing calibration" << std::endl;
    delete old;
  }
  m_fitMaps[calibKey] = &newCal;
}

Bool_t AcdCalibBase::writeHistograms(AcdCalib::HISTTYPE histType, const char* newFileName ) {
  AcdHistCalibMap* map = getHistMap(histType);
  if ( map == 0 ) return kFALSE;
  return map->writeHistograms(newFileName);
}

Bool_t AcdCalibBase::readCalib(AcdCalibData::CALTYPE calKey, const char* fileName) {
  AcdCalibMap* map = getCalibMap(calKey);
  if ( map != 0 ) {
    std::cout << "Warning: replacing old calibration" << std::endl;
    delete map;
  }
  const CalibData::AcdCalibDescription* desc = CalibData::AcdCalibDescription::getDesc(calKey);
  if ( desc == 0 ) {
    switch (calKey) {
    case AcdCalibData::PEDESTAL:  desc = &CalibData::AcdPedestalFitDesc::instance(); break;
    case AcdCalibData::GAIN:      desc = &CalibData::AcdGainFitDesc::instance(); break;
    default: 
      ;
    }
  }
  map = new AcdCalibMap(*desc);

  if ( map == 0 ) return kFALSE;
  addCalibration(calKey,*map);

  std::string fName(fileName); 
  if ( fName.find(".xml") != fName.npos ) {
    return map->readXmlFile(fileName);
  }
  return map->readTxtFile(fileName);
}


void AcdCalibBase::writeXmlHeader(DomElement& node) const {
  
  DomElement sourceNode = AcdXmlUtil::makeChildNode(node,"inputSample");  
  AcdXmlUtil::addAttributeMET(sourceNode,"startTime",m_eventStats.time_first());
  AcdXmlUtil::addAttributeMET(sourceNode,"stopTime",m_eventStats.time_last());
  AcdXmlUtil::addAttribute(sourceNode,"triggers",m_eventStats.nUsed());
  AcdXmlUtil::addAttribute(sourceNode,"source","Orbit");
  AcdXmlUtil::addAttribute(sourceNode,"mode","Normal"); 
  writeXmlSources(sourceNode);
}

void AcdCalibBase::writeTxtHeader(std::ostream& os) const {
  os << "#startTime = " << m_eventStats.runId_first() << ':'  << m_eventStats.evtId_first() << endl
     << "#stopTime = " << m_eventStats.runId_last() << ':'  << m_eventStats.evtId_last() << endl
     << "#triggers = " << m_eventStats.nUsed() << '/' << m_eventStats.nFilter() << '/' << m_eventStats.nTrigger() << endl
     << "#source = " << endl
     << "#mode = " << 0 << endl;
  writeTxtSources(os);
}


/// for writing output files
void AcdCalibBase::writeXmlSources( DomElement& node) const {
  writeCalibXml(node,AcdCalibData::PEDESTAL);
  writeChainXml(node,AcdCalib::DIGI);
  writeChainXml(node,AcdCalib::RECON);
  writeChainXml(node,AcdCalib::MERIT);
  writeChainXml(node,AcdCalib::SVAC);
  writeChainXml(node,AcdCalib::BENCH);  
}

void AcdCalibBase::writeTxtSources(std::ostream& os) const {
  writeCalibTxt(os,AcdCalibData::PEDESTAL);
  writeChainTxt(os,AcdCalib::DIGI);
  writeChainTxt(os,AcdCalib::RECON);
  writeChainTxt(os,AcdCalib::MERIT);
  writeChainTxt(os,AcdCalib::SVAC);
  writeChainTxt(os,AcdCalib::BENCH);  
}


// get the maps of the histograms to be fit
AcdHistCalibMap* AcdCalibBase::getHistMap(AcdCalib::HISTTYPE hType) {
  return hType < 0 ? 0 : m_histMaps[hType];
}
const AcdHistCalibMap* AcdCalibBase::getHistMap(AcdCalib::HISTTYPE hType) const {
  return hType < 0 ? 0 : m_histMaps[hType];
}


/// Read the map of the histograms to be fit from a root file
AcdHistCalibMap* AcdCalibBase::readHistMap(AcdCalib::HISTTYPE hType, const char* fileName) {
  TFile* f = TFile::Open(fileName);
  if ( f == 0 ) return 0;
  AcdHistCalibMap* nMap = new AcdHistCalibMap(*f);
  m_histMaps[hType] = nMap;
  return nMap;
}


// get the results maps
AcdCalibMap* AcdCalibBase::getCalibMap(AcdCalibData::CALTYPE cType) {
  return cType < 0 ? 0 : m_fitMaps[cType];
}
const AcdCalibMap* AcdCalibBase::getCalibMap(AcdCalibData::CALTYPE cType) const {
  return cType < 0 ? 0 : m_fitMaps[cType];
}

// get a particular chain
TChain* AcdCalibBase::getChain(AcdCalib::CHAIN chain) {
  return chain < 0 ? 0 : m_chains[chain];
}
const TChain* AcdCalibBase::getChain(AcdCalib::CHAIN chain) const {
  return chain < 0 ? 0 : m_chains[chain];
}


AcdCalibMap* AcdCalibBase::fit(AcdCalibFit& fitter, AcdCalibData::CALTYPE cType, AcdCalib::HISTTYPE hType) { 
  AcdCalibMap* result = getCalibMap(cType);
  if ( result == 0 ) {
    result = new AcdCalibMap(*(fitter.desc()));
    addCalibration(cType,*result);
  }
  AcdHistCalibMap* hists = getHistMap(hType);
  fitter.fitAll(*result,*hists);
  return result;
}

// get the pedestal for a channel
float AcdCalibBase::getPeds(UInt_t key) const {
  const AcdCalibMap* peds = m_fitMaps[AcdCalibData::PEDESTAL];
  if ( peds == 0 ) return -1;
  const CalibData::AcdCalibObj * pedRes = peds->get(key);
  if ( pedRes == 0 ) return -2;
  return (*pedRes)[0];
}

// 
void AcdCalibBase::writeCalibTxt(std::ostream& os, AcdCalibData::CALTYPE cType) const {

  std::string tag;
  switch (cType) {
  case AcdCalibData::PEDESTAL:   tag += "#pedestalFile = "; break;
  case AcdCalibData::GAIN:       tag += "#gainFile = ";     break;
  case AcdCalibData::RANGE:      tag += "#rangeFile = ";    break;
  case AcdCalibData::HIGH_RANGE: tag += "#pedHighFile = ";    break;
  default:
    return;
  }
  const AcdCalibMap* calib = getCalibMap(cType);
  if ( calib == 0 ) return;
  os << tag << calib->fileName() << endl;
}

//
void AcdCalibBase::writeChainTxt(std::ostream& os, AcdCalib::CHAIN chain) const {
  std::string tag;
  switch (chain) {
  case AcdCalib::DIGI:  tag += "#inputDigiFile = ";  break;
  case AcdCalib::RECON: tag += "#inputReconFile = "; break;
  case AcdCalib::MERIT: tag += "#inputMeritFile = "; break;
  case AcdCalib::SVAC:  tag += "#inputSvacFile = ";  break;
  case AcdCalib::BENCH: tag += "#inputBenchFile = "; break;
  default:
    return;
  }
  const TChain* tchain = getChain(chain);
  if ( tchain == 0 ) return;
  TObjArray* files = tchain->GetListOfFiles();
  if ( files == 0 ) return;
  for ( Int_t i(0); i < files->GetEntriesFast(); i++ ) {
    TObject* obj = files->At(i);
    os << tag << obj->GetTitle() << endl;
  }
}

// 
void AcdCalibBase::writeCalibXml(DomElement& node, AcdCalibData::CALTYPE cType) const {
  std::string tag;
  switch (cType) {
  case AcdCalibData::PEDESTAL:   tag += "Pedestal";  break;
  case AcdCalibData::GAIN:       tag += "Gain";      break;
  case AcdCalibData::RANGE:      tag += "Range";     break;
  case AcdCalibData::HIGH_RANGE: tag += "Pedestal";  break;
  default:
    return;
  }
  const AcdCalibMap* calib = getCalibMap(cType);
  if ( calib == 0 ) return;
  std::string fname = calib->fileName();
  if ( fname.length() < 2 ) return;
  DomElement cNode = AcdXmlUtil::makeChildNode(node,"inputFile");
  AcdXmlUtil::addAttribute(cNode,"type",tag.c_str());
  AcdXmlUtil::addAttribute(cNode,"path",calib->fileName());
}

//
void AcdCalibBase::writeChainXml(DomElement& node, AcdCalib::CHAIN chain) const {
  std::string tag;
  switch (chain) {
  case AcdCalib::DIGI:  tag += "Digi";  break;
  case AcdCalib::RECON: tag += "Recon"; break;
  case AcdCalib::MERIT: tag += "Merit"; break;
  case AcdCalib::SVAC:  tag += "Svac";  break;
  default:
    return;
  }
  const TChain* tchain = getChain(chain);
  if ( tchain == 0 ) return;
  TObjArray* files = tchain->GetListOfFiles();
  if ( files == 0 ) return;
  for ( Int_t i(0); i < files->GetEntriesFast(); i++ ) {
    TObject* obj = files->At(i);
    DomElement cNode = AcdXmlUtil::makeChildNode(node,"inputFile");
    AcdXmlUtil::addAttribute(cNode,"type",tag.c_str());
    AcdXmlUtil::addAttribute(cNode,"path",obj->GetTitle());
  }
}


// return the total number of events in the chains
int AcdCalibBase::getTotalEvents() const {
  for ( int i(0); i < AcdCalib::NCHAIN; i++ ) {
    TChain* chain = m_chains[i];
    if ( chain != 0 ) {
      return (int)chain->GetEntries();
    }
  }
  return -1;
}

