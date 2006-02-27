#include <fstream>
#include "TH1F.h"
#include "TF1.h"
#include "AcdMeritCalib.h"

#include "AcdCalibUtil.h"
#include "AcdMap.h"

#include "digiRootData/DigiEvent.h"
#include "reconRootData/ReconEvent.h"

#include <cassert>
#include <cmath>

using std::cout;
using std::cerr;
using std::endl;
using std::string;

ClassImp(AcdMeritCalib) ;

AcdMeritCalib::AcdMeritCalib(TChain& digiChain, TChain& reconChain, TChain& meritChain)
  :AcdCalibBase(), 
   m_digiChain(&digiChain),
   m_reconChain(&reconChain),
   m_meritChain(&meritChain),
   m_digiEvent(0),
   m_reconEvent(0),
   m_gains(0),
   m_peds(0){
  
  Bool_t ok = attachChains();
  if ( ! ok ) {
    cerr << "ERR:  Failed to attach to input chains."  << endl;
  }
}


AcdMeritCalib::~AcdMeritCalib() 
{
  if (m_digiEvent) delete m_digiEvent;
  if (m_reconEvent) delete m_reconEvent;	
}

Bool_t AcdMeritCalib::attachChains() {
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
  
  if(m_meritChain ) {
    m_meritChain->SetBranchStatus("*",0);
    
    m_meritChain->SetBranchStatus("TkrNumTracks",1);
    m_meritChain->SetBranchAddress("TkrNumTracks",&m_TkrNumTracks);

    m_meritChain->SetBranchStatus("Tkr1SSDVeto",1);
    m_meritChain->SetBranchAddress("Tkr1SSDVeto",&m_Tkr1SSDVeto);

    m_meritChain->SetBranchStatus("Tkr1Hits",1);
    m_meritChain->SetBranchAddress("Tkr1Hits",&m_Tkr1Hits);

    m_meritChain->SetBranchStatus("Tkr1Chisq",1);
    m_meritChain->SetBranchAddress("Tkr1Chisq",&m_Tkr1Chisq);

    m_meritChain->SetBranchStatus("CalMIPRatio",1);
    m_meritChain->SetBranchAddress("CalMIPRatio",&m_CalMIPRatio);
    

  }

  m_outputTree = new TTree("AcdMiss","AcdMiss");
  m_outputTree->Branch("RunId",&m_runIdOut,"RunId/I");
  m_outputTree->Branch("EvtId",&m_evtIdOut,"EvtId/I");
  m_outputTree->Branch("AcdId",&m_acdIdOut,"AcdId/I");
  m_outputTree->Branch("HitMask",&m_hit,"HitMask/I");
  m_outputTree->Branch("Weight",&m_weight,"Weight/F");
  m_outputTree->Branch("Doca",&m_doca,"Doca/F");
  m_outputTree->Branch("sToX",&m_sToX,"sToX/F");
  m_outputTree->Branch("Tkr1SSDVeto",&m_Tkr1SSDVeto,"Tkr1SSDVeto/F");
  m_outputTree->Branch("nHits",&m_Tkr1Hits,"Tkr1Hits/F");
  m_outputTree->Branch("CalMIPRatio",&m_CalMIPRatio,"CalMIPRatio/F");
  m_outputTree->Branch("ChiSq",&m_Tkr1Chisq,"Tkr1Chisq/F");
  m_outputTree->Branch("GlobalX",&m_gX,"GlobalX/F");
  m_outputTree->Branch("GlobalY",&m_gY,"GlobalY/F");
  m_outputTree->Branch("GlobalZ",&m_gZ,"GlobalZ/F");
  m_outputTree->Branch("mipA",&m_mipA,"mipA/F");
  m_outputTree->Branch("mipB",&m_mipB,"mipB/F");
 
  return kTRUE;
}

