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



AcdCalibLoop_Svac::AcdCalibLoop_Svac(AcdCalibData::CALTYPE t, TChain* svacChain, 
				     Bool_t correctPathLength, 
				     Bool_t calGCRSelect,
				     AcdMap::Config config)
  :AcdCalibBase(t,config),
   m_correctPathLength(correctPathLength),
   m_calGCRSelect(calGCRSelect),
   m_gainHists(0){

  setChain(AcdCalib::SVAC,svacChain);
  
  switch ( t ) {
  case AcdCalibData::GAIN:
    m_gainHists = bookHists(AcdCalib::H_GAIN,256,-0.5,4095.5);
    break;
  case AcdCalibData::RIBBON:
    m_gainHists = bookHists(AcdCalib::H_RIBBONS,256,-0.5,4095.5,7);
    break;
  case AcdCalibData::CARBON:
    m_gainHists = bookHists(AcdCalib::H_GAIN,64,-0.5,1023.5);
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

    svacChain->SetBranchAddress("EvtTime",&m_timeStamp);
    svacChain->SetBranchStatus("EvtTime",1);

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

    svacChain->SetBranchAddress("AcdTkrIntSecGlobalX",&(m_AcdTkrIntSecGlobalX[0]));
    svacChain->SetBranchStatus("AcdTkrIntSecGlobalX", 1);
    
    svacChain->SetBranchAddress("AcdTkrIntSecGlobalY",&(m_AcdTkrIntSecGlobalY[0]));
    svacChain->SetBranchStatus("AcdTkrIntSecGlobalY", 1);
    
    svacChain->SetBranchAddress("AcdTkrIntSecGlobalZ",&(m_AcdTkrIntSecGlobalZ[0]));
    svacChain->SetBranchStatus("AcdTkrIntSecGlobalZ", 1);    

    if ( calType() == AcdCalibData::RIBBON ) {
      svacChain->SetBranchAddress("AcdRibbonCount",&(m_AcdRibbonCount));
      svacChain->SetBranchStatus("AcdRibbonCount", 1);   

    }

    if ( calType() == AcdCalibData::CARBON && m_calGCRSelect ) {
      svacChain->SetBranchAddress("TrgEngineGem",&(m_TrgEngineGem));
      svacChain->SetBranchStatus("TrgEngineGem", 1);    

      svacChain->SetBranchAddress("Tkr1ZDir",&(m_Tkr1ZDir));
      svacChain->SetBranchStatus("Tkr1ZDir", 1);    

      svacChain->SetBranchAddress("CalXtalEne",&(m_CalXtalEne[0][0][0]));
      svacChain->SetBranchStatus("CalXtalEne", 1);          
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
  
  if ( m_correctPathLength ) {
    // require that track not clip edge of tile
    if ( id < 500 && pathFactor < 0.99 ) return;    
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
  
  // only take stuff in the middle of ribbons
  Int_t ribbonBin(-1);
  Bool_t ribbonCut(kTRUE);
  if ( id >= 500 ) {
    Float_t localX = AcdRibbonFitLibrary::getLocalX(id,
						    m_AcdTkrIntSecGlobalX[iISect],
						    m_AcdTkrIntSecGlobalY[iISect],
						    m_AcdTkrIntSecGlobalZ[iISect]);
    ribbonBin = AcdRibbonFitLibrary::getBin(id,localX);
    if ( ribbonBin != 3) ribbonCut = kFALSE;
  }

  if ( calType() == AcdCalibData::GAIN ) {
    if ( !ribbonCut ) return;
    if ( rng0 == 0 ) fillHist(*m_gainHists,id,AcdDigi::A,redPha_A);
    if ( rng1 == 0 ) fillHist(*m_gainHists,id,AcdDigi::B,redPha_B);
  } else if ( calType() == AcdCalibData::RIBBON ) {
    if ( ribbonBin < 0 ) {
      std::cerr << "Couldn't bin ribbon " << id << ' ' 
		<< m_AcdTkrIntSecGlobalX[iISect] << ' ' 
		<< m_AcdTkrIntSecGlobalY[iISect] << ' ' 
		<< m_AcdTkrIntSecGlobalZ[iISect] << std::endl;
      return;
    }
    if ( rng0 == 0 ) fillHist(*m_gainHists,id,AcdDigi::A,redPha_A,ribbonBin);
    if ( rng1 == 0 ) fillHist(*m_gainHists,id,AcdDigi::B,redPha_B,ribbonBin);
  } else if (  calType() == AcdCalibData::CARBON ) {
    if ( !ribbonCut ) return;
    if ( ! singleRange(keyA,pmt0,rng0,redPha_A ) ) return;
    if ( ! singleRange(keyB,pmt1,rng1,redPha_B ) ) return;
    if ( m_correctPathLength ) {
      redPha_A /= pathFactor;
      redPha_B /= pathFactor;
    }
    if ( redPha_A > 0 ) fillHist(*m_gainHists,id,AcdDigi::A,redPha_A);
    if ( redPha_B > 0 ) fillHist(*m_gainHists,id,AcdDigi::B,redPha_B);
    
  } else {
    std::cerr << "Unknown calibration type " << calType() << std::endl;
    return;
  }

}


Bool_t AcdCalibLoop_Svac::readEvent(int ievent, Bool_t& filtered, 
				   int& /* runId */, int& /*evtId*/, Double_t& timeStamp) {
  
  TChain* svacChain =getChain(AcdCalib::SVAC);
  if(svacChain) { 
    svacChain->GetEvent(ievent);
  }
  timeStamp = m_timeStamp;
  
  if ( calType() == AcdCalibData::RIBBON && m_AcdRibbonCount == 0 ) {
    filtered = kTRUE;
  } else {
    filtered = kFALSE;
  }
  
  return kTRUE;
}


void AcdCalibLoop_Svac::useEvent(Bool_t& used) {

  used = kFALSE;

  if (  calType() == AcdCalibData::CARBON ) {
    if ( m_calGCRSelect && !runGCRSelect() ) return;
  }
  
  for(int i = 0; i != m_AcdNumTkrIntSec; i++) {

    if ( i >= 20 ) continue;
    if ( m_AcdTkrIntSecTkrIndex[i] > 0 ) continue;

    used = kTRUE;
    unsigned id = m_AcdTkrIntSecTileId[i];
    float path = m_AcdTkrIntSecPathLengthInTile[i];

    fillGainHistCorrect(id,path,i);
  }

}



Bool_t AcdCalibLoop_Svac::runGCRSelect() {
  //if ( m_TrgEngineGem !=4 ) return kFALSE;

  Float_t CalELayer[4] = {0.,0.,0.,0.};

  for ( Int_t iTow(0); iTow < 16; iTow++) {
    for ( Int_t iLay(0); iLay < 4; iLay++ ) {
      for ( Int_t iLog(0); iLog < 12; iLog++ ) {
	CalELayer[iLay] += m_CalXtalEne[iTow][iLay][iLog];
      }
    }
  }

  Float_t normCutLay0 = -m_Tkr1ZDir*CalELayer[0];
  if ( normCutLay0 < 450 || normCutLay0 > 650 ) return kFALSE;
  if ( CalELayer[1] <= 0. || 
       CalELayer[2] <= 0. ) return kFALSE;
  
  Float_t ratio10 = TMath::Abs((CalELayer[1]/CalELayer[0])-1.);
  Float_t ratio21 = TMath::Abs((CalELayer[2]/CalELayer[1])-1.);
  Float_t ratio32 = TMath::Abs((CalELayer[3]/CalELayer[2])-1.);
  
  if ( ratio10 > 0.3 ) return kFALSE;
  if ( ratio21 > 0.3 ) return kFALSE;
  if ( ratio32 > 0.3 ) return kFALSE;

  return kTRUE;  
}


/// Get the range cross-over data
Bool_t AcdCalibLoop_Svac::getRangeData(UInt_t key, Float_t& low_max, Float_t& high_min) const {
  const AcdCalibMap* ranges = getCalibMap(AcdCalibData::RANGE);
  if ( ranges == 0 ) return kFALSE;
  const CalibData::AcdCalibObj * rangeRes = ranges->get(key);
  if ( rangeRes == 0 ) return kFALSE;
  low_max = (*rangeRes)[0];
  high_min = (*rangeRes)[1];
  return kTRUE;
}


/// get the high range pedestal for a channel
float AcdCalibLoop_Svac::getPedsHigh(UInt_t key) const {
  const AcdCalibMap* peds = getCalibMap(AcdCalibData::PED_HIGH);
  if ( peds == 0 ) return -1;
  const CalibData::AcdCalibObj * pedRes = peds->get(key);
  if ( pedRes == 0 ) return -2;
  return (*pedRes)[0];  
}


/// Transform the data into a single range
Bool_t AcdCalibLoop_Svac::singleRange(UInt_t key, Int_t pha, Int_t range, Float_t& val) const {
  Float_t low_max(0);
  Float_t high_min(0);
  if ( range == 1 ) {
    if ( ! getRangeData(key,low_max,high_min) ){
      return kFALSE;
    }
    val = pha - high_min;
    val += 40;
  } else {
    Float_t peds = getPeds(key);
    if ( peds < 0 ) return kFALSE;
    val = pha - peds;
    val /= 100.;
  }
  return kTRUE;
}
