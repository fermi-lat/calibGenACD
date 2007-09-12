#include <fstream>
#include "TH1F.h"
#include "TF1.h"
#include "AcdCoherentNoise.h"

#include "AcdHistCalibMap.h"
#include "AcdPedestalFit.h"
#include "AcdGainFit.h"
#include "AcdCalibUtil.h"

#include "digiRootData/DigiEvent.h"
#include "reconRootData/ReconEvent.h"

#include <cassert>
#include <cmath>
#include <cstdio>

using std::cout;
using std::cerr;
using std::endl;
using std::string;

ClassImp(AcdCoherentNoise) ;

AcdCoherentNoise::AcdCoherentNoise(TChain* digiChain, UInt_t loDT, UInt_t hiDT, UInt_t nBins, 
				   AcdMap::Config config)
  :AcdCalibBase(AcdCalibBase::COHERENT_NOISE,config),
   m_loDT(loDT),
   m_hiDT(hiDT),
   m_nBins(nBins),
   m_binSize( ((float)(hiDT-loDT))/((float)(nBins)) ), 
   m_digiChain(digiChain),
   m_digiEvent(0),
   m_histMap(0),
   m_peds(0){

  m_histMap = bookHists(COHERENT_NOISE,nBins,(Float_t)loDT,(Float_t)hiDT);
  
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
      UInt_t id = AcdMap::getId(key);
      UInt_t pmt = AcdMap::getPmt(key);
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
  if ( bin >= m_nBins ) return;

  int rng0 = digi.getRange(AcdDigi::A);    
  int pmt0 = digi.getPulseHeight(AcdDigi::A);
  
  int rng1 = digi.getRange(AcdDigi::B);
  int pmt1 = digi.getPulseHeight(AcdDigi::B);

  int id = digi.getId().getId();
  if ( id > 700 ) return;
  
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
				   int& runId, int& evtId) {

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

Bool_t AcdCoherentNoise::readPedestals(const char* fileName) {
  Bool_t latchVal = readCalib(PEDESTAL,fileName);
  AcdCalibMap* map = getCalibMap(PEDESTAL);
  m_peds = (AcdPedestalFitMap*)(map);
  return latchVal;
}

void AcdCoherentNoise::writeXmlSources( DomElement& node) const {
  //std::string pedFileName;
  //if ( m_peds != 0 ) pedFileName +=  m_peds->fileName();
  //os << "pedestalFile=" << pedFileName << std::endl;
  //TObjArray* files = m_digiChain != 0 ? m_digiChain->GetListOfFiles() : 0;
  //if ( files != 0 ) {
  //  for ( Int_t i(0); i < files->GetEntriesFast(); i++ ) {
  //    TObject* obj = files->At(i);
  //    os << "inputDigi=" << obj->GetTitle() << std::endl;
  //  }
  //}
}

void AcdCoherentNoise::writeTxtSources(ostream& os) const {
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

