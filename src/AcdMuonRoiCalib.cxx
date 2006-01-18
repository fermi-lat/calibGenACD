#include <fstream>
#include "TH1F.h"
#include "TF1.h"
#include "AcdMuonRoiCalib.h"

#include "AcdHistCalibMap.h"
#include "AcdPedestalFit.h"
#include "AcdGainFit.h"

#include "digiRootData/DigiEvent.h"

#include <cassert>
#include <cmath>

using std::cout;
using std::cerr;
using std::endl;
using std::string;

ClassImp(AcdMuonRoiCalib) ;

AcdMuonRoiCalib::AcdMuonRoiCalib(TChain *digiChain, TChain *meritChain, 
				 const char *histFileName)
  :AcdCalibBase(), 
   m_digiChain(digiChain), m_meritChain(meritChain), 
   m_digiEvent(0),
   m_reconDirZ(-9999){

  Bool_t ok = bookHists(histFileName);
  if ( !ok ) {
    cerr << "ERR:  Failed to book histograms to file " << histFileName <<endl;
  }

  ok = attachChains();
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
  }
  
  if (m_meritChain != 0) {
    m_meritChain->SetBranchStatus("*",0);  // disable all branches
    m_meritChain->SetBranchAddress("VtxXDir", &m_reconDirX);
    m_meritChain->SetBranchAddress("VtxYDir", &m_reconDirY);
    m_meritChain->SetBranchAddress("VtxZDir", &m_reconDirZ);
    
    // activate desired branches
    m_meritChain->SetBranchStatus("VtxXDir", 1);
    m_meritChain->SetBranchStatus("VtxYDir", 1);
    m_meritChain->SetBranchStatus("VtxZDir", 1);
  }
  return kTRUE;
}

void AcdMuonRoiCalib::getFitDir()
{  
  if ( fabs(m_reconDirX) < 0.000001 ) m_reconDirX = -9999;  
  if ( fabs(m_reconDirY) < 0.000001 ) m_reconDirY = -9999;  
  if ( fabs(m_reconDirZ) < 0.000001 ) m_reconDirZ = -9999;  
}

void AcdMuonRoiCalib::fillGainHistCorrect(Int_t id, Int_t pmt, Int_t range, Int_t pha) {

  Float_t phaCorrect(0.);
  if ( range != 0 ) return;
  AcdPedestalFitMap* peds = getPedestals();
  if ( peds == 0 || m_meritChain == 0) {
    if ( pha < 1 ) return;
    phaCorrect = (Float_t)(pha);
  } else {
    UInt_t key = AcdMap::makeKey(pmt,id);
    AcdPedestalFitResult* pedRes = peds->find(key);
    if ( pedRes == 0 ) {
      return;
    }
    Float_t ped = pedRes->mean();
    Float_t redPha = ((Float_t)(pha)) - ped;
    int face = AcdMap::getFace(key);
    Float_t angleCorr(1.);
    switch (face) {
    case 0: angleCorr = (-1./ m_reconDirZ); break;
    case 1: angleCorr = (-1. / m_reconDirX); break;
    case 3: angleCorr = (1. / m_reconDirX); break;
    case 2: angleCorr = (-1. / m_reconDirY); break;
    case 4: angleCorr = (1. / m_reconDirY); break;
    }
    phaCorrect = redPha * angleCorr;    
  }
  fillGainHist(id,pmt,phaCorrect);  
}


Bool_t AcdMuonRoiCalib::readEvent(int ievent, Bool_t& filtered, 
				  int& runId, int& evtId) {
  
  if(m_digiEvent) m_digiEvent->Clear();
  
  if(m_digiChain) { 
    m_digiChain->GetEvent(ievent);
    evtId = m_digiEvent->getEventId(); 
    runId = m_digiEvent->getRunId();
  }
  if(m_meritChain) m_meritChain->GetEvent(ievent);
  
  filtered = kFALSE;
  
  return kTRUE;
}


void AcdMuonRoiCalib::useEvent(Bool_t& used) {

  used = kFALSE;
  const TObjArray* acdDigiCol = m_digiEvent->getAcdDigiCol();
  if (!acdDigiCol) return;

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
    int pmt0 = acdDigi->getPulseHeight(AcdDigi::A);

    int rng1 = acdDigi->getRange(AcdDigi::B);
    int pmt1 = acdDigi->getPulseHeight(AcdDigi::B);

    if ( calType() == GAIN ) {
      if ( m_meritChain !=  0 ) {
	if(m_reconDirX < -9990) continue;
	if(m_reconDirY < -9990) continue;
	if(m_reconDirZ < -9990) continue;
	if(m_reconDirZ > -0.1 ) continue;
      }
    }

    switch ( calType () ) {
    case PEDESTAL:
      if ( rng0 == 0 ) fillPedestalHist(id, AcdDigi::A, pmt0);
      if ( rng1 == 0 ) fillPedestalHist(id, AcdDigi::B, pmt1);
      break;
    case GAIN:
      fillGainHistCorrect(id, AcdDigi::A, rng0, pmt0);
      fillGainHistCorrect(id, AcdDigi::B, rng1, pmt1);
      break;
    case UNPAIRED:
      if ( pmt1 == 0 && rng0 == 0) fillUnpairedHist(id, AcdDigi::A, pmt0);
      if ( pmt0 == 0 && rng1 == 0) fillUnpairedHist(id, AcdDigi::B, pmt1);
      break;
    }
    
    used = kTRUE;
  }
}
