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
#include "CalibData/Acd/AcdHighPed.h"
#include "CalibData/Acd/AcdPed.h"
#include "CalibData/Acd/AcdCarbon.h"
#include "CalibData/Acd/AcdGain.h"
#include "CalibData/Acd/AcdRange.h"
#include "CalibData/Acd/AcdHighRange.h"

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


AcdCalibBase::AcdCalibBase(AcdCalibData::CALTYPE t, AcdKey::Config config)
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

void AcdCalibBase::go_list(std::vector<int> EvtRecon ) {
//  ofstream outfile;
//  outfile.open("out.txt");

  int nTotal = EvtRecon.size();
  int last = EvtRecon.back();
  
  m_eventStats.setRange(EvtRecon.front(),last);
  cout << "Number of events in the chain: " << nTotal << endl;
  cout << "Number of events used: " << EvtRecon.size() << endl;
  cout << "Starting at event: " << EvtRecon.front() << endl;

  for (vector<int>::iterator nRecon = EvtRecon.begin() ; nRecon != EvtRecon.end(); ++nRecon){
    Bool_t filtered(kFALSE);
    Int_t runId, evtId;
    Double_t timeStamp;
    Int_t ievent = *nRecon;

    Bool_t ok = readEvent(ievent,filtered, runId, evtId,timeStamp);

    if ( !ok ) {
      cout << "Failed to read event " << ievent << " aborting" << endl;
      break;
    }

    Bool_t used(kFALSE);
    if ( !filtered ) {
//      std::cout << "Event " << ievent << std::endl;
//      useEvent(used,outfile,ievent);
      useEvent(used);
    }
    m_eventStats.logEvent(ievent,used,filtered,runId,evtId,timeStamp);

  }

  m_eventStats.lastEvent();

  cout << endl;

}

void AcdCalibBase::go(int numEvents, int startEvent) {

//  ofstream outfile;
//  outfile.open("out.txt");

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
//      std::cout << "Event " << ievent << std::endl;
//      useEvent(used,outfile,ievent);
      useEvent(used);
    }
    
    m_eventStats.logEvent(ievent,used,filtered,runId,evtId,timeStamp);
  }

  m_eventStats.lastEvent();

  cout << endl;

}

void AcdCalibBase::fillHist(AcdHistCalibMap& histMap, int id, int pmtId, float val, UInt_t idx)
{
  UInt_t histId = AcdKey::makeKey(pmtId,id);
  
  if ( ! AcdKey::channelExists(id) ) {
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
  UInt_t histId = AcdKey::makeKey(pmtId,id);
  
  if ( ! AcdKey::channelExists(id) ) {
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
  case AcdCalib::H_RANGE: name += "RANGE"; break;
  case AcdCalib::H_CNO: name += "CNO"; break;  
  case AcdCalib::H_HIGH_RANGE: name += "HIGH_RANGE"; break;    
  case AcdCalib::H_COHERENT_NOISE: name += "COHERENT_NOISE"; break;
  case AcdCalib::H_RIBBONS: name += "RIBBONS"; break;
  case AcdCalib::H_CARBON: name += "CARBON"; break;
  case AcdCalib::H_VETO_FIT: name += "VETO_FIT"; break;  
  case AcdCalib::H_TREND: name += "TREND"; break;  
  case AcdCalib::H_NONE:
  default:
    return 0;
  }

  map = new AcdHistCalibMap(name,nBin,low,hi,m_config,nHist);
  m_histMaps[histType] = map;
  return map;
} 

void AcdCalibBase::giveInfoToCalib(AcdCalibMap& theMap) {
  theMap.latchStats( m_eventStats.time_first(), m_eventStats.time_last(), m_eventStats.nUsed() );
  for ( int iCalib = AcdCalibData::PEDESTAL; iCalib < AcdCalibData::NDESC; iCalib++ ) {
    AcdCalibMap* inputMap = m_fitMaps[iCalib];
    if ( inputMap == 0 ) continue;    
    std::string path = inputMap->fileName();
    if ( path.size() < 1 ) continue;
    std::string type;
    AcdXmlUtil::getCalibTypeName(type,iCalib);
    type.erase(0,4);
    theMap.addInput(path,type); 
  }
  for ( int iChain = AcdCalib::DIGI; iChain < AcdCalib::NCHAIN; iChain++ ) {
    TChain* inputChain = m_chains[iChain];
    if ( inputChain == 0 ) continue;
    TObjArray* files = inputChain->GetListOfFiles();
    if ( files == 0 ) return;
    std::string type;
    AcdXmlUtil::getEventFileType(type,iChain);
    for ( Int_t i(0); i < files->GetEntriesFast(); i++ ) {
      TObject* obj = files->At(i);
      std::string path = obj->GetTitle();
      theMap.addInput(path,type); 
    }
  }  
}


void AcdCalibBase::addCalibration(AcdCalibData::CALTYPE calibKey, AcdCalibMap& newCal) {  
  AcdCalibMap* old = getCalibMap(calibKey);
  if ( old != 0 ) {
    std::cout << "Warning: replacing calibration" << std::endl;
    delete old;
  }
  m_fitMaps[calibKey] = &newCal;
}

Bool_t AcdCalibBase::readCalib(AcdCalibData::CALTYPE calKey, const char* fileName) {
  AcdCalibMap* map = getCalibMap(calKey);
  if ( map != 0 ) {
    std::cout << "Warning: replacing old calibration" << std::endl;
    delete map;
  }
  
  const CalibData::AcdCalibDescription* desc = CalibData::AcdCalibDescription::getDesc(calKey);
  if ( desc == 0 ) {
    std::cerr << "No description for calibration type " << calKey << std::endl;
    return kFALSE;
  }
  map = new AcdCalibMap(*desc);

  if ( map == 0 ) return kFALSE;
  addCalibration(calKey,*map);

  std::string fName(fileName); 
  if ( fName.find(".xml") != fName.npos ) {
    return map->readXmlFile(fileName);
  } else if ( fName.find(".txt") != fName.npos ) {
    return map->readTxtFile(fileName);
  } else {
    std::cerr << "File " << fName << " is not xml or txt file for calibration type " << desc->calibTypeName() << std::endl;
  }
  return kFALSE;
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


AcdCalibMap* AcdCalibBase::fit(AcdCalibFit& fitter, AcdCalibData::CALTYPE cType, AcdCalib::HISTTYPE hType, 
			       const char* referenceFile, AcdKey::ChannelSet cSet) {   

  AcdHistCalibMap* hists = getHistMap(hType);
  if ( hists == 0 ) return 0;

  AcdCalibMap* result = getCalibMap(cType);
  if ( result == 0 ) {
    result = new AcdCalibMap(*(fitter.desc()));
    addCalibration(cType,*result);
  }
  AcdCalibMap* ref(0);
  if ( referenceFile != 0 && std::string(referenceFile).size() > 1 ) {       
    ref = new AcdCalibMap(*(fitter.desc()));
    if ( ! ref->readXmlFile(referenceFile) ) {
      std::cerr << "Failed to read reference file " << referenceFile  << std::endl;
      return 0;
    }
    if ( ! ref->readTree() ) {
      std::cerr << "Failed to read reference results from TTree " << referenceFile << std::endl;
      return 0;
    }
    result->setReference(*ref);
  }
  fitter.fitAll(*result,*hists,cSet);
  result->setHists(*hists);
  giveInfoToCalib(*result);
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

