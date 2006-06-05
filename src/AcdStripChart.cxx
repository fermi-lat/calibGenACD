#include <fstream>
#include "TH1F.h"
#include "TF1.h"
#include "AcdStripChart.h"

#include "AcdHistCalibMap.h"
#include "AcdPedestalFit.h"
#include "AcdGainFit.h"
#include "AcdCalibUtil.h"

#include "digiRootData/DigiEvent.h"
#include "reconRootData/ReconEvent.h"

#include <cassert>
#include <cmath>

using std::cout;
using std::cerr;
using std::endl;
using std::string;

ClassImp(AcdStripChart) ;

AcdStripChart::AcdStripChart(TChain* digiChain, UInt_t nBins)
  :AcdCalibBase(),
   m_nBins(nBins),
   m_nEvtPerBin(0),
   m_digiChain(digiChain),
   m_digiEvent(0),
   m_phaStrip(0),
   m_peds(0){

  Float_t lowBin = -0.5; 
  Float_t hiBin = (Float_t)nBins;
  hiBin -= 0.5;
  m_phaStrip = bookHists(TIME_PROF,nBins,lowBin,hiBin);

  Bool_t ok = attachChains();
  if ( ! ok ) {
    cerr << "ERR:  Failed to attach to input chains."  << endl;
  }
}


AcdStripChart::~AcdStripChart() 
{
  if (m_digiEvent) delete m_digiEvent;
}

Bool_t AcdStripChart::attachChains() {
  if (m_digiChain != 0) {
    m_digiEvent = 0;
    m_digiChain->SetBranchAddress("DigiEvent", &m_digiEvent);
    m_digiChain->SetBranchStatus("*",0);  // disable all branches
    // activate desired brances
    m_digiChain->SetBranchStatus("m_acd*",1);
    m_digiChain->SetBranchStatus("m_eventId", 1); 
    m_digiChain->SetBranchStatus("m_runId", 1);
    m_digiChain->SetBranchStatus("m_gem", 1);
  }
  
  return kTRUE;
}

void AcdStripChart::accumulate(int ievent, const AcdDigi& digi) {



  int rng0 = digi.getRange(AcdDigi::A);    
  int pmt0 = digi.getPulseHeight(AcdDigi::A);
  
  int rng1 = digi.getRange(AcdDigi::B);
  int pmt1 = digi.getPulseHeight(AcdDigi::B);

  int id = digi.getId().getId();
  
  UInt_t keyA = AcdMap::makeKey(AcdDigi::A,id);
  UInt_t keyB = AcdMap::makeKey(AcdDigi::B,id);

  Float_t redPha_A = ((Float_t)(pmt0));
  Float_t redPha_B = ((Float_t)(pmt1));
  
  if ( m_peds != 0 ) {
    AcdPedestalFitResult* pedRes_A = m_peds->find(keyA);
    AcdPedestalFitResult* pedRes_B = m_peds->find(keyB);

    if ( pedRes_A == 0 && pedRes_B == 0 ) return;

    redPha_A -= pedRes_A->mean();
    redPha_B -= pedRes_B->mean();
  }

  if ( rng0 == 0 ) {
    m_run_N[keyA] += 1.;
    m_run_X[keyA] += redPha_A;
    m_run_X2[keyA] += (redPha_A*redPha_A);
  }
  if ( rng1 == 0 ) {
    m_run_N[keyB] += 1.;
    m_run_X[keyB] += redPha_B;
    m_run_X2[keyB] += (redPha_B*redPha_B);
  }
}


Bool_t AcdStripChart::readEvent(int ievent, Bool_t& filtered, 
				int& runId, int& evtId) {

  if ( m_nEvtPerBin == 0 ) {
    UInt_t nEvts = (UInt_t)(last());
    m_nEvtPerBin = nEvts / m_nBins;
    UInt_t remainder = nEvts % m_nBins;
    std::cout << "Events: " << last() << ".  Bins:  " << m_nBins << ".  Per bin: " << m_nEvtPerBin << std::endl;
    std::cout << "Will drop last " << remainder << " events to make bins even." << std::endl;
  }

  m_currentCount = ievent % m_nEvtPerBin;
  m_currentBin = ievent / m_nEvtPerBin;
  if ( m_currentCount == 0 ) {
    m_run_N.clear();
    m_run_X.clear();
    m_run_X2.clear();
  }

  if(m_digiEvent) m_digiEvent->Clear();

  filtered = kFALSE;
  if(m_digiChain) { 
    m_digiChain->GetEvent(ievent);
    evtId = m_digiEvent->getEventId(); 
    runId = m_digiEvent->getRunId();
    if ( m_digiEvent->getGem().getConditionSummary() != 32 &&  
	 m_digiEvent->getGem().getConditionSummary() != 128 )  {
      filtered = kTRUE;
    }
  }
  
  // end of point, get mean and rms
  if ( m_currentCount == ( m_nEvtPerBin-1 ) ) {
    for ( std::map<UInt_t,Double_t>::const_iterator itr = m_run_N.begin();
	  itr != m_run_N.end(); itr++ ) {
      UInt_t fillkey = itr->first;
      Double_t n = itr->second;
      Double_t x = m_run_X[fillkey];
      Double_t x2 =  m_run_X2[fillkey];
      Double_t mean = x / n;
      Double_t rms2 = (x2/n) - (mean*mean);
      Double_t rms = sqrt(rms2);
      UInt_t id = AcdMap::getId(fillkey);
      UInt_t pmt = AcdMap::getPmt(fillkey);
      fillHistBin(*m_phaStrip,id,pmt == 0 ? AcdDigi::A : AcdDigi::B, m_currentBin, mean, rms);
    }
  }

  return kTRUE;
}


void AcdStripChart::useEvent(Bool_t& used) {

  used = kFALSE;
  
  const TObjArray* acdDigiCol = m_digiEvent->getAcdDigiCol();
  if (!acdDigiCol) return;

  Int_t iEvt = (Int_t) ( m_digiChain->GetReadEntry() );

  UInt_t nDigi =  acdDigiCol->GetEntries();
  for(UInt_t i = 0; i != nDigi; i++ ) {
    const AcdDigi* acdDigi = static_cast<const AcdDigi*>(acdDigiCol->At(i));   
    assert(acdDigi != 0);      
    accumulate(iEvt,*acdDigi);
    used = kTRUE;
  }
}

Bool_t AcdStripChart::readPedestals(const char* fileName) {
  Bool_t latchVal = readCalib(PEDESTAL,fileName);
  AcdCalibMap* map = getCalibMap(PEDESTAL);
  m_peds = (AcdPedestalFitMap*)(map);
  return latchVal;
}

void AcdStripChart::writeXmlSources(ostream& os) const {
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

void AcdStripChart::writeTxtSources(ostream& os) const {
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

