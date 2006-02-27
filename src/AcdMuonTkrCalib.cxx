#include <fstream>
#include "TH1F.h"
#include "TF1.h"
#include "AcdMuonTkrCalib.h"

#include "AcdHistCalibMap.h"
#include "AcdPedestalFit.h"
#include "AcdGainFit.h"
#include "AcdCalibUtil.h"

#include "digiRootData/DigiEvent.h"
#include "reconRootData/ReconEvent.h"

#include <cassert>
#include <cmath>

using std::cout;
using std::cerr;
using std::endl;
using std::string;

ClassImp(AcdMuonTkrCalib) ;

AcdMuonTkrCalib::AcdMuonTkrCalib(TChain* digiChain, TChain *reconChain)
  :AcdCalibBase(), 
   m_digiChain(digiChain),
   m_reconChain(reconChain), 
   m_digiEvent(0),
   m_reconEvent(0),
   m_gainHists(0),
   m_gains(0),
   m_peds(0){

  m_gainHists = bookHists(GAIN,256,-0.5,4095.5);
  
  Bool_t ok = attachChains();
  if ( ! ok ) {
    cerr << "ERR:  Failed to attach to input chains."  << endl;
  }
}


AcdMuonTkrCalib::~AcdMuonTkrCalib() 
{
  if (m_digiEvent) delete m_digiEvent;
  if (m_reconEvent) delete m_reconEvent;	
}

Bool_t AcdMuonTkrCalib::attachChains() {
  if (m_digiChain != 0) {
    m_digiEvent = 0;
    m_digiChain->SetBranchAddress("DigiEvent", &m_digiEvent);
    m_digiChain->SetBranchStatus("*",0);  // disable all branches
    // activate desired brances
    m_digiChain->SetBranchStatus("m_acd*",1);
    m_digiChain->SetBranchStatus("m_eventId", 1); 
    m_digiChain->SetBranchStatus("m_runId", 1);
  }
  
  if (m_reconChain != 0) {
    m_reconEvent = 0;
    m_reconChain->SetBranchAddress("ReconEvent", &m_reconEvent);
    m_reconChain->SetBranchStatus("*",0);  // disable all branches
    // activate desired brances
    m_reconChain->SetBranchStatus("m_acd",1);
    m_reconChain->SetBranchStatus("m_eventId", 1); 
    m_reconChain->SetBranchStatus("m_runId", 1);
  }
    
  return kTRUE;
}

void AcdMuonTkrCalib::fillGainHistCorrect(const AcdTkrIntersection& inter, const AcdDigi& digi) {

  int rng0 = digi.getRange(AcdDigi::A);    
  int pmt0 = digi.getPulseHeight(AcdDigi::A);
  
  int rng1 = digi.getRange(AcdDigi::B);
  int pmt1 = digi.getPulseHeight(AcdDigi::B);

  int id = digi.getId().getId();
  
  float width = AcdCalibUtil::width(id);
  float pathFactor = inter.getPathLengthInTile() / width;
  
  // if ( pathFactor < 0.99 || pathFactor > 1.2 ) return;
  if ( pathFactor > 1.5 ) return;

  UInt_t keyA = AcdMap::makeKey(AcdDigi::A,id);
  UInt_t keyB = AcdMap::makeKey(AcdDigi::B,id);

  AcdPedestalFitResult* pedRes_A = m_peds->find(keyA);
  AcdPedestalFitResult* pedRes_B = m_peds->find(keyB);

  if ( pedRes_A == 0 && pedRes_B == 0 ) return;

  Float_t redPha_A = ((Float_t)(pmt0)) - pedRes_A->mean();
  Float_t redPha_B = ((Float_t)(pmt1)) - pedRes_B->mean();
  
  redPha_A /= pathFactor;
  redPha_B /= pathFactor;

  ///Float_t redPha_A = ((Float_t)(pmt0));
  // Float_t redPha_B = ((Float_t)(pmt1));

  if ( rng0 == 0 ) fillHist(*m_gainHists,id,AcdDigi::A,redPha_A);
  if ( rng1 == 0 ) fillHist(*m_gainHists,id,AcdDigi::B,redPha_B);

}


Bool_t AcdMuonTkrCalib::readEvent(int ievent, Bool_t& filtered, 
				  int& runId, int& evtId) {
  
  if(m_digiEvent) m_digiEvent->Clear();
  if(m_reconEvent) m_reconEvent->Clear();
  
  if(m_digiChain) { 
    m_digiChain->GetEvent(ievent);
    evtId = m_digiEvent->getEventId(); 
    runId = m_digiEvent->getRunId();
  }
  if(m_reconChain) m_reconChain->GetEvent(ievent);
  
  if(m_digiEvent && m_reconEvent) {
    int reconEventId = m_reconEvent->getEventId(); 
    int reconRunNum = m_reconEvent->getRunId();
    assert ( evtId == reconEventId );
    assert ( runId == reconRunNum );
  }

  filtered = kFALSE;
  
  return kTRUE;
}


void AcdMuonTkrCalib::useEvent(Bool_t& used) {

  used = kFALSE;
  const AcdRecon* acdRecon= m_reconEvent->getAcdRecon();
  if (!acdRecon) return;
  
  const TObjArray* acdDigiCol = m_digiEvent->getAcdDigiCol();
  if (!acdDigiCol) return;

  int nAcdInter = acdRecon->nAcdIntersections();
  for(int i = 0; i != nAcdInter; i++) {

    const AcdTkrIntersection* acdInter = acdRecon->getAcdTkrIntersection(i);
    if ( acdInter->getTrackIndex() > 0 ) continue;

    assert(acdInter != 0);

    const AcdId& acdId = acdInter->getTileId();
    unsigned id = acdId.getId();

    int nAcdDigi = acdDigiCol->GetLast() + 1;
    for(int j = 0; j != nAcdDigi; j++) {      
      const AcdDigi* acdDigi = static_cast<const AcdDigi*>(acdDigiCol->At(j));      
      assert(acdDigi != 0);      
      const AcdId& acdIdCheck = acdDigi->getId();
      if ( acdIdCheck.getId() != id ) continue;
      fillGainHistCorrect(*acdInter,*acdDigi);
      used = kTRUE;
    }
  }
}

AcdGainFitMap* AcdMuonTkrCalib::fitGains(AcdGainFit& fitter) {
  m_gains = new AcdGainFitMap;
  addCalibration(GAIN,*m_gains);
  AcdHistCalibMap* hists = getHistMap(GAIN);
  fitter.fitAll(*m_gains,*hists);
  return m_gains;
}  

Bool_t AcdMuonTkrCalib::readPedestals(const char* fileName) {
  Bool_t latchVal = readCalib(PEDESTAL,fileName);
  AcdCalibMap* map = getCalibMap(PEDESTAL);
  m_peds = (AcdPedestalFitMap*)(map);
  return latchVal;
}


/// for writing output files
void AcdMuonTkrCalib::writeXmlHeader(ostream& os) const {
  AcdCalibBase::writeXmlHeader(os);
  std::string pedFileName;
  if ( m_peds != 0 ) pedFileName +=  m_peds->fileName();
  os << "  <pedestalFile value=\"" << pedFileName << "\"/>" << endl; 
}

void AcdMuonTkrCalib::writeTxtHeader(ostream& os) const {
  AcdCalibBase::writeTxtHeader(os);
  std::string pedFileName;
  if ( m_peds != 0 ) pedFileName +=  m_peds->fileName();
  os << "#pedestalFile = " << pedFileName << endl;
}

