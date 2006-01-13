#include <fstream>
#include "TH1F.h"
#include "TF1.h"
#include "AcdMuonTkrCalib.h"

#include "AcdHistCalibMap.h"
#include "AcdPedestalFit.h"
#include "AcdGainFit.h"

#include "digiRootData/DigiEvent.h"
#include "reconRootData/ReconEvent.h"

#include <cassert>
#include <cmath>

using std::cout;
using std::cerr;
using std::endl;
using std::string;

ClassImp(AcdMuonTkrCalib) ;

AcdMuonTkrCalib::AcdMuonTkrCalib(TChain* digiChain, TChain *reconChain,
				 const char *histFileName)
  :AcdCalibBase(), 
   m_digiChain(digiChain),
   m_reconChain(reconChain), 
   m_digiEvent(0),
   m_reconEvent(0),
   m_startEvent(0){

  Bool_t ok = bookHists(histFileName);
  if ( !ok ) {
    cerr << "ERR:  Failed to book histograms to file " << histFileName <<endl;
  }
  
  ok = attachChains();
  if ( ! ok ) {
    cerr << "ERR:  Failed to attach to input chains."  << endl;
  }
}


AcdMuonTkrCalib::~AcdMuonTkrCalib() 
{
  if (m_digiEvent) delete m_digiEvent;
  if (m_reconEvent) delete m_reconEvent;	
}

void AcdMuonTkrCalib::doAcd() 
{
  const AcdRecon* acdRecon= m_reconEvent->getAcdRecon();
  if (!acdRecon) return;

  const TObjArray* acdDigiCol = m_digiEvent->getAcdDigiCol();
  if (!acdDigiCol) return;

  int nAcdInter = acdRecon->nAcdIntersections();
  for(int i = 0; i != nAcdInter; ++i) {

    const AcdTkrIntersection* acdInter = acdRecon->getAcdTkrIntersection(i);
    if ( acdInter->getTrackIndex() > 0 ) continue;

    assert(acdInter != 0);

    const AcdId& acdId = acdInter->getTileId();
    unsigned id = acdId.getId();

    int nAcdDigi = acdDigiCol->GetLast() + 1;
    for(int j = 0; j != nAcdDigi; ++j) {      
      const AcdDigi* acdDigi = static_cast<const AcdDigi*>(acdDigiCol->At(j));      
      assert(acdDigi != 0);
      const AcdId& acdIdCheck = acdDigi->getId();
      if ( acdIdCheck.getId() != id ) continue;
      fillGainHistCorrect(*acdInter,*acdDigi);
    }
  }
}

void AcdMuonTkrCalib::go(int numEvents)
{        

  // determine how many events to process
  int nEntries = (int) m_reconChain->GetEntries();
  cout << "Number of events in the recon chain: " << nEntries << endl;
  int nMax = TMath::Min(numEvents+m_startEvent,nEntries);
  cout << "Number of events used: " << nMax-m_startEvent << endl;
  if (m_startEvent == nEntries) {
    cout << " all events in file read" << endl;
    return;
  }
  if (nEntries <= 0) return;
    
  // BEGINNING OF EVENT LOOP
  for (Int_t ievent=m_startEvent; ievent!=nMax; ++ievent) {
        
    if(m_digiEvent) m_digiEvent->Clear();
    if(m_reconEvent) m_reconEvent->Clear();

    if(m_digiChain) m_digiChain->GetEvent(ievent);
    if(m_reconChain) m_reconChain->GetEvent(ievent);

    if(m_digiEvent && m_reconEvent) {
      int digiEventId = m_digiEvent->getEventId(); 
      int digiRunNum = m_digiEvent->getRunId();
      int reconEventId = m_reconEvent->getEventId(); 
      int reconRunNum = m_reconEvent->getRunId();
      assert ( digiEventId == reconEventId );
      assert ( digiRunNum == reconRunNum );

      if ( ievent == 0 ) { ;}
      else if ( ievent % 100000 == 0 ) { std::cout << 'x' << std::endl; }
      else if ( ievent % 10000 == 0 ) { std::cout << 'x' << std::flush; }
      else if ( ievent % 1000 == 0 ) { std::cout << '.' << std::flush; }

      doAcd();
    }
        
  }  // end analysis code in event loop    
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
  
  float pathFactor = inter.getPathLengthInTile() / 10.;

  int id = digi.getId().getId();

  AcdPedestalFitMap* peds = getPedestals();
  if ( peds == 0 ) return;
  UInt_t keyA = AcdMap::makeKey(AcdDigi::A,id);
  UInt_t keyB = AcdMap::makeKey(AcdDigi::B,id);

  AcdPedestalFitResult* pedRes_A = peds->find(keyA);
  AcdPedestalFitResult* pedRes_B = peds->find(keyB);
  if ( pedRes_A == 0 && pedRes_B == 0 ) return;

  Float_t redPha_A = ((Float_t)(pmt0)) - pedRes_A->mean();
  Float_t redPha_B = ((Float_t)(pmt1)) - pedRes_B->mean();
  
  redPha_A /= pathFactor;
  redPha_B /= pathFactor;

  if ( rng0 == 0 ) fillGainHist(id,AcdDigi::A,redPha_A);
  if ( rng1 == 0 ) fillGainHist(id,AcdDigi::B,redPha_B);
  
}
