#include <fstream>
#include "TH1F.h"
#include "TF1.h"
#include "muonCalib.h"

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

ClassImp(muonCalib) ;

muonCalib::muonCalib(TChain *digiChain, TChain *meritChain, 
		     const char *histFileName)
  :AcdCalibBase(), 
   m_digiChain(digiChain), m_meritChain(meritChain), 
   m_digiEvent(0),
   m_startEvent(0),
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


muonCalib::~muonCalib() 
{
  if (m_digiEvent) delete m_digiEvent;	
}

void muonCalib::digiAcd() 
{
  const TObjArray* acdDigiCol = m_digiEvent->getAcdDigiCol();
  if (!acdDigiCol) return;

  int nAcdDigi = acdDigiCol->GetLast() + 1;
  for(int i = 0; i != nAcdDigi; ++i) {

    AcdDigi* acdDigi = static_cast<AcdDigi*>(acdDigiCol->At(i));

    assert(acdDigi != 0);

    const AcdId& id = acdDigi->getId();
    int face = id.getFace();
    int row = face < 5 ? id.getRow() : 0;
    int col = face < 5 ? id.getColumn() : id.getRibbonNumber();

    int pmt0 = acdDigi->getPulseHeight(AcdDigi::A);
    int rng0 = acdDigi->getRange(AcdDigi::A);
    
    int pmt1 = acdDigi->getPulseHeight(AcdDigi::B);
    int rng1 = acdDigi->getRange(AcdDigi::B);
    
    switch ( calType () ) {
    case PEDESTAL:
      fillPedestalHist(face, row, col, 0, rng0, pmt0);
      fillPedestalHist(face, row, col, 1, rng1, pmt1);
      break;
    case GAIN:
      fillGainHist(face, row, col, 0, rng0, pmt0);
      fillGainHist(face, row, col, 1, rng1, pmt1);
      break;
    case UNPAIRED:
      if ( pmt1 == 0 && rng0 == 0) fillUnpairedHist(face, row, col, 0, rng0, pmt0);
      if ( pmt0 == 0 && rng1 == 0) fillUnpairedHist(face, row, col, 1, rng1, pmt1);
      break;
    }
 }
}

void muonCalib::go(int numEvents)
{        

  // determine how many events to process
  int nEntries = (int) m_digiChain->GetEntries();
  cout << "Number of events in the digi chain: " << nEntries << endl;
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
               
    if(m_digiChain) m_digiChain->GetEvent(ievent);
    if(m_meritChain) m_meritChain->GetEvent(ievent);

    if(m_digiEvent) {
      int digiEventId = m_digiEvent->getEventId(); 
      int digiRunNum = m_digiEvent->getRunId();

      if ( ievent == 0 ) { ;}
      else if ( ievent % 100000 == 0 ) { std::cout << 'x' << std::endl; }
      else if ( ievent % 10000 == 0 ) { std::cout << 'x' << std::flush; }
      else if ( ievent % 1000 == 0 ) { std::cout << '.' << std::flush; }

      if( calType() == GAIN) getFitDir();
      
      if( calType() == GAIN && failCuts() ) continue;
 
      digiAcd();

    }
        
  }  // end analysis code in event loop    
}

Bool_t muonCalib::attachChains() {
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
    m_meritChain->SetBranchAddress("VtxZDir", &m_reconDirZ);
    // activate desired branches
    m_meritChain->SetBranchStatus("VtxZDir", 1);
  }
  return kTRUE;
}

void muonCalib::getFitDir()
{

    if ( fabs(m_reconDirZ) < 0.0001 ) m_reconDirZ = -9999;

}

Bool_t muonCalib::failCuts() 
{
  if(m_reconDirZ < -9990) return true;
  else return false;
}

