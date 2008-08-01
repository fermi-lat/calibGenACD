
#include "AcdCalibLoop_Digi.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TChain.h"

#include "AcdHistCalibMap.h"
#include "AcdCalibMap.h"
#include "AcdCalibUtil.h"
#include "AcdGarcGafeHits.h"

#include "digiRootData/DigiEvent.h"
#include "CalibData/Acd/AcdCalibObj.h"
#include <TMath.h>

#include <cassert>
#include <cmath>
#include <fstream>

using std::cout;
using std::cerr;
using std::endl;
using std::string;


AcdCalibLoop_Digi::AcdCalibLoop_Digi(AcdCalibData::CALTYPE t, TChain *digiChain, Bool_t requirePeriodic, AcdKey::Config config)
  :AcdCalibBase(t,config), 
   m_digiEvent(0),
   m_requirePeriodic(requirePeriodic),
   m_rawHists(0),
   m_threshHists(0),
   m_rangeHists(0){

  setChain(AcdCalib::DIGI,digiChain);

  switch ( t ) {
  case AcdCalibData::PEDESTAL:
  case AcdCalibData::PED_HIGH:
    m_rawHists = bookHists(AcdCalib::H_RAW,4096,-0.5,4095.5);
    break;
  case AcdCalibData::VETO:
    m_threshHists = bookHists(AcdCalib::H_VETO,256,-0.5,4095.5,3);
    break;
  case AcdCalibData::RANGE:
    m_rangeHists = bookHists(AcdCalib::H_RANGE,4096,-0.5,4095.5,2);
    break;
  case AcdCalibData::CNO:
    m_threshHists = bookHists(AcdCalib::H_CNO,64,-0.5,255.5,3);
    break;
  default:
    break;
  }

  Bool_t ok = attachChains();
  if ( ! ok ) {
    cerr << "ERR:  Failed to attach to input chains."  << endl;
  }
}


AcdCalibLoop_Digi::~AcdCalibLoop_Digi() 
{
  if (m_digiEvent) delete m_digiEvent;	
}

Bool_t AcdCalibLoop_Digi::attachChains() {
  TChain* digiChain = getChain(AcdCalib::DIGI);
  if (digiChain != 0) {
    m_digiEvent = 0;
    digiChain->SetBranchAddress("DigiEvent", &m_digiEvent);
    digiChain->SetBranchStatus("*",0);  // disable all branches
    // activate desired brances
    digiChain->SetBranchStatus("m_acd*",1);
    digiChain->SetBranchStatus("m_timeStamp", 1); 
    digiChain->SetBranchStatus("m_eventId", 1);
    digiChain->SetBranchStatus("m_runId", 1);
    //if ( m_requirePeriodic ) {
    digiChain->SetBranchStatus("m_gem", 1);
    //}
  }
  return kTRUE;
}

Bool_t AcdCalibLoop_Digi::readEvent(int ievent, Bool_t& filtered, 
				  int& runId, int& evtId, Double_t& timeStamp) {
  
  if(m_digiEvent) m_digiEvent->Clear();

  filtered = kFALSE;
  TChain* digiChain = getChain(AcdCalib::DIGI);
  if(digiChain) { 
    digiChain->GetEvent(ievent);
    evtId = m_digiEvent->getEventId(); 
    runId = m_digiEvent->getRunId();
    timeStamp = m_digiEvent->getTimeStamp();
    switch ( calType () ) {
    case AcdCalibData::PEDESTAL:
    case AcdCalibData::PED_HIGH:
      if ( m_requirePeriodic &&
	   (m_digiEvent->getGem().getConditionSummary() != 32 &&  
	    m_digiEvent->getGem().getConditionSummary() != 128 ) )
	filtered = kTRUE;
      break;
    default:
      break;
    } 
  }
  
  return kTRUE;
}


