

#include <fstream>
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "AcdMuonRoiCalib.h"

#include "AcdHistCalibMap.h"
#include "AcdPedestalFit.h"
#include "AcdGainFit.h"
#include "AcdCalibUtil.h"

#include "digiRootData/DigiEvent.h"

#include <cassert>
#include <cmath>

using std::cout;
using std::cerr;
using std::endl;
using std::string;

ClassImp(AcdMuonRoiCalib) ;

AcdMuonRoiCalib::AcdMuonRoiCalib(TChain *digiChain, Bool_t requirePeriodic, AcdMap::Config config)
  :AcdCalibBase(config), 
   m_digiChain(digiChain),
   m_digiEvent(0),
   m_requirePeriodic(requirePeriodic){

  setCalType(PEDESTAL);
  m_peds = 0;

  m_pedHists = bookHists(PEDESTAL,4096,-0.5,4095.5);
  m_gainHists = bookHists(GAIN,64,-0.5,4095.5);
  m_unPairHists = bookHists(UNPAIRED,512,-0.5,4095.5);
  m_rawHists = bookHists(RAW,64,-0.5,4095.5);
  m_vetoHists = bookHists(VETO,64,-0.5,4095.5);
  m_vfHists = bookHists(VETO_FRAC,64,-0.5,4095.5);

  m_hitMapHist = new TH2F("hitMapHist","hitMapHist",128,-0.5,127.5,3,0.5,3.5);  
  m_condArrHist = new TH2F("condArrHist","condArrHist",32,-0.5,31.5,2,0.5,2.5);

  Bool_t ok = attachChains();
  if ( ! ok ) {
    cerr << "ERR:  Failed to attach to input chains."  << endl;
  }
}


AcdMuonRoiCalib::~AcdMuonRoiCalib() 
{
  if (m_digiEvent) delete m_digiEvent;	
}

Bool_t AcdMuonRoiCalib::attachChains() {
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

Bool_t AcdMuonRoiCalib::readEvent(int ievent, Bool_t& filtered, 
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


void AcdMuonRoiCalib::useEvent(Bool_t& used) {

  used = kFALSE;
  const TObjArray* acdDigiCol = m_digiEvent->getAcdDigiCol();
  if (!acdDigiCol) return;

  if ( calType() == HITMAP ) {
    used = kTRUE;
    compareDigiToGem();
    return;
  }

  int nAcdDigi = acdDigiCol->GetLast() + 1;
  for(int i = 0; i != nAcdDigi; ++i) {

    AcdDigi* acdDigi = static_cast<AcdDigi*>(acdDigiCol->At(i));

    assert(acdDigi != 0);

    const AcdId& acdId = acdDigi->getId();
    int id = acdId.getId();
    //int face = acdId.getFace();
    //int row = face < 5 ? acdId.getRow() : 0;
    //int col = face < 5 ? acdId.getColumn() : acdId.getRibbonNumber();

    if ( ! AcdMap::channelExists( id ) ) continue;
    
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
      }
      if ( calType() == VETO ) {
	useA &= pmt0 > ( pedRes_A->mean() + 30. );
	useB &= pmt1 > ( pedRes_B->mean() + 30. );
      }
    }

    switch ( calType () ) {
    case PEDESTAL:
      if ( useA ) fillHist(*m_pedHists, id, AcdDigi::A, pmt0);
      if ( useB ) fillHist(*m_pedHists, id, AcdDigi::B, pmt1);
      break;
    case GAIN:
      if ( useA ) fillHist(*m_gainHists, id, AcdDigi::A, pmt0);
      if ( useB ) fillHist(*m_gainHists, id, AcdDigi::B, pmt1);
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
}

AcdPedestalFitMap* AcdMuonRoiCalib::fitPedestals(AcdPedestalFit& fitter) { 
  m_peds = new AcdPedestalFitMap;
  addCalibration(PEDESTAL,*m_peds);
  AcdHistCalibMap* hists = getHistMap(PEDESTAL);
  fitter.fitAll(*m_peds,*hists);
  return m_peds;
}

AcdGainFitMap* AcdMuonRoiCalib::fitGains(AcdGainFit& fitter) {
  m_gains = new AcdGainFitMap;
  addCalibration(GAIN,*m_gains);
  AcdHistCalibMap* hists = getHistMap(GAIN);
  fitter.fitAll(*m_gains,*hists);
  return m_gains;
}  

AcdVetoFitMap* AcdMuonRoiCalib::fitVetos(AcdVetoFit& fitter) {
  m_vetos = new AcdVetoFitMap;
  addCalibration(VETO,*m_vetos);
  AcdHistCalibMap* hists = getHistMap(VETO_FRAC);
  fitter.fitAll(*m_vetos,*hists);
  return m_vetos;
}  

AcdHistCalibMap* AcdMuonRoiCalib::makeVetoRatio() {
  for ( UInt_t iPmt(0); iPmt < AcdMap::nPmt; iPmt++ ) {    
    for ( UInt_t iFace(0); iFace < AcdMap::nFace; iFace++ ) {
      UInt_t nRow = AcdMap::getNRow(iFace);
      for ( UInt_t iRow(0); iRow < nRow; iRow++ ) {
	UInt_t nCol = AcdMap::getNCol(iFace,iRow);
	for ( UInt_t iCol(0); iCol < nCol; iCol++ ) {
	  UInt_t key = AcdMap::makeKey(iPmt,iFace,iRow,iCol);
	  TH1* raw = m_rawHists->getHist(key);
	  TH1* veto = m_vetoHists->getHist(key);
	  TH1* vf = m_vfHists->getHist(key);
	  if ( raw == 0 || veto == 0 || vf == 0 ) {
	    std::cout << "missing one " << key << std::endl;
	    continue;
	  }
	  Int_t nVeto = veto->GetEntries();
	  if ( nVeto > 100 && veto->GetMaximum() > 10 ) {	  
	    AcdCalibUtil::efficDivide(*vf,*veto,*raw,kFALSE,10.);
	  }
	}
      }
    }
  }
  return m_vfHists;
}


void AcdMuonRoiCalib::compareDigiToGem() {

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



/// for writing output files
void AcdMuonRoiCalib::writeXmlSources(ostream& os) const {
  std::string pedFileName;
  if ( m_peds != 0 ) pedFileName +=  m_peds->fileName();
  os << "pedestalFile=" << pedFileName << std::endl;
  TObjArray* files = m_digiChain != 0 ? m_digiChain->GetListOfFiles() : 0;
  if ( files != 0 ) {
    for ( Int_t i(0); i < files->GetEntriesFast(); i++ ) {
      TObject* obj = files->At(i);
      os << "inputDigi=" << obj->GetTitle() << std::endl;
    }
  }
}

void AcdMuonRoiCalib::writeTxtSources(ostream& os) const {
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


Bool_t AcdMuonRoiCalib::readPedestals(const char* fileName) {
  Bool_t latchVal = readCalib(PEDESTAL,fileName);
  AcdCalibMap* map = getCalibMap(PEDESTAL);
  m_peds = (AcdPedestalFitMap*)(map);
  return latchVal;
}

