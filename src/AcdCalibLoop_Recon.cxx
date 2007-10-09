#include "AcdCalibLoop_Recon.h"

#include "AcdHistCalibMap.h"
#include "AcdCalibUtil.h"
#include "AcdCalibMap.h"

#include "digiRootData/DigiEvent.h"
#include "reconRootData/ReconEvent.h"

#include "TH1F.h"
#include "TChain.h"

#include <cassert>
#include <cmath>

AcdCalibLoop_Recon::AcdCalibLoop_Recon(TChain* digiChain, TChain *reconChain, Bool_t correctPathLength, AcdMap::Config config)
  :AcdCalibBase(AcdCalibData::GAIN,config),
   m_correctPathLength(correctPathLength),
   m_digiEvent(0),
   m_reconEvent(0),
   m_gainHists(0){

  setChain(AcdCalib::DIGI,digiChain);
  setChain(AcdCalib::RECON,reconChain);

  m_gainHists = bookHists(AcdCalib::H_GAIN,64,-0.5,4095.5);
  

  Bool_t ok = attachChains();
  if ( ! ok ) {
    cerr << "ERR:  Failed to attach to input chains."  << endl;
  }
}


AcdCalibLoop_Recon::~AcdCalibLoop_Recon() 
{
  if (m_digiEvent) delete m_digiEvent;
  if (m_reconEvent) delete m_reconEvent;	
}

Bool_t AcdCalibLoop_Recon::attachChains() {

  TChain* digiChain = getChain(AcdCalib::DIGI);
  if (digiChain != 0) {
    m_digiEvent = 0;
    digiChain->SetBranchAddress("DigiEvent", &m_digiEvent);
    digiChain->SetBranchStatus("*",0);  // disable all branches
    // activate desired brances
    digiChain->SetBranchStatus("m_acd*",1);
    digiChain->SetBranchStatus("m_eventId", 1); 
    digiChain->SetBranchStatus("m_runId", 1);
  }
  
  TChain* reconChain = getChain(AcdCalib::RECON); 
  if (reconChain != 0) {
    m_reconEvent = 0;
    reconChain->SetBranchAddress("ReconEvent", &m_reconEvent);
    reconChain->SetBranchStatus("*",0);  // disable all branches
    // activate desired brances
    reconChain->SetBranchStatus("m_acd",1);
    reconChain->SetBranchStatus("m_eventId", 1); 
    reconChain->SetBranchStatus("m_runId", 1);
  }
    
  return kTRUE;
}

void AcdCalibLoop_Recon::fillGainHistCorrect(const AcdTkrIntersection& inter, const AcdDigi& digi) {

  int rng0 = digi.getRange(AcdDigi::A);    
  int pmt0 = digi.getPulseHeight(AcdDigi::A);
  
  int rng1 = digi.getRange(AcdDigi::B);
  int pmt1 = digi.getPulseHeight(AcdDigi::B);

  int id = digi.getId().getId();
  
  float width = AcdCalibUtil::width(id);
  float pathFactor = inter.getPathLengthInTile() / width;
  
  // require that track not clip edge of tile
  if ( id < 500 && pathFactor < 0.99 ) return;
  if ( m_correctPathLength ) {
    // correcting for pathlength, go ahead and take stuff up to 1/cos_th = 1.5
    if ( pathFactor > 1.5 ) return;
  } else {
    // not correcting for pathlength, only take stuff up to 1/cos_th = 1.2
    if ( pathFactor > 1.2 ) return;
  }  

  UInt_t keyA = AcdMap::makeKey(AcdDigi::A,id);
  UInt_t keyB = AcdMap::makeKey(AcdDigi::B,id);

  float pedA = getPeds(keyA);
  float pedB = getPeds(keyB);
  if ( pedA < 0 || pedB < 0 ) return;

  Float_t redPha_A = ((Float_t)(pmt0)) - pedA;
  Float_t redPha_B = ((Float_t)(pmt1)) - pedB;

  if ( m_correctPathLength ) {
    redPha_A /= pathFactor;
    redPha_B /= pathFactor;
  }

  ///Float_t redPha_A = ((Float_t)(pmt0));
  // Float_t redPha_B = ((Float_t)(pmt1));

  if ( rng0 == 0 ) fillHist(*m_gainHists,id,AcdDigi::A,redPha_A);
  if ( rng1 == 0 ) fillHist(*m_gainHists,id,AcdDigi::B,redPha_B);

}


Bool_t AcdCalibLoop_Recon::readEvent(int ievent, Bool_t& filtered, 
				  int& runId, int& evtId) {
  
  if(m_digiEvent) m_digiEvent->Clear();
  if(m_reconEvent) m_reconEvent->Clear();

  TChain* digiChain = getChain(AcdCalib::DIGI);
  TChain* reconChain = getChain(AcdCalib::RECON);
  
  if(digiChain) { 
    digiChain->GetEvent(ievent);
    evtId = m_digiEvent->getEventId(); 
    runId = m_digiEvent->getRunId();
  }
  if(reconChain) reconChain->GetEvent(ievent);
  
  if(m_digiEvent && m_reconEvent) {
    int reconEventId = m_reconEvent->getEventId(); 
    int reconRunNum = m_reconEvent->getRunId();
    assert ( evtId == reconEventId );
    assert ( runId == reconRunNum );
  }

  filtered = kFALSE;
  
  return kTRUE;
}


void AcdCalibLoop_Recon::useEvent(Bool_t& used) {

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
    if (!( acdId.isTile() || acdId.isRibbon()) ) continue;
    unsigned id = acdId.getId();

    int nAcdDigi = acdDigiCol->GetLast() + 1;
    for(int j = 0; j != nAcdDigi; j++) {      
      const AcdDigi* acdDigi = static_cast<const AcdDigi*>(acdDigiCol->At(j));      
      assert(acdDigi != 0);      
      const AcdId& acdIdCheck = acdDigi->getId();
      if ( acdIdCheck.getId() != id ) continue;
      if (!( acdId.isTile() || acdId.isRibbon()) ) continue;
      fillGainHistCorrect(*acdInter,*acdDigi);
      used = kTRUE;
    }
  }
}