void AcdCalibLoop_Digi::useEvent(Bool_t& used) {


  used = kFALSE;
  const TObjArray* acdDigiCol = m_digiEvent->getAcdDigiCol();
  if (!acdDigiCol) return;

  if ( calType() == AcdCalibData::CNO ) {  
    m_garcGafeHits.reset();
    m_garcGafeHits.setCNO( m_digiEvent->getGem().getCnoVector() );
  }

  int nAcdDigi = acdDigiCol->GetLast() + 1;
  for(int i = 0; i != nAcdDigi; ++i) {

    AcdDigi* acdDigi = static_cast<AcdDigi*>(acdDigiCol->At(i));
    assert(acdDigi != 0);

    const AcdId& acdId = acdDigi->getId();
    int id = acdId.getId();

    if ( ! AcdKey::channelExists( id ) ) continue;

    // NOT CNO calibaration
    int rng0 = acdDigi->getRange(AcdDigi::A);    
    float pmt0 = (float)acdDigi->getPulseHeight(AcdDigi::A);

    int rng1 = acdDigi->getRange(AcdDigi::B);
    float pmt1 = (float)acdDigi->getPulseHeight(AcdDigi::B);

    Bool_t useA = rng0 == 0 && pmt0 > 0;
    Bool_t useB = rng1 == 0 && pmt1 > 0;

    UInt_t keyA = AcdKey::makeKey(AcdDigi::A,id);
    UInt_t keyB = AcdKey::makeKey(AcdDigi::B,id);

    Float_t pedA(0.);    
    Float_t pedB(0.);
    Float_t pedHighA(0.);
    Float_t pedHighB(0.);
    
    const AcdCalibMap* calib(0);
    const CalibData::AcdCalibObj* calibA(0);
    const CalibData::AcdCalibObj* calibB(0);
    switch ( calType() ){
    case AcdCalibData::CNO:
    case AcdCalibData::RANGE:
      calib = getCalibMap(AcdCalibData::PED_HIGH);
      if ( calib == 0 ) return;
      calibA = calib->get(keyA);
      calibB = calib->get(keyB);
      if ( calibA == 0 || calibB == 0 ) return;
      pedHighA = calibA->operator[](0);
      pedHighB = calibB->operator[](0);
      break;
    case AcdCalibData::VETO:
      pedA = getPeds(keyA);
      pedB = getPeds(keyB);
      if ( pedA < 0 || pedB < 0 ) return;
      pmt0 -= pedA;
      pmt1 -= pedB;
      break;
    default:
      break;
    }

    if ( calType() == AcdCalibData::CNO ) {
      if ( rng0 == 0 ) {
	pmt0 = -1;
      } else {      
	pmt0 -=  pedHighA;
      }
      if ( rng1 == 0 ) {
	pmt1 = -1;
      } else {      
	pmt1 -=  pedHighB;
      }
      m_garcGafeHits.setDigi(*acdDigi,pmt0,pmt1);
    } else if ( calType() == AcdCalibData::VETO ) {
      useA &= pmt0 > 25.;
      useB &= pmt1 > 25.;
    } else if ( calType() == AcdCalibData::RANGE ) {
      useA = rng0 == 0 || ( pmt0 > pedHighA );
      useB = rng1 == 0 || ( pmt1 > pedHighB );
    }

    switch ( calType () ) {
    case AcdCalibData::PEDESTAL:
      if ( useA ) fillHist(*m_rawHists, id, AcdDigi::A, pmt0);
      if ( useB ) fillHist(*m_rawHists, id, AcdDigi::B, pmt1);
      break;
    case AcdCalibData::PED_HIGH:
      fillHist(*m_rawHists, id, AcdDigi::A, pmt0);
      fillHist(*m_rawHists, id, AcdDigi::B, pmt1);
      break;
    case AcdCalibData::RANGE:
      if ( useA ) {
	if ( rng0 == 0 ) {
	  fillHist(*m_rangeHists, id, AcdDigi::A, pmt0, 0);
	} else {
	  fillHist(*m_rangeHists, id, AcdDigi::A, pmt0, 1);
	}
      }
      if ( useB ) {
	if ( rng1 == 0 ) {
	  fillHist(*m_rangeHists, id, AcdDigi::B, pmt1, 0);
	} else {
	  fillHist(*m_rangeHists, id, AcdDigi::B, pmt1, 1);
	}
      }
      break;
    case AcdCalibData::VETO:
      if ( useA ) {
	fillHist(*m_threshHists, id, AcdDigi::A, pmt0);
	if ( acdDigi->getVeto(AcdDigi::A) ) fillHist(*m_threshHists, id, AcdDigi::A, pmt0, 1);
      }
      if ( useB ) {  
	fillHist(*m_threshHists, id, AcdDigi::B, pmt1);
	if ( acdDigi->getVeto(AcdDigi::B) ) fillHist(*m_threshHists, id, AcdDigi::B, pmt1, 1);
      }
      break;
    default:
      break;
    }
    used = kTRUE;
  }

  if ( calType() == AcdCalibData::CNO ) {  
    fillCnoData();
  }
}


AcdHistCalibMap* AcdCalibLoop_Digi::makeRatioPlots() {
  for ( UInt_t iPmt(0); iPmt < AcdKey::nPmt; iPmt++ ) {    
    for ( UInt_t iFace(0); iFace < AcdKey::nFace; iFace++ ) {
      UInt_t nRow = AcdKey::getNRow(iFace);
      for ( UInt_t iRow(0); iRow < nRow; iRow++ ) {
	UInt_t nCol = AcdKey::getNCol(iFace,iRow);
	for ( UInt_t iCol(0); iCol < nCol; iCol++ ) {
	  UInt_t key = AcdKey::makeKey(iPmt,iFace,iRow,iCol);
	  TH1* raw = m_threshHists->getHist(key,0);
	  TH1* veto = m_threshHists->getHist(key,1);
	  TH1* vf = m_threshHists->getHist(key,2);
	  if ( raw == 0 || veto == 0 || vf == 0 ) {
	    std::cout << "missing one " << key << std::endl;
	    continue;
	  }
	  Int_t nVeto = (Int_t)veto->GetEntries();
	  if ( nVeto > 100 && veto->GetMaximum() > 10 ) {	  
	    AcdCalibUtil::efficDivide(*vf,*veto,*raw,kFALSE,10.);
	  }
	}
      }
    }
  }
  return m_threshHists;
}

void AcdCalibLoop_Digi::fillCnoData() {
  for ( UInt_t iGarc(0); iGarc < 12; iGarc++ ) {
    Bool_t cno(kFALSE); 
    UInt_t nHits(0), nVeto(0);
    m_garcGafeHits.garcStatus(iGarc,cno,nHits,nVeto);
    //if ( nVeto != 1  ) continue;
    Int_t gafe(-1);
    while ( m_garcGafeHits.nextGarcVeto(iGarc,gafe) ) {
      UInt_t uGafe = (UInt_t)gafe;
      UShort_t inPha = m_garcGafeHits.inPha(iGarc,uGafe);
      UShort_t flags = m_garcGafeHits.flags(iGarc,uGafe);
      if ( (flags & 0x4) != 0x4 ) continue; // low range
      UInt_t tile(0); UInt_t pmt(0);
      AcdGarcGafeHits::convertToTilePmt(iGarc,uGafe,tile,pmt);
      //UInt_t key = AcdKey::makeKey(pmt,tile);
      fillHist(*m_threshHists,tile,pmt,inPha);
      if ( cno ) {
	fillHist(*m_threshHists,tile,pmt,inPha,1);
      }
    }
  }
}