Bool_t AcdMeritCalib::readEvent(int ievent, Bool_t& filtered, 
				int& runId, int& evtId) {

  filtered = kFALSE;
  
  if(m_digiEvent) m_digiEvent->Clear();
  if(m_reconEvent) m_reconEvent->Clear();

  if(m_meritChain) {
    m_meritChain->LoadTree(ievent);
    m_meritChain->GetEvent(ievent);
  }

  // filter on the merit quantities
  if ( m_TkrNumTracks > 1.5 )  filtered = kTRUE;
  if ( m_TkrNumTracks < 0.5 )  filtered = kTRUE;
  //if ( m_Tkr1SSDVeto > 0.5 ) filtered = kTRUE;
  //if ( m_Tkr1Hits < 12 ) filtered = kTRUE;
  if ( m_Tkr1Chisq > 2. ) filtered = kTRUE;
  if ( m_Tkr1Chisq < 0. ) filtered = kTRUE;
  // less than a MIP
  if ( m_CalMIPRatio > 1e-9 && m_CalMIPRatio < 0.5 ) filtered = kTRUE;
  // more than a MIP
  if ( m_CalMIPRatio > 1.5) filtered = kTRUE;
  if ( filtered ) return kTRUE;
  
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
  return kTRUE;

}


void AcdMeritCalib::useEvent(Bool_t& used) {

  used = kFALSE;
  m_evtIdOut = m_digiEvent->getEventId(); 
  m_runIdOut = m_digiEvent->getRunId();
  
  const TObjArray* acdDigiCol = m_digiEvent->getAcdDigiCol();
  if (!acdDigiCol) return;
  int nAcdDigi = acdDigiCol->GetLast() + 1;
  std::map<int,AcdDigi*> digiMap;

  for ( Int_t iDigi(0); iDigi < nAcdDigi; iDigi++ ) {
    AcdDigi* aDigi = (AcdDigi*)(acdDigiCol->At(iDigi));
    int digiId = aDigi->getId().getId();
    digiMap[digiId] = aDigi;
  }

  const AcdRecon* acdRecon= m_reconEvent->getAcdRecon();
  if (!acdRecon) return;

  int nAcdInter = acdRecon->nAcdIntersections();
  int nAcdPoca = acdRecon->nAcdTkrPoca();

  int i(0);
  float nAbove(0.);
  float nBelow(0.);

  m_mipA = 0.;
  m_mipB = 0.;

  // first loop, count number of intersections
  for(i = 0; i != nAcdInter; i++) {
    const AcdTkrIntersection* acdInter = acdRecon->getAcdTkrIntersection(i);
    assert(acdInter != 0);
    if ( acdInter->getTrackIndex() > 0 ) continue;    
    if ( acdInter->getArcLengthToIntersection() > 0. ) {
      nAbove += 1.;
    } else {
      nBelow += 1.;
    }
  }

  m_hit = 0;
  m_doca = -200.;
  if ( (acdRecon->getTileCount() + acdRecon->getRibbonCount()) > 0 ) {
    m_doca = -300.;
  }
  if ( nAbove < 0.5 && nAcdPoca == 0 ) {
    // Nothing extrapolates to the acd, but there was a track
    m_weight = 0;
    m_acdIdOut = -1;
    m_sToX = 0.;
    m_gX = -1000.;
    m_gY = -1000.;
    m_gZ = -1000.;    
    m_outputTree->Fill();
    used = kTRUE;
    return;    
  }

  int acdIdBest(-1);

  for ( Int_t iPoca(0); iPoca < nAcdPoca; iPoca++ ) {
    const AcdTkrPoca* aPoca = acdRecon->getAcdTkrPoca(iPoca);
    if ( aPoca->getTkrIndex() != 0 ) continue;
    if ( nAbove < 0.5 ) {
      // No intersections, but a poca, this is important
      m_weight = 0;
      m_acdIdOut = aPoca->getId().getId();
      m_doca = aPoca->getDoca();
      m_sToX = 0.;
      m_gX = aPoca->getPoca().X();
      m_gY = aPoca->getPoca().Y();
      m_gZ = aPoca->getPoca().Z();
      AcdDigi* digi = digiMap[m_acdIdOut];
      if ( digi != 0 ) {
	Int_t tileId = digi->getId().getId();
	UInt_t idA = AcdMap::makeKey(0,tileId);
	UInt_t idB = AcdMap::makeKey(1,tileId);
	UInt_t phaA = digi->getRange(AcdDigi::A) == 0 ? digi->getPulseHeight(AcdDigi::A) : 4096;
	UInt_t phaB = digi->getRange(AcdDigi::B) == 0 ? digi->getPulseHeight(AcdDigi::B) : 4096;
	m_mipA = toMip(idA,phaA);
	m_mipB = toMip(idB,phaB);
      }
      used = kTRUE;
      m_outputTree->Fill();
    } else {
      // compare to best poca
      if ( m_doca < aPoca->getDoca() ) {
	m_doca = aPoca->getDoca();
	acdIdBest = aPoca->getId().getId();
      }
    }
  }

  Float_t poca_mip_A(0.);
  Float_t poca_mip_B(0.);

  // latch the pha of the best POCA
  if ( acdIdBest >= 0 ) {
    AcdDigi* digi = digiMap[acdIdBest];
    if ( digi != 0 ) {
      Int_t tileId = digi->getId().getId();
      UInt_t idA = AcdMap::makeKey(0,tileId);
      UInt_t idB = AcdMap::makeKey(1,tileId);
      UInt_t phaA = digi->getRange(AcdDigi::A) == 0 ? digi->getPulseHeight(AcdDigi::A) : 4096;
      UInt_t phaB = digi->getRange(AcdDigi::B) == 0 ? digi->getPulseHeight(AcdDigi::B) : 4096;
      poca_mip_A = toMip(idA,phaA);
      poca_mip_B = toMip(idB,phaB);
    }
  }

  // second loop, handle the intersections, with weights from first loop
  for(i = 0; i != nAcdInter; i++) {
    const AcdTkrIntersection* acdInter = acdRecon->getAcdTkrIntersection(i);
    assert(acdInter != 0);
    if ( acdInter->getTrackIndex() > 0 ) continue;
    m_hit = (Int_t)acdInter->tileHit();
    if ( (m_hit & 15) == 15 ) continue; 
    used = kTRUE;
    if ( m_hit > 0 ) {
      Int_t tileId = acdInter->getTileId().getId();
      AcdDigi* digi = digiMap[acdIdBest];
      if ( digi != 0 ) {
	UInt_t idA = AcdMap::makeKey(0,tileId);
	UInt_t idB = AcdMap::makeKey(1,tileId);
	UInt_t phaA = digi->getRange(AcdDigi::A) == 0 ? digi->getPulseHeight(AcdDigi::A) : 4096;
	UInt_t phaB = digi->getRange(AcdDigi::B) == 0 ? digi->getPulseHeight(AcdDigi::B) : 4096;
	m_mipA = toMip(idA,phaA);
	m_mipB = toMip(idB,phaB);
      } else {
	m_mipA = -1.;
	m_mipB = -1.;
      }
    } else {
      m_mipA = -1.*poca_mip_A;
      m_mipB = -1.*poca_mip_B;	
    }

    m_sToX = acdInter->getArcLengthToIntersection();
    m_weight = m_sToX > 0. ? 1./nAbove :  1./nBelow;
    const AcdId& acdId = acdInter->getTileId();
    m_acdIdOut = acdId.getId();
    const TVector3& globalPos = acdInter->getGlobalPosition();
    m_gX = globalPos.X();
    m_gY = globalPos.Y();
    m_gZ = globalPos.Z();
    m_outputTree->Fill();
  }
}


