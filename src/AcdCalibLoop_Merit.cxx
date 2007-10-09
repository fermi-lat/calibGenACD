#include "AcdCalibLoop_Merit.h"

#include "TH1F.h"
#include "TF1.h"
#include "TChain.h"

#include "AcdCalibUtil.h"
#include "AcdMap.h"

#include "digiRootData/DigiEvent.h"
#include "reconRootData/ReconEvent.h"

#include <cassert>
#include <cmath>
#include <fstream>

using std::cout;
using std::cerr;
using std::endl;
using std::string;

AcdCalibLoop_Merit::AcdCalibLoop_Merit(TChain& digiChain, TChain& reconChain, TChain& meritChain)
  :AcdCalibBase(AcdCalibData::MERITCALIB), 
   m_digiEvent(0),
   m_reconEvent(0){

  setChain(AcdCalib::DIGI,&digiChain);
  setChain(AcdCalib::RECON,&reconChain);
  setChain(AcdCalib::MERIT,&meritChain);    
  
  Bool_t ok = attachChains();
  if ( ! ok ) {
    cerr << "ERR:  Failed to attach to input chains."  << endl;
  }
}


AcdCalibLoop_Merit::~AcdCalibLoop_Merit() 
{
  if (m_digiEvent) delete m_digiEvent;
  if (m_reconEvent) delete m_reconEvent;	
}

Bool_t AcdCalibLoop_Merit::attachChains() {
  TChain* digiChain = getChain(AcdCalib::DIGI);
  TChain* reconChain = getChain(AcdCalib::RECON);
  TChain* meritChain = getChain(AcdCalib::MERIT);

  if (digiChain != 0) {
    m_digiEvent = 0;
    digiChain->SetBranchAddress("DigiEvent", &m_digiEvent);
    digiChain->SetBranchStatus("*",0);  // disable all branches
    // activate desired brances
    digiChain->SetBranchStatus("m_acd*",1);
    digiChain->SetBranchStatus("m_eventId", 1); 
    digiChain->SetBranchStatus("m_runId", 1);
  }
  
  if (reconChain != 0) {
    m_reconEvent = 0;
    reconChain->SetBranchAddress("ReconEvent", &m_reconEvent);
    reconChain->SetBranchStatus("*",0);  // disable all branches
    // activate desired brances
    reconChain->SetBranchStatus("m_acd",1);
    reconChain->SetBranchStatus("m_eventId", 1); 
    reconChain->SetBranchStatus("m_runId", 1);
  }
  
  if(meritChain ) {
    meritChain->SetBranchStatus("*",0);
    
    meritChain->SetBranchStatus("TkrNumTracks",1);
    meritChain->SetBranchAddress("TkrNumTracks",&m_TkrNumTracks);

    meritChain->SetBranchStatus("Tkr1SSDVeto",1);
    meritChain->SetBranchAddress("Tkr1SSDVeto",&m_Tkr1SSDVeto);

    meritChain->SetBranchStatus("Tkr1Hits",1);
    meritChain->SetBranchAddress("Tkr1Hits",&m_Tkr1Hits);

    meritChain->SetBranchStatus("Tkr1Chisq",1);
    meritChain->SetBranchAddress("Tkr1Chisq",&m_Tkr1Chisq);

    meritChain->SetBranchStatus("CalMIPRatio",1);
    meritChain->SetBranchAddress("CalMIPRatio",&m_CalMIPRatio);
    

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

Bool_t AcdCalibLoop_Merit::readEvent(int ievent, Bool_t& filtered, 
				int& runId, int& evtId) {

  filtered = kFALSE;
  
  if(m_digiEvent) m_digiEvent->Clear();
  if(m_reconEvent) m_reconEvent->Clear();

  TChain* digiChain = getChain(AcdCalib::DIGI);
  TChain* reconChain = getChain(AcdCalib::RECON);
  TChain* meritChain = getChain(AcdCalib::MERIT);

  if(meritChain) {
    meritChain->LoadTree(ievent);
    meritChain->GetEvent(ievent);
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
  return kTRUE;

}


void AcdCalibLoop_Merit::useEvent(Bool_t& used) {

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


Float_t AcdCalibLoop_Merit::toMip(UInt_t channel, Int_t pha) const {
  if ( pha == 0 ) return 0.; 

  Float_t ped = getPeds(channel);
  if ( ped < 0 ) return -100;

  Float_t redPha = (Float_t)(pha) - ped;
  
  // FIXME
  // Float_t mipPeak = getGains(channel);
  Float_t mipPeak = 1000.;
  if ( mipPeak < 0 ) return -200.;
  mipPeak -= ped;
  redPha /= mipPeak;
  return redPha;
}
