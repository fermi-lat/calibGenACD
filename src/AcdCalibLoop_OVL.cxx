
#include "AcdCalibLoop_OVL.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TChain.h"

#include "AcdHistCalibMap.h"
#include "AcdCalibMap.h"
#include "AcdCalibUtil.h"

#include "digiRootData/AcdDigi.h"
#include "overlayRootData/EventOverlay.h"
#include "overlayRootData/AcdOverlay.h"
#include "CalibData/Acd/AcdCalibObj.h"
#include <TMath.h>

#include <cassert>
#include <cmath>
#include <fstream>

using std::cout;
using std::cerr;
using std::endl;
using std::string;


AcdCalibLoop_OVL::AcdCalibLoop_OVL(AcdCalibData::CALTYPE t, TChain *ovlChain, 
				   AcdKey::Config config, 
				   int gemDeltaEventTimeCut,
				   bool requireBothPmtsInTiles,
				   bool requireSinglesInTiles)
  :AcdCalibBase(t,config), 
   m_eventOverlay(0),
   m_phaHists(0),
   m_MeVHists(0),
   m_deltaEventTimeCut(gemDeltaEventTimeCut),
   m_requireBothPmtsInTiles(requireBothPmtsInTiles),
   m_requireSinglesInTiles(requireSinglesInTiles){

  setChain(AcdCalib::OVERLAY,ovlChain);

  switch ( t ) {
  case AcdCalibData::PEDESTAL:
    m_phaHists = bookHists(AcdCalib::H_RAW,4096,-0.5,4095.5);
    m_MeVHists = bookHists(AcdCalib::H_GAIN,200,0.,1.);
  default:
    break;
  }

  Bool_t ok = attachChains();
  if ( ! ok ) {
    cerr << "ERR:  Failed to attach to input chains."  << endl;
  }
}


AcdCalibLoop_OVL::~AcdCalibLoop_OVL() 
{
  if (m_eventOverlay) delete m_eventOverlay;	
}

Bool_t AcdCalibLoop_OVL::attachChains() {
  TChain* ovlChain = getChain(AcdCalib::OVERLAY);
  if ( ovlChain!= 0) {
    m_eventOverlay = 0;
    ovlChain->SetBranchAddress("EventOverlay", &m_eventOverlay);
    ovlChain->SetBranchStatus("*",0);  // disable all branches
    // activate desired brances
    ovlChain->SetBranchStatus("m_acdOverlayCol",1);
    ovlChain->SetBranchStatus("m_timeStamp", 1); 
    ovlChain->SetBranchStatus("m_eventId", 1);
    ovlChain->SetBranchStatus("m_runId", 1);
    ovlChain->SetBranchStatus("m_gemOverlay", 1);    
  }
  return kTRUE;
}

Bool_t AcdCalibLoop_OVL::readEvent(int ievent, Bool_t& filtered, 
				  int& runId, int& evtId, Double_t& timeStamp) {
  
  if(m_eventOverlay) m_eventOverlay->Clear();

  filtered = kFALSE;
  TChain* ovlChain = getChain(AcdCalib::OVERLAY);
  if(ovlChain) { 
    ovlChain->GetEvent(ievent);
    evtId = m_eventOverlay->getEventId(); 
    runId = m_eventOverlay->getRunId();
    timeStamp = m_eventOverlay->getTimeStamp();
    switch ( calType () ) {
    case AcdCalibData::PEDESTAL:
      if ( m_eventOverlay->getGemOverlay().getDeltaEventTime() < m_deltaEventTimeCut ) filtered = kTRUE;
      break;
    default:
      break;
    } 
  }
  
  return kTRUE;
}


void AcdCalibLoop_OVL::useEvent(Bool_t& used) {

  used = kFALSE;
  const TObjArray* acdOvlCol = m_eventOverlay->getAcdOverlayCol();
  if (!acdOvlCol) return;

  int nAcdOvl = acdOvlCol->GetLast() + 1;

  static const float MeVMipTile10 = 1.9;
  static const float MeVMipTile12 = 2.28;
  static const float MeVMipRibbon = 0.5;  

  float MeVMip(-1.);

  static int count(0);

  for(int i = 0; i != nAcdOvl; ++i) {

    AcdOverlay* acdOvl = static_cast<AcdOverlay*>(acdOvlCol->At(i));
    assert(acdOvl != 0);

    const AcdId& acdId = acdOvl->getAcdId();
    int id = acdId.getId();

    if ( ! AcdKey::channelExists( id ) ) continue;
    
    if ( acdId.isTile() ) {
      MeVMip = ( acdId.getFace() == 0 && acdId.getRow() == 2 ) ? MeVMipTile12 : MeVMipTile10;
    } else if ( acdId.isRibbon() ) {
      MeVMip = MeVMipRibbon;
    }

    UInt_t keyA = AcdKey::makeKey(AcdDigi::A,id);
    UInt_t keyB = AcdKey::makeKey(AcdDigi::B,id);

    double energyDep = acdOvl->getEnergyDep();

    const AcdCalibMap* gainCalibs = getCalibMap(AcdCalibData::GAIN);
    
    const CalibData::AcdCalibObj* calibA = gainCalibs->get(keyA);
    const CalibData::AcdCalibObj* calibB = gainCalibs->get(keyB);

    double gainA = calibA->operator[](0);
    double gainB = calibB->operator[](0);
    
    bool hasHitPMT_A = (acdOvl->getStatus() & 0x00000001 ) != 0;
    bool hasHitPMT_B = (acdOvl->getStatus() & 0x00010000 ) != 0;
    
    if ( m_requireBothPmtsInTiles && (! ( hasHitPMT_A && hasHitPMT_B ) ) ) continue;
    if ( m_requireSinglesInTiles && ( hasHitPMT_A  && hasHitPMT_B ) ) continue;

    double energy_A = energyDep;
    double energy_B = energyDep;

    if ( hasHitPMT_A && ( ! hasHitPMT_B ) ) {
      energy_A += energy_B;
      energy_B = 0.;
    }
    if ( hasHitPMT_B && ( ! hasHitPMT_A ) ) {
      energy_B += energy_A;
      energy_A = 0.;
    }

    double phaA = energy_A * gainA / MeVMip;
    double phaB = energy_B * gainB / MeVMip;

    switch ( calType () ) {
    case AcdCalibData::PEDESTAL:
      if (hasHitPMT_A) fillHist(*m_phaHists, id, AcdDigi::A, phaA);
      if (hasHitPMT_B) fillHist(*m_phaHists, id, AcdDigi::B, phaB);
      if (hasHitPMT_A) fillHist(*m_MeVHists, id, AcdDigi::A, energy_A);
      if (hasHitPMT_B) fillHist(*m_MeVHists, id, AcdDigi::B, energy_B);      
      break;
    default:
      break;
    }
    used = kTRUE;
  }

}



