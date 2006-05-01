#include <fstream>
#include "TH1F.h"
#include "TF1.h"
#include "AcdMuonSvacCalib.h"

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

ClassImp(AcdMuonSvacCalib) ;

AcdMuonSvacCalib::AcdMuonSvacCalib(TChain* svacChain, Bool_t correctPathLength)
  :AcdCalibBase(),
   m_correctPathLength(correctPathLength),
   m_svacChain(svacChain),
   m_gainHists(0),
   m_gains(0),
   m_peds(0){

  m_gainHists = bookHists(GAIN,256,-0.5,4095.5);
  
  Bool_t ok = attachChains();
  if ( ! ok ) {
    cerr << "ERR:  Failed to attach to input chains."  << endl;
  }
}


AcdMuonSvacCalib::~AcdMuonSvacCalib() 
{
}

Bool_t AcdMuonSvacCalib::attachChains() {
  if (m_svacChain != 0) {
    m_svacChain->SetBranchStatus("*",0);  // disable all branches
    // activate desired brances
    m_svacChain->SetBranchAddress("AcdPha",&(m_AcdPha[0][0]));
    m_svacChain->SetBranchStatus("AcdPha",1);

    m_svacChain->SetBranchAddress("AcdRange",&(m_AcdRange[0][0]));
    m_svacChain->SetBranchStatus("AcdRange", 1); 

    m_svacChain->SetBranchAddress("AcdNumTkrIntSec",&m_AcdNumTkrIntSec);
    m_svacChain->SetBranchStatus("AcdNumTkrIntSec", 1);

    m_svacChain->SetBranchAddress("AcdTkrIntSecTileId",&(m_AcdTkrIntSecTileId[0]));
    m_svacChain->SetBranchStatus("AcdTkrIntSecTileId", 1);    

    m_svacChain->SetBranchAddress("AcdTkrIntSecTkrIndex",&(m_AcdTkrIntSecTkrIndex[0]));
    m_svacChain->SetBranchStatus("AcdTkrIntSecTkrIndex", 1);    

    m_svacChain->SetBranchAddress("AcdTkrIntSecPathLengthInTile",&(m_AcdTkrIntSecPathLengthInTile[0]));
    m_svacChain->SetBranchStatus("AcdTkrIntSecPathLengthInTile", 1);
    
  }

  return kTRUE;
}

void AcdMuonSvacCalib::fillGainHistCorrect(unsigned id, float pathLength) {

  int rng0 = m_AcdRange[id][AcdDigi::A];
  int pmt0 = m_AcdPha[id][AcdDigi::A];
  
  int rng1 = m_AcdRange[id][AcdDigi::B];
  int pmt1 = m_AcdPha[id][AcdDigi::B];
 
  float width = AcdCalibUtil::width(id);
  float pathFactor = pathLength / width;
  
  // require that track not clip edge of tile
  if ( id < 500 && pathFactor < 0.99 ) return;
  if ( m_correctPathLength ) {
    // correcting for pathlength, go ahead and take stuff up to 1/cos_th = 1.5
    if ( pathFactor > 1.5 ) return;
  } else {
    // not correcting for pathlength, only take stuff up to 1/cos_th = 1.2
    if ( pathFactor > 1.2 ) return;
  }  

  UInt_t keyA = AcdMap::makeKey(AcdDigi::A,id);
  UInt_t keyB = AcdMap::makeKey(AcdDigi::B,id);

  AcdPedestalFitResult* pedRes_A = m_peds->find(keyA);
  AcdPedestalFitResult* pedRes_B = m_peds->find(keyB);

  if ( pedRes_A == 0 && pedRes_B == 0 ) return;

  Float_t redPha_A = ((Float_t)(pmt0)) - pedRes_A->mean();
  Float_t redPha_B = ((Float_t)(pmt1)) - pedRes_B->mean();

  if ( m_correctPathLength ) {
    redPha_A /= pathFactor;
    redPha_B /= pathFactor;
  }

  ///Float_t redPha_A = ((Float_t)(pmt0));
  // Float_t redPha_B = ((Float_t)(pmt1));

  if ( rng0 == 0 ) fillHist(*m_gainHists,id,AcdDigi::A,redPha_A);
  if ( rng1 == 0 ) fillHist(*m_gainHists,id,AcdDigi::B,redPha_B);

}


Bool_t AcdMuonSvacCalib::readEvent(int ievent, Bool_t& filtered, 
				   int& /* runId */, int& /*evtId*/) {
  
  if(m_svacChain) { 
    m_svacChain->GetEvent(ievent);
  }
  filtered = kFALSE;
  
  return kTRUE;
}


void AcdMuonSvacCalib::useEvent(Bool_t& used) {

  used = kFALSE;
  
  for(int i = 0; i != m_AcdNumTkrIntSec; i++) {

    if ( m_AcdTkrIntSecTkrIndex[i] > 0 ) continue;

    unsigned id = m_AcdTkrIntSecTileId[i];
    float path = m_AcdTkrIntSecPathLengthInTile[i];

    fillGainHistCorrect(id,path);
  }
}

AcdGainFitMap* AcdMuonSvacCalib::fitGains(AcdGainFit& fitter) {
  m_gains = new AcdGainFitMap;
  addCalibration(GAIN,*m_gains);
  AcdHistCalibMap* hists = getHistMap(GAIN);
  fitter.fitAll(*m_gains,*hists);
  return m_gains;
}  

Bool_t AcdMuonSvacCalib::readPedestals(const char* fileName) {
  Bool_t latchVal = readCalib(PEDESTAL,fileName);
  AcdCalibMap* map = getCalibMap(PEDESTAL);
  m_peds = (AcdPedestalFitMap*)(map);
  return latchVal;
}

void AcdMuonSvacCalib::writeXmlSources(ostream& os) const {
  std::string pedFileName;
  if ( m_peds != 0 ) pedFileName +=  m_peds->fileName();
  os << "pedestalFile=" << pedFileName << std::endl;
  TObjArray* files = m_svacChain != 0 ? m_svacChain->GetListOfFiles() : 0;
  if ( files != 0 ) {
    for ( Int_t i(0); i < files->GetEntriesFast(); i++ ) {
      TObject* obj = files->At(i);
      os << "inputSvac=" << obj->GetTitle() << std::endl;
    }
  }
}

void AcdMuonSvacCalib::writeTxtSources(ostream& os) const {
  std::string pedFileName;
  if ( m_peds != 0 ) pedFileName +=  m_peds->fileName();
  os << "#pedestalFile = " << pedFileName << endl;
  TObjArray* files = m_svacChain != 0 ? m_svacChain->GetListOfFiles() : 0;
  if ( files != 0 ) {
    for ( Int_t i(0); i < files->GetEntriesFast(); i++ ) {
      TObject* obj = files->At(i);
      os << "#inputSvacFile = " << obj->GetTitle() << endl;
    }
  }
}

