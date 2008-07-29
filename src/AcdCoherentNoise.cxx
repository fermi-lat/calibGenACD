#include "AcdCoherentNoise.h"


#include "TH1F.h"
#include "TF1.h"
#include "TChain.h"

#include "AcdHistCalibMap.h"
#include "AcdCalibUtil.h"
#include "AcdCalibMap.h"

#include "digiRootData/DigiEvent.h"
#include "reconRootData/ReconEvent.h"

#include <cassert>
#include <cmath>
#include <cstdio>
#include <fstream>

using std::cout;
using std::cerr;
using std::endl;
using std::string;

AcdCoherentNoise::AcdCoherentNoise(TChain* digiChain, UInt_t loDT, UInt_t hiDT, UInt_t nBins, 
				   AcdKey::Config config)
  :AcdCalibBase(AcdCalibData::COHERENT_NOISE,config),
   m_loDT(loDT),
   m_hiDT(hiDT),
   m_nBins(nBins),
   m_binSize( ((float)(hiDT-loDT))/((float)(nBins)) ), 
   m_digiEvent(0),
   m_histMap(0) {

  setChain(AcdCalib::DIGI,digiChain);
  Float_t hiEdge = hiDT - loDT;
  m_histMap = bookHists(AcdCalib::H_COHERENT_NOISE,nBins,0,hiEdge);
  
  Bool_t ok = attachChains();
  if ( ! ok ) {
    cerr << "ERR:  Failed to attach to input chains."  << endl;
  }
  
}


AcdCoherentNoise::~AcdCoherentNoise() 
{
  if (m_digiEvent) delete m_digiEvent;
}

Bool_t AcdCoherentNoise::attachChains() {
  TChain* digiChain = getChain(AcdCalib::DIGI);
  if (digiChain != 0) {
    m_digiEvent = 0;
    digiChain->SetBranchAddress("DigiEvent", &m_digiEvent);
    digiChain->SetBranchStatus("*",0);  // disable all branches
    // activate desired brances
    digiChain->SetBranchStatus("m_acd*",1);
    digiChain->SetBranchStatus("m_eventId", 1); 
    digiChain->SetBranchStatus("m_runId", 1);
    digiChain->SetBranchStatus("m_timeStamp", 1);
    digiChain->SetBranchStatus("m_gem", 1);
  }
  
  return kTRUE;
}

void AcdCoherentNoise::fillHistograms() {

  // first loop is over channels
  for ( std::map<UInt_t, AcdBinDataMap >::const_iterator itr = m_vals.begin(); 
	itr != m_vals.end(); itr++ ) {
    UInt_t key = itr->first;    
    const std::vector< std::multiset<Double_t> >& data = itr->second.val();
    UInt_t idx(0);
    for ( std::vector< std::multiset<Double_t> >::const_iterator itrB = data.begin();
	  itrB != data.end(); itrB++ ) {
      idx++;
      const std::multiset<Double_t>& theVals = *itrB;
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
      if ( nn < 2 ) continue;
      Double_t mean = x / nn;
      Double_t rms2 = (x2/nn) - (mean*mean);
      Double_t rms = sqrt(rms2);
      UInt_t id = AcdKey::getId(key);
      UInt_t pmt = AcdKey::getPmt(key);
      // got it, fill the histogram
      fillHistBin(*m_histMap,id,pmt == 0 ? AcdDigi::A : AcdDigi::B, idx, mean, rms);      
    }    
  }
}

void AcdCoherentNoise::accumulate(Int_t deltaT, const AcdDigi& digi) {

  Float_t dt = (Float_t)(deltaT - m_loDT);
  if ( dt <= 0 ) return;
  dt /= m_binSize;
  Int_t bin = (int)(dt);
  if ( bin >= (int)m_nBins ) return;

  int rng0 = digi.getRange(AcdDigi::A);    
  int pmt0 = digi.getPulseHeight(AcdDigi::A);
  
  int rng1 = digi.getRange(AcdDigi::B);
  int pmt1 = digi.getPulseHeight(AcdDigi::B);

  int id = digi.getId().getId();
  if ( id > 700 ) return;
  
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
    std::map<UInt_t, AcdBinDataMap >::iterator itrFindA = m_vals.find(keyA);
    if ( itrFindA == m_vals.end() ) {
      m_vals[keyA] = AcdBinDataMap(m_nBins);
      itrFindA = m_vals.find(keyA);
    }
    (itrFindA->second).val()[bin].insert(redPha_A);
  }
  if ( rng1 == 0 ) {    
    std::map<UInt_t, AcdBinDataMap >::iterator itrFindB = m_vals.find(keyB);
    if ( itrFindB == m_vals.end() ) {
      m_vals[keyB] = AcdBinDataMap(m_nBins);
      itrFindB = m_vals.find(keyB);
    }
    (itrFindB->second).val()[bin].insert(redPha_B);
  }
}


Bool_t AcdCoherentNoise::readEvent(int ievent, Bool_t& filtered, 
				   int& runId, int& evtId, Double_t& timestamp) {

  // ok, grab the event
  if(m_digiEvent) m_digiEvent->Clear();
  
  filtered = kFALSE;
  TChain* digiChain = getChain(AcdCalib::DIGI);
  if(digiChain) { 
    digiChain->GetEvent(ievent);
    evtId = m_digiEvent->getEventId(); 
    runId = m_digiEvent->getRunId();
    timestamp = m_digiEvent->getTimeStamp();

    // use only periodic & externals for later processing
    if ( m_digiEvent->getGem().getConditionSummary() != 32 &&  
	 m_digiEvent->getGem().getConditionSummary() != 128 )  {
      
      // disable further processing
      filtered = kTRUE;
    }
  }
  
  return kTRUE;
}


void AcdCoherentNoise::useEvent(Bool_t& used) {

  used = kFALSE;
  
  const TObjArray* acdDigiCol = m_digiEvent->getAcdDigiCol();
  if (!acdDigiCol) return;
  
  Int_t deltaT = m_digiEvent->getGem().getDeltaEventTime();

  UInt_t nDigi =  acdDigiCol->GetEntries();
  for(UInt_t i = 0; i != nDigi; i++ ) {
    const AcdDigi* acdDigi = static_cast<const AcdDigi*>(acdDigiCol->At(i));   
    assert(acdDigi != 0);    
    accumulate(deltaT,*acdDigi);
    used = kTRUE;
  }
}
