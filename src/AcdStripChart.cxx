#include "AcdStripChart.h"

#include "TChain.h"
#include "TH1F.h"

#include "AcdHistCalibMap.h"
#include "AcdCalibMap.h"
#include "AcdCalibUtil.h"

#include "digiRootData/DigiEvent.h"

#include <cassert>
#include <cmath>
#include <ctime>
#include <cstdio>
#include <ctime>
#include <fstream>

using std::cout;
using std::cerr;
using std::endl;
using std::string;

AcdStripChart::AcdStripChart(TChain* digiChain, UInt_t nBins, const char*  timeStampFile)
  :AcdCalibBase(AcdCalibData::TIME_PROF),
   m_nBins(nBins),
   m_nEvtPerBin(0),
   m_digiEvent(0),
   m_phaStrip(0),
   m_hitStrip(0),
   m_vetoStrip(0),
   m_timeStampLog(0){

  setChain(AcdCalib::DIGI,digiChain);

  Float_t lowBin = -0.5; 
  Float_t hiBin = (Float_t)nBins;
  hiBin -= 0.5;
  m_phaStrip = bookHists(AcdCalib::H_TIME_PHA,nBins,lowBin,hiBin);
  m_hitStrip = bookHists(AcdCalib::H_TIME_HIT,nBins,lowBin,hiBin);
  m_vetoStrip = bookHists(AcdCalib::H_TIME_VETO,nBins,lowBin,hiBin);
   
  Bool_t ok = attachChains();
  if ( ! ok ) {
    cerr << "ERR:  Failed to attach to input chains."  << endl;
  }

  m_timeStampLog = new ofstream(timeStampFile);
}


AcdStripChart::~AcdStripChart() 
{
  if (m_digiEvent) delete m_digiEvent;
  if (m_timeStampLog) {
    //m_timeStampLog->close();
    delete m_timeStampLog;
  }
}

Bool_t AcdStripChart::attachChains() {
  TChain* digiChain = getChain(AcdCalib::DIGI);
  if (digiChain != 0) {
    m_digiEvent = 0;
    digiChain->SetBranchAddress("DigiEvent", &m_digiEvent);
    digiChain->SetBranchStatus("*",0);  // disable all branches
    // activate desired brances
    digiChain->SetBranchStatus("m_acd*",1);
    digiChain->SetBranchStatus("m_eventId", 1); 
    digiChain->SetBranchStatus("m_runId", 1);
    digiChain->SetBranchStatus("m_gem", 1);
    digiChain->SetBranchStatus("m_metaEvent", 1);
  }
  
  return kTRUE;
}

void AcdStripChart::accumulate(int /* ievent */, const AcdDigi& digi) {

  int rng0 = digi.getRange(AcdDigi::A);    
  int pmt0 = digi.getPulseHeight(AcdDigi::A);
  
  int rng1 = digi.getRange(AcdDigi::B);
  int pmt1 = digi.getPulseHeight(AcdDigi::B);

  int id = digi.getId().getId();
  
  UInt_t keyA = AcdKey::makeKey(AcdDigi::A,id);
  UInt_t keyB = AcdKey::makeKey(AcdDigi::B,id);

  Float_t redPha_A = ((Float_t)(pmt0));
  Float_t redPha_B = ((Float_t)(pmt1));
  
 
  float pedA = getPeds(keyA);
  float pedB = getPeds(keyB);
  
  if ( pedA < 0 || pedB < 0 ) return;
  redPha_A -= pedA;
  redPha_B -= pedB;

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
    UInt_t nEvts = (UInt_t)( eventStats().last());
    m_nEvtPerBin = nEvts / m_nBins;
    UInt_t remainder = nEvts % m_nBins;
    std::cout << "Events: " << eventStats().last() << ".  Bins:  " << m_nBins << ".  Per bin: " << m_nEvtPerBin << std::endl;
    std::cout << "Will drop last " << remainder << " events to make bins even." << std::endl;
  }

  // figure out which bin we are currently in
  m_currentCount = ievent % m_nEvtPerBin;
  m_currentBin = ievent / m_nEvtPerBin;

  // Time from Mission elapsed time to Unix time and then from PDT to GMT:
  static const int METtoGMT = 978307200 + 25200;

  // if this is the first event in new bin, reset the value cache
  if ( m_currentCount == 0 ) {
    m_vals.clear();
    if ( m_timeStampLog ) {
      UInt_t timeStamp = m_digiEvent->getMetaEvent().time().current().timeSecs();
      UInt_t time = timeStamp + METtoGMT;
      *m_timeStampLog << m_currentBin+1 << ' ' << ctime((time_t*) (&time));
    }
  }

  // ok, grab the event
  if(m_digiEvent) m_digiEvent->Clear();
  TChain* digiChain = getChain(AcdCalib::DIGI);

  filtered = kFALSE;
  if(digiChain) { 
    digiChain->GetEvent(ievent);
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
      
      //Int_t iEvt = (Int_t) ( digiChain->GetReadEntry() );

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
    for ( std::map<UInt_t,std::multiset<Double_t> >::const_iterator itr = m_vals.begin();
	  itr != m_vals.end(); itr++ ) {
      UInt_t fillkey = itr->first;
      const std::multiset<Double_t>& theVals = itr->second;
      Double_t n = (Double_t)(theVals.size());
      Double_t truncate = n * 0.2;
      Double_t x(0.);
      Double_t x2(0.);
      Double_t nn(0.);      
      Int_t i(0);
      for ( std::multiset<Double_t>::const_iterator itrSet = theVals.begin(); itrSet != theVals.end(); itrSet++, i++ ) {	
	if ( i > truncate && (i < (n - truncate) ) ) {
	  nn += 1.;
	  x += *itrSet;
	  x2 += (*itrSet)*(*itrSet);
	}
      }
      Double_t mean = x / nn;
      Double_t rms2 = (x2/nn) - (mean*mean);
      Double_t rms = sqrt(rms2);
      UInt_t id = AcdKey::getId(fillkey);
      UInt_t pmt = AcdKey::getPmt(fillkey);
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

  TChain* digiChain = getChain(AcdCalib::DIGI);
  Int_t iEvt = (Int_t) ( digiChain->GetReadEntry() );

  UInt_t nDigi =  acdDigiCol->GetEntries();
  for(UInt_t i = 0; i != nDigi; i++ ) {
    const AcdDigi* acdDigi = static_cast<const AcdDigi*>(acdDigiCol->At(i));   
    assert(acdDigi != 0);      
    accumulate(iEvt,*acdDigi);
    used = kTRUE;
  }
}

