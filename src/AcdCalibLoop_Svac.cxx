#include "AcdCalibLoop_Svac.h"

#include "TH1F.h"
#include "TChain.h"

#include "AcdHistCalibMap.h"
#include "AcdCalibUtil.h"
#include "AcdCalibMap.h"
#include "AcdRibbonFit.h"

#include "digiRootData/AcdDigi.h"

#include <cassert>
#include <cmath>



AcdCalibLoop_Svac::AcdCalibLoop_Svac(AcdCalibData::CALTYPE t, TChain* svacChain, Bool_t correctPathLength, AcdMap::Config config)
  :AcdCalibBase(t,config),
   m_correctPathLength(correctPathLength),
   m_gainHists(0){

  setChain(AcdCalib::SVAC,svacChain);
  
  switch ( t ) {
  case AcdCalibData::GAIN:
    m_gainHists = bookHists(AcdCalib::H_GAIN,256,-0.5,4095.5);
    break;
  case AcdCalibData::RIBBON:
    m_gainHists = bookHists(AcdCalib::H_RIBBONS,256,-0.5,4095.5,7);
    break;
  default:
    break;
  }
  
  if ( m_gainHists == 0 ) {
    std::cerr << "ERR:  Unknown calibration type." << t << std::endl;
  }

  Bool_t ok = attachChains();
  if ( ! ok ) {
    std::cerr << "ERR:  Failed to attach to input chains."  << std::endl;
  }
}


AcdCalibLoop_Svac::~AcdCalibLoop_Svac() 
{
}

Bool_t AcdCalibLoop_Svac::attachChains() {
  TChain* svacChain =getChain(AcdCalib::SVAC);
  if (svacChain != 0) {
    svacChain->SetBranchStatus("*",0);  // disable all branches
    // activate desired brances
    svacChain->SetBranchAddress("AcdPha",&(m_AcdPha[0][0]));
    svacChain->SetBranchStatus("AcdPha",1);

    svacChain->SetBranchAddress("AcdRange",&(m_AcdRange[0][0]));
    svacChain->SetBranchStatus("AcdRange", 1); 

    svacChain->SetBranchAddress("AcdNumTkrIntSec",&m_AcdNumTkrIntSec);
    svacChain->SetBranchStatus("AcdNumTkrIntSec", 1);

    svacChain->SetBranchAddress("AcdTkrIntSecTileId",&(m_AcdTkrIntSecTileId[0]));
    svacChain->SetBranchStatus("AcdTkrIntSecTileId", 1);    

    svacChain->SetBranchAddress("AcdTkrIntSecTkrIndex",&(m_AcdTkrIntSecTkrIndex[0]));
    svacChain->SetBranchStatus("AcdTkrIntSecTkrIndex", 1);    

    svacChain->SetBranchAddress("AcdTkrIntSecPathLengthInTile",&(m_AcdTkrIntSecPathLengthInTile[0]));
    svacChain->SetBranchStatus("AcdTkrIntSecPathLengthInTile", 1);

    if ( calType() == AcdCalibData::RIBBON ) {
      svacChain->SetBranchAddress("AcdRibbonCount",&(m_AcdRibbonCount));
      svacChain->SetBranchStatus("AcdRibbonCount", 1);   

      svacChain->SetBranchAddress("AcdTkrIntSecGlobalX",&(m_AcdTkrIntSecGlobalX[0]));
      svacChain->SetBranchStatus("AcdTkrIntSecGlobalX", 1);

      svacChain->SetBranchAddress("AcdTkrIntSecGlobalY",&(m_AcdTkrIntSecGlobalY[0]));
      svacChain->SetBranchStatus("AcdTkrIntSecGlobalY", 1);

      svacChain->SetBranchAddress("AcdTkrIntSecGlobalZ",&(m_AcdTkrIntSecGlobalZ[0]));
      svacChain->SetBranchStatus("AcdTkrIntSecGlobalZ", 1);    
    }
    
  }

  return kTRUE;
}

void AcdCalibLoop_Svac::fillGainHistCorrect(unsigned id, float pathLength, unsigned iISect) {

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

  float pedA = getPeds(keyA);
  float pedB = getPeds(keyB);
  if ( pedA < 0 || pedB < 0 ) {
    std::cerr << "No Pedestal " << keyA << ' ' << keyB << std::endl;
    return;
  }

  Float_t redPha_A = ((Float_t)(pmt0)) - pedA;
  Float_t redPha_B = ((Float_t)(pmt1)) - pedB;

  if ( m_correctPathLength ) {
    redPha_A /= pathFactor;
    redPha_B /= pathFactor;
  }

  ///Float_t redPha_A = ((Float_t)(pmt0));
  // Float_t redPha_B = ((Float_t)(pmt1));
  
  if ( calType() == AcdCalibData::GAIN ) {
    if ( rng0 == 0 ) fillHist(*m_gainHists,id,AcdDigi::A,redPha_A);
    if ( rng1 == 0 ) fillHist(*m_gainHists,id,AcdDigi::B,redPha_B);
  } else if ( calType() == AcdCalibData::RIBBON ) {
    Float_t localX = AcdRibbonFitLibrary::getLocalX(id,
						    m_AcdTkrIntSecGlobalX[iISect],
						    m_AcdTkrIntSecGlobalY[iISect],
						    m_AcdTkrIntSecGlobalZ[iISect]);
    Int_t ribbonBin = AcdRibbonFitLibrary::getBin(id,localX);
    if ( ribbonBin < 0 ) {
      std::cerr << "Couldn't bin ribbon " << id << ' ' 
		<< m_AcdTkrIntSecGlobalX[iISect] << ' ' 
		<< m_AcdTkrIntSecGlobalY[iISect] << ' ' 
		<< m_AcdTkrIntSecGlobalZ[iISect] << std::endl;
      return;
    }
    if ( rng0 == 0 ) fillHist(*m_gainHists,id,AcdDigi::A,redPha_A,ribbonBin);
    if ( rng1 == 0 ) fillHist(*m_gainHists,id,AcdDigi::B,redPha_B,ribbonBin);
  } else {
    std::cerr << "Unknown calibration type " << calType() << std::endl;
    return;
  }

}


Bool_t AcdCalibLoop_Svac::readEvent(int ievent, Bool_t& filtered, 
				   int& /* runId */, int& /*evtId*/) {
  
  TChain* svacChain =getChain(AcdCalib::SVAC);
  if(svacChain) { 
    svacChain->GetEvent(ievent);
  }

  if ( calType() == AcdCalibData::RIBBON && m_AcdRibbonCount == 0 ) {
    filtered = kTRUE;
  } else {
    filtered = kFALSE;
  }
  
  return kTRUE;
}


void AcdCalibLoop_Svac::useEvent(Bool_t& used) {

  used = kFALSE;
  
  for(int i = 0; i != m_AcdNumTkrIntSec; i++) {

    if ( i >= 20 ) continue;
    if ( m_AcdTkrIntSecTkrIndex[i] > 0 ) continue;

    used = kTRUE;
    unsigned id = m_AcdTkrIntSecTileId[i];
    float path = m_AcdTkrIntSecPathLengthInTile[i];

    fillGainHistCorrect(id,path,i);
  }

}
