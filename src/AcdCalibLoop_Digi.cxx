
#include "AcdCalibLoop_Digi.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TChain.h"

#include "AcdHistCalibMap.h"
#include "AcdCalibMap.h"
#include "AcdCalibUtil.h"
#include "AcdGarcGafeHits.h"

#include "digiRootData/DigiEvent.h"

#include <cassert>
#include <cmath>
#include <fstream>

using std::cout;
using std::cerr;
using std::endl;
using std::string;


AcdCalibLoop_Digi::AcdCalibLoop_Digi(AcdCalibData::CALTYPE t, TChain *digiChain, Bool_t requirePeriodic, AcdMap::Config config)
  :AcdCalibBase(t,config), 
   m_digiEvent(0),
   m_requirePeriodic(requirePeriodic){

  setChain(AcdCalib::DIGI,digiChain);

  if ( t == AcdCalibData::PEDESTAL ) {
    m_rawHists = bookHists(AcdCalib::H_RAW,4096,-0.5,4095.5);
  } else if ( t == AcdCalibData::GAIN ) {
    m_gainHists = bookHists(AcdCalib::H_GAIN,64,-0.5,4095.5);    
  } else if ( t == AcdCalibData::VETO ) {
    m_rawHists = bookHists(AcdCalib::H_RAW,256,-0.5,4095.5);
    m_vetoHists = bookHists(AcdCalib::H_VETO,256,-0.5,4095.5);
    m_fracHists = bookHists(AcdCalib::H_FRAC,256,-0.5,4095.5);
  } else if ( t == AcdCalibData::RANGE ) {
    m_rangeHists = bookHists(AcdCalib::H_RANGE,4096,-0.5,4095.5,2);
  } else if ( t == AcdCalibData::CNO ) {    
    m_rawHists = bookHists(AcdCalib::H_RAW,64,-0.5,255.5);
    m_vetoHists = bookHists(AcdCalib::H_VETO,64,-0.5,255.5);
    m_fracHists = bookHists(AcdCalib::H_FRAC,64,-0.5,255.5);
  } else if ( t == AcdCalibData::UNPAIRED ) {
    m_unPairHists = bookHists(AcdCalib::H_RAW,512,-0.5,4095.5);
  } else if ( t == AcdCalibData::HITMAP ) {
    m_hitMapHist = new TH2F("hitMapHist","hitMapHist",128,-0.5,127.5,3,0.5,3.5);  
    m_condArrHist = new TH2F("condArrHist","condArrHist",32,-0.5,31.5,2,0.5,2.5);
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
    digiChain->SetBranchStatus("m_evtId", 1);
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
      if ( m_requirePeriodic &&
	   (m_digiEvent->getGem().getConditionSummary() != 32 &&  
	    m_digiEvent->getGem().getConditionSummary() != 128 ) )
	filtered = kTRUE;
      break;
    case AcdCalibData::HITMAP:
      if ( m_digiEvent->getGem().getConditionSummary() >= 32 ) 
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

  if ( calType() == AcdCalibData::HITMAP ) {
    used = kTRUE;
    compareDigiToGem();
    return;
  } else if ( calType() == AcdCalibData::CNO ) {  
    m_garcGafeHits.reset();
    //printf("0x%05x\n", m_digiEvent->getGem().getCnoVector() );
    m_garcGafeHits.setCNO( m_digiEvent->getGem().getCnoVector() );
  }

  int nAcdDigi = acdDigiCol->GetLast() + 1;
  for(int i = 0; i != nAcdDigi; ++i) {

    AcdDigi* acdDigi = static_cast<AcdDigi*>(acdDigiCol->At(i));
    assert(acdDigi != 0);

    const AcdId& acdId = acdDigi->getId();
    int id = acdId.getId();

    if ( ! AcdMap::channelExists( id ) ) continue;

    // NOT CNO calibaration
    int rng0 = acdDigi->getRange(AcdDigi::A);    
    float pmt0 = (float)acdDigi->getPulseHeight(AcdDigi::A);

    int rng1 = acdDigi->getRange(AcdDigi::B);
    float pmt1 = (float)acdDigi->getPulseHeight(AcdDigi::B);

    Bool_t useA = rng0 == 0 && pmt0 > 0;
    Bool_t useB = rng1 == 0 && pmt1 > 0;

    UInt_t keyA = AcdMap::makeKey(AcdDigi::A,id);
    UInt_t keyB = AcdMap::makeKey(AcdDigi::B,id);

    if ( calType() == AcdCalibData::GAIN ) {
      float pedA = getPeds(keyA);
      float pedB = getPeds(keyB);
      if ( pedA < 0 || pedB < 0 ) return;
      pmt0 -=  pedA;
      pmt1 -=  pedB;
      useA &= pmt0 > 50.;
      useB &= pmt1 > 50.;
    } else if ( calType() == AcdCalibData::CNO ) {
      float pedA = getPeds(keyA);
      float pedB = getPeds(keyB);
      if ( pedA < 0 || pedB < 0 ) return;
      if ( rng0 == 0 ) {
	pmt0 = 0;
      } else {      
	pmt0 -=  pedA;
	pmt0 += 15;
      }
      if ( rng1 == 0 ) {
	pmt1 = 0;
      } else {      
	pmt1 -=  pedB;
	pmt1 += 15;
      }
      m_garcGafeHits.setDigi(*acdDigi,pmt0,pmt1);
    } else if ( calType() == AcdCalibData::VETO ) {
      float pedA = getPeds(keyA);
      float pedB = getPeds(keyB);
      if ( pedA < 0 || pedB < 0 ) return;
      useA &= pmt0 > ( pedA + 10. );
      useB &= pmt1 > ( pedB + 10. );
    }

    switch ( calType () ) {
    case AcdCalibData::PEDESTAL:
      if ( useA ) fillHist(*m_rawHists, id, AcdDigi::A, pmt0);
      if ( useB ) fillHist(*m_rawHists, id, AcdDigi::B, pmt1);
      break;
    case AcdCalibData::GAIN:
      if ( useA ) fillHist(*m_gainHists, id, AcdDigi::A, pmt0);
      if ( useB ) fillHist(*m_gainHists, id, AcdDigi::B, pmt1);
      break;
    case AcdCalibData::RANGE:
      if ( useA ) fillHist(*m_rangeHists, id, AcdDigi::A, pmt0, 0);
      if ( useB ) fillHist(*m_rangeHists, id, AcdDigi::B, pmt1, 0);
      if ( rng0 ) fillHist(*m_rangeHists, id, AcdDigi::A, pmt0, 1);
      if ( rng1 ) fillHist(*m_rangeHists, id, AcdDigi::B, pmt1, 1);
      break;
    case AcdCalibData::UNPAIRED:
      if ( pmt1 == 0 && rng0 == 0) fillHist(*m_unPairHists, id, AcdDigi::A, pmt0);
      if ( pmt0 == 0 && rng1 == 0) fillHist(*m_unPairHists, id, AcdDigi::B, pmt1);
      break;
    case AcdCalibData::VETO:
      if ( useA ) fillHist(*m_rawHists, id, AcdDigi::A, pmt0);
      if ( useB ) fillHist(*m_rawHists, id, AcdDigi::B, pmt1);      
      if ( useA && acdDigi->getVeto(AcdDigi::A) ) fillHist(*m_vetoHists, id, AcdDigi::A, pmt0);
      if ( useB && acdDigi->getVeto(AcdDigi::B) ) fillHist(*m_vetoHists, id, AcdDigi::B, pmt1);
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
  for ( UInt_t iPmt(0); iPmt < AcdMap::nPmt; iPmt++ ) {    
    for ( UInt_t iFace(0); iFace < AcdMap::nFace; iFace++ ) {
      UInt_t nRow = AcdMap::getNRow(iFace);
      for ( UInt_t iRow(0); iRow < nRow; iRow++ ) {
	UInt_t nCol = AcdMap::getNCol(iFace,iRow);
	for ( UInt_t iCol(0); iCol < nCol; iCol++ ) {
	  UInt_t key = AcdMap::makeKey(iPmt,iFace,iRow,iCol);
	  TH1* raw = m_rawHists->getHist(key);
	  TH1* veto = m_vetoHists->getHist(key);
	  TH1* vf = m_fracHists->getHist(key);
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
  return m_fracHists;
}

void AcdCalibLoop_Digi::compareDigiToGem() {

  const TObjArray* acdDigiCol = m_digiEvent->getAcdDigiCol();
  assert ( acdDigiCol != 0 );
  
  const Gem& gem = m_digiEvent->getGem();
  const GemTileList& gtl = gem.getTileList();

  UInt_t gemSide[4];
  UInt_t aemSide[4];

  for ( UInt_t iZ(0); iZ < 4; iZ++ ) {
    gemSide[iZ] = aemSide[iZ] = 0;
  }

  gemSide[0] = (gtl.getXzp() << 16) | gtl.getXzm();
  gemSide[1] = (gtl.getYzp() << 16) | gtl.getYzm();
  gemSide[2] = gtl.getXy();
  gemSide[3] = (gtl.getNa() << 16) | gtl.getRbn();

  int nAcdDigi = acdDigiCol->GetLast() + 1;
  for(int i = 0; i != nAcdDigi; ++i) {

    AcdDigi* acdDigi = static_cast<AcdDigi*>(acdDigiCol->At(i));
    assert(acdDigi != 0);

    if ( (!acdDigi->getVeto(AcdDigi::A)) && (!acdDigi->getVeto(AcdDigi::B) ) ) continue;
    
    UShort_t gid = AcdCalibUtil::gemId(acdDigi->getId().getId());
    UShort_t gword = gid / 32;
    UShort_t gbit = gid % 32;
    if (gword > 3 ) continue;
    aemSide[gword] += 1 << gbit;  
  }

  UInt_t nGem(0);
  UInt_t nAem(0);
  UInt_t nMatch(0);
  for ( UInt_t iC(0); iC < 4; iC++ ) {
    if ( (gemSide[iC] == 0) && (aemSide[iC] == 0) ) continue;
    for ( UInt_t iB(0); iB < 32; iB++ ) {
      UInt_t test = (1 << iB);
      Bool_t hasGem = (gemSide[iC] & test) != 0;
      Bool_t hasAem = (aemSide[iC] & test) != 0;
      if ( ! hasGem && ! hasAem ) continue;      
      if ( hasGem ) nGem++;
      if ( hasAem ) nAem++;
      if ( hasGem && hasAem ) nMatch++;
      Float_t xVal( 32. * iC + iB );
      Float_t yVal(0.);
      if ( hasAem ) yVal += 1.;
      if ( hasGem ) yVal += 2.;
      m_hitMapHist->Fill(xVal,yVal,1.);
    }
  }


  // exactly one hit, check the cond arrival times
  if ( nGem == 1 && (gemSide[1] & (1 << 13)) ) {
  //if ( nGem == 1 ) {
    UShort_t roiTime = gem.getCondArrTime().roi();
    if ( nMatch > 0 ) {
      if ( roiTime > 8 && roiTime < 31 ) {
	printf("\nBOTH %08x %08x %08x %08x %i\n",gemSide[3],gemSide[2],gemSide[1],gemSide[0],roiTime);
      } 
      m_condArrHist->Fill((Float_t)roiTime,1.,1.);
    } else {
      if ( roiTime < 6 || roiTime == 31 ) {
	printf("\nGEM  %08x %08x %08x %08x %i\n",gemSide[3],gemSide[2],gemSide[1],gemSide[0],roiTime);
      } 
      m_condArrHist->Fill((Float_t)roiTime,2.,1.);
    }
  }
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
      //UInt_t key = AcdMap::makeKey(pmt,tile);
      fillHist(*m_rawHists,tile,pmt,inPha);
      if ( cno ) {
	fillHist(*m_vetoHists,tile,pmt,inPha);
      }
    }
  }
}
