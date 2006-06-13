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
   m_hitStrip(0),
   m_vetoStrip(0),
   m_peds(0){

  Float_t lowBin = -0.5; 
  Float_t hiBin = (Float_t)nBins;
  hiBin -= 0.5;
  m_phaStrip = bookHists(TIME_PROF_PHA,nBins,lowBin,hiBin);
  m_hitStrip = bookHists(TIME_PROF_HIT,nBins,lowBin,hiBin);
  m_vetoStrip = bookHists(TIME_PROF_VETO,nBins,lowBin,hiBin);
   
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

void AcdStripChart::accumulate(int /* ievent */, const AcdDigi& digi) {

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
    m_vals[keyA].insert(redPha_A);
  }
  if ( rng1 == 0 ) {
    m_vals[keyB].insert(redPha_B);
  }
}


Bool_t AcdStripChart::readEvent(int ievent, Bool_t& filtered, 
				int& runId, int& evtId) {

  // set up a could of things of the first event
  if ( m_nEvtPerBin == 0 ) {
    UInt_t nEvts = (UInt_t)(last());
    m_nEvtPerBin = nEvts / m_nBins;
    UInt_t remainder = nEvts % m_nBins;
    std::cout << "Events: " << last() << ".  Bins:  " << m_nBins << ".  Per bin: " << m_nEvtPerBin << std::endl;
    std::cout << "Will drop last " << remainder << " events to make bins even." << std::endl;
  }

  // figure out which bin we are currently in
  m_currentCount = ievent % m_nEvtPerBin;
  m_currentBin = ievent / m_nEvtPerBin;

  // if this is the first event in new bin, reset the value cache
  if ( m_currentCount == 0 ) {
    m_vals.clear();
  }

  // ok, grab the event
  if(m_digiEvent) m_digiEvent->Clear();

  filtered = kFALSE;
  if(m_digiChain) { 
    m_digiChain->GetEvent(ievent);
    evtId = m_digiEvent->getEventId(); 
    runId = m_digiEvent->getRunId();
    // use only periodic & externals for later processing
    if ( m_digiEvent->getGem().getConditionSummary() != 32 &&  
	 m_digiEvent->getGem().getConditionSummary() != 128 )  {

      // disable further processing
      filtered = kTRUE;

      // but do grab the hitmap and vetos for the non-pedestal events      
      const TObjArray* acdDigiCol = m_digiEvent->getAcdDigiCol();
      if (!acdDigiCol) return kFALSE;
      
      Int_t iEvt = (Int_t) ( m_digiChain->GetReadEntry() );

      UInt_t nDigi =  acdDigiCol->GetEntries();
      for(UInt_t i = 0; i != nDigi; i++ ) {
	const AcdDigi* acdDigi = static_cast<const AcdDigi*>(acdDigiCol->At(i));   
	assert(acdDigi != 0); 
	int acdId = acdDigi->getId().getId();
	// ignore NA channels
	if ( acdId == 899 ) continue;
	if ( acdDigi->getAcceptMapBit( AcdDigi::A ) ) {
	  fillHist(*m_hitStrip,acdId,AcdDigi::A,(Float_t)m_currentBin);
	}
	if ( acdDigi->getAcceptMapBit( AcdDigi::B ) ) {
	  fillHist(*m_hitStrip,acdId,AcdDigi::B,(Float_t)m_currentBin);
	}
	if ( acdDigi->getVeto( AcdDigi::A ) ) {
	  fillHist(*m_vetoStrip,acdId,AcdDigi::A,(Float_t)m_currentBin);
	}
	if ( acdDigi->getVeto( AcdDigi::B ) ) {
	  fillHist(*m_vetoStrip,acdId,AcdDigi::B,(Float_t)m_currentBin);
	}
      }
    }
  }
  
  // last event in bin, get mean and rms of PHA distribution for every channel
  if ( m_currentCount == ( m_nEvtPerBin-1 ) ) {
    for ( std::map<UInt_t,std::set<Double_t> >::const_iterator itr = m_vals.begin();
	  itr != m_vals.end(); itr++ ) {
      UInt_t fillkey = itr->first;
      const std::set<Double_t> theVals = itr->second;
      Double_t n = (Double_t)(theVals.size());
      Double_t truncate = n * 0.2;
      Double_t x(0.);
      Double_t x2(0.);
      Double_t nn(0.);      
      Int_t i(0);
      for ( std::set<Double_t>::const_iterator itrSet = theVals.begin(); itrSet != theVals.end(); itrSet++, i++ ) {	
	if ( i > truncate && (i < (n - truncate) ) ) {
	  nn += 1.;
	  x += *itrSet;
	  x2 += (*itrSet)*(*itrSet);
	}
      }
      Double_t mean = x / nn;
      Double_t rms2 = (x2/nn) - (mean*mean);
      Double_t rms = sqrt(rms2);
      UInt_t id = AcdMap::getId(fillkey);
      UInt_t pmt = AcdMap::getPmt(fillkey);
      // got in, fill the histogram
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