Bool_t AcdMeritCalib::readPedestals(const char* fileName) {
  Bool_t latchVal = readCalib(PEDESTAL,fileName);
  AcdCalibMap* map = getCalibMap(PEDESTAL);
  m_peds = (AcdPedestalFitMap*)(map);
  return latchVal;
}


Bool_t AcdMeritCalib::readGains(const char* fileName) {
  Bool_t latchVal = readCalib(GAIN,fileName);
  AcdCalibMap* map = getCalibMap(GAIN);
  m_gains = (AcdGainFitMap*)(map);
  return latchVal;
}

/// for writing output files
void AcdMeritCalib::writeXmlHeader(ostream& os) const {
  AcdCalibBase::writeXmlHeader(os);
}

void AcdMeritCalib::writeTxtHeader(ostream& os) const {
  AcdCalibBase::writeTxtHeader(os);
}

Float_t AcdMeritCalib::toMip(UInt_t channel, Int_t pha) const {
  if ( pha == 0 ) return 0.; 
  if ( m_peds == 0 ) return -100.;
  if ( m_gains == 0 ) return -200.;
  const AcdPedestalFitResult* pedRes = m_peds->find(channel);
  if ( pedRes == 0 ) return -300.;
  Float_t ped = pedRes->mean();
  Float_t redPha = (Float_t)(pha) - ped;
  const AcdGainFitResult* gainRes = m_gains->find(channel);
  if ( gainRes == 0 ) return -400.;
  Float_t peak = gainRes->peak();
  peak -= ped;
  redPha /= peak;
  return redPha;
}
