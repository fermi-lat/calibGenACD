

#include <fstream>
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "AcdCalibLoop_Digi.h"

#include "AcdHistCalibMap.h"
#include "AcdPedestalFit.h"
#include "AcdGainFit.h"
#include "AcdCalibUtil.h"
#include "AcdGarcGafeHits.h"

#include "digiRootData/DigiEvent.h"

#include <cassert>
#include <cmath>

using std::cout;
using std::cerr;
using std::endl;
using std::string;

ClassImp(AcdCalibLoop_Digi) ;

AcdCalibLoop_Digi::AcdCalibLoop_Digi(CALTYPE t, TChain *digiChain, Bool_t requirePeriodic, AcdMap::Config config)
  :AcdCalibBase(t,config), 
   m_digiChain(digiChain),
   m_digiEvent(0),
   m_requirePeriodic(requirePeriodic){

  m_peds = 0;
  m_ranges = 0;

  if ( t == PEDESTAL ) {
    m_rawHists = bookHists(H_RAW,4096,-0.5,4095.5);
  } else if ( t == GAIN ) {
    m_gainHists = bookHists(H_GAIN,64,-0.5,4095.5);    
  } else if ( t == VETO ) {
    m_rawHists = bookHists(H_RAW,64,-0.5,4095.5);
    m_vetoHists = bookHists(H_VETO,64,-0.5,4095.5);
    m_fracHists = bookHists(H_FRAC,64,-0.5,4095.5);
  } else if ( t == RANGE ) {
    m_rawHists = bookHists(H_RAW,4096,-0.5,4095.5);
    m_rangeHists = bookHists(H_RANGE,4096,-0.5,4095.5);
  } else if ( t == CNO ) {    
    m_rawHists = bookHists(H_RAW,256,-0.5,255.5);
    m_vetoHists = bookHists(H_VETO,256,-0.5,255.5);
    m_fracHists = bookHists(H_FRAC,256,-0.5,255.5);
  } else if ( t == UNPAIRED ) {
    m_unPairHists = bookHists(H_RAW,512,-0.5,4095.5);
  } else if ( t == HITMAP ) {
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
  if (m_digiChain != 0) {
    m_digiEvent = 0;
    m_digiChain->SetBranchAddress("DigiEvent", &m_digiEvent);
    m_digiChain->SetBranchStatus("*",0);  // disable all branches
    // activate desired brances
    m_digiChain->SetBranchStatus("m_acd*",1);
    m_digiChain->SetBranchStatus("m_eventId", 1); 
    m_digiChain->SetBranchStatus("m_runId", 1);
    //if ( m_requirePeriodic ) {
    m_digiChain->SetBranchStatus("m_gem", 1);
    //}
  }
  return kTRUE;
}

Bool_t AcdCalibLoop_Digi::readEvent(int ievent, Bool_t& filtered, 
				  int& runId, int& evtId) {
  
  if(m_digiEvent) m_digiEvent->Clear();

  filtered = kFALSE;

  if(m_digiChain) { 
    m_digiChain->GetEvent(ievent);
    evtId = m_digiEvent->getEventId(); 
    runId = m_digiEvent->getRunId();
    switch ( calType () ) {
    case PEDESTAL:
      if ( m_requirePeriodic &&
	   (m_digiEvent->getGem().getConditionSummary() != 32 &&  
	    m_digiEvent->getGem().getConditionSummary() != 128 ) )
	filtered = kTRUE;
      break;
    case HITMAP:
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

  if ( calType() == HITMAP ) {
    used = kTRUE;
    compareDigiToGem();
    return;
  } else if ( calType() == CNO ) {  
    m_garcGafeHits.reset();
    m_garcGafeHits.setCNO( m_digiEvent->getGem().getCnoVector() );
  }

  int nAcdDigi = acdDigiCol->GetLast() + 1;
  for(int i = 0; i != nAcdDigi; ++i) {

    AcdDigi* acdDigi = static_cast<AcdDigi*>(acdDigiCol->At(i));
    assert(acdDigi != 0);

    const AcdId& acdId = acdDigi->getId();
    int id = acdId.getId();

    if ( ! AcdMap::channelExists( id ) ) continue;

    if ( calType() == CNO ) {  
      m_garcGafeHits.setDigi(*acdDigi);
      continue;
    }

    // NOT CNO calibaration
    int rng0 = acdDigi->getRange(AcdDigi::A);    
    float pmt0 = (float)acdDigi->getPulseHeight(AcdDigi::A);

    int rng1 = acdDigi->getRange(AcdDigi::B);
    float pmt1 = (float)acdDigi->getPulseHeight(AcdDigi::B);

    Bool_t useA = rng0 == 0 && pmt0 > 0;
    Bool_t useB = rng1 == 0 && pmt1 > 0;
    if ( m_peds != 0 ) {
      UInt_t keyA = AcdMap::makeKey(AcdDigi::A,id);
      UInt_t keyB = AcdMap::makeKey(AcdDigi::B,id);
      AcdPedestalFitResult* pedRes_A = m_peds->find(keyA);
      AcdPedestalFitResult* pedRes_B = m_peds->find(keyB);      
      if ( pedRes_A == 0 && pedRes_B == 0 ) return;
      if ( calType() == GAIN ) {
	pmt0 -=  pedRes_A->mean();
	pmt1 -=  pedRes_B->mean();
	useA &= pmt0 > 50.;
	useB &= pmt1 > 50.;
      } else if ( calType() == VETO ) {
	useA &= pmt0 > ( pedRes_A->mean() + 30. );
	useB &= pmt1 > ( pedRes_B->mean() + 30. );
      }
    }

    switch ( calType () ) {
    case PEDESTAL:
      if ( useA ) fillHist(*m_rawHists, id, AcdDigi::A, pmt0);
      if ( useB ) fillHist(*m_rawHists, id, AcdDigi::B, pmt1);
      break;
    case GAIN:
      if ( useA ) fillHist(*m_gainHists, id, AcdDigi::A, pmt0);
      if ( useB ) fillHist(*m_gainHists, id, AcdDigi::B, pmt1);
      break;
    case RANGE:
      if ( useA ) fillHist(*m_rawHists, id, AcdDigi::A, pmt0);
      if ( useB ) fillHist(*m_rawHists, id, AcdDigi::B, pmt1);
      if ( rng0 ) fillHist(*m_rangeHists, id, AcdDigi::A, pmt0);
      if ( rng1 ) fillHist(*m_rangeHists, id, AcdDigi::B, pmt1);
      break;
    case UNPAIRED:
      if ( pmt1 == 0 && rng0 == 0) fillHist(*m_unPairHists, id, AcdDigi::A, pmt0);
      if ( pmt0 == 0 && rng1 == 0) fillHist(*m_unPairHists, id, AcdDigi::B, pmt1);
      break;
    case VETO:
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

  if ( calType() == CNO ) {  
    fillCnoData();
  }
}

AcdPedestalFitMap* AcdCalibLoop_Digi::fitPedestals(AcdPedestalFit& fitter) { 
  m_peds = new AcdPedestalFitMap;
  addCalibration(PEDESTAL,*m_peds);
  AcdHistCalibMap* hists = getHistMap(H_RAW);
  fitter.fitAll(*m_peds,*hists);
  return m_peds;
}

AcdGainFitMap* AcdCalibLoop_Digi::fitGains(AcdGainFit& fitter) {
  m_gains = new AcdGainFitMap;
  addCalibration(GAIN,*m_gains);
  AcdHistCalibMap* hists = getHistMap(H_GAIN);
  fitter.fitAll(*m_gains,*hists);
  return m_gains;
}  

AcdCnoFitMap* AcdCalibLoop_Digi::fitCnos(AcdCnoFit& fitter) {
  m_cnos = new AcdCnoFitMap;
  addCalibration(CNO,*m_cnos);
  AcdHistCalibMap* hists = getHistMap(H_FRAC);
  fitter.fitAll(*m_cnos,*hists);
  return m_cnos;
}  

AcdVetoFitMap* AcdCalibLoop_Digi::fitVetos(AcdVetoFit& fitter) {
  m_vetos = new AcdVetoFitMap;
  addCalibration(VETO,*m_vetos);
  AcdHistCalibMap* hists = getHistMap(H_FRAC);
  fitter.fitAll(*m_vetos,*hists);
  return m_vetos;
}  

AcdRangeFitMap* AcdCalibLoop_Digi::fitRanges(AcdRangeFit& fitter) {
  m_ranges = new AcdRangeFitMap;
  addCalibration(RANGE,*m_ranges);
  AcdHistCalibMap* lowhists = getHistMap(H_RAW);
  AcdHistCalibMap* highhists = getHistMap(H_RANGE);
  fitter.fitAll(*m_ranges,*lowhists,*highhists);
  return m_ranges;
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
    if ( nHits > 1 ) continue;
    Int_t gafe(-1);
    if ( ! m_garcGafeHits.nextGarcHit(iGarc,gafe) ) continue;
    UInt_t uGafe = (UInt_t)gafe;
    UShort_t inPha = m_garcGafeHits.inPha(iGarc,uGafe);
    UShort_t flags = m_garcGafeHits.flags(iGarc,uGafe);
    if ( (flags & 0x4) != 0x4 ) continue; // low range

    UInt_t tile(0); UInt_t pmt(0);
    AcdGarcGafeHits::convertToTilePmt(iGarc,uGafe,tile,pmt);
    UInt_t key = AcdMap::makeKey(pmt,tile);
    AcdRangeFitResult* rangePed = m_ranges->find(key);
    if ( rangePed == 0 ) {
      std::cerr << "No range pedestal for Key " << tile << ' ' << pmt << std::endl;
      continue;
    }
    UShort_t phaRed = inPha > rangePed->high() ? inPha - rangePed->high() : 0;
    fillHist(*m_rawHists,tile,pmt,phaRed);
    if ( cno ) {
      fillHist(*m_vetoHists,tile,pmt,phaRed);
    }
  }
}


/// for writing output files
void AcdCalibLoop_Digi::writeXmlSources( DomElement& node) const {
  //std::string pedFileName;
  //if ( m_peds != 0 ) pedFileName +=  m_peds->fileName();
  //os << "pedestalFile=" << pedFileName << std::endl;
  //TObjArray* files = m_digiChain != 0 ? m_digiChain->GetListOfFiles() : 0;
  //if ( files != 0 ) {
  //  for ( Int_t i(0); i < files->GetEntriesFast(); i++ ) {
  //    TObject* obj = files->At(i);
  //    os << "inputDigi=" << obj->GetTitle() << std::endl;
  //  }
  //}
}

void AcdCalibLoop_Digi::writeTxtSources(ostream& os) const {
  std::string pedFileName;
  if ( m_peds != 0 ) pedFileName +=  m_peds->fileName();
  os << "#pedestalFile = " << pedFileName << endl;
  TObjArray* files = m_digiChain != 0 ? m_digiChain->GetListOfFiles() : 0;
  if ( files != 0 ) {
    for ( Int_t i(0); i < files->GetEntriesFast(); i++ ) {
      TObject* obj = files->At(i);
      os << "#inputDigiFile = " << obj->GetTitle() << endl;
    }
  }
}


Bool_t AcdCalibLoop_Digi::readPedestals(const char* fileName) {
  Bool_t latchVal = readCalib(PEDESTAL,fileName);
  AcdCalibMap* map = getCalibMap(PEDESTAL);
  m_peds = (AcdPedestalFitMap*)(map);
  return latchVal;
}

Bool_t AcdCalibLoop_Digi::readRanges(const char* fileName) {
  Bool_t latchVal = readCalib(RANGE,fileName);
  AcdCalibMap* map = getCalibMap(RANGE);
  m_ranges = (AcdRangeFitMap*)(map);
  return latchVal;
}
