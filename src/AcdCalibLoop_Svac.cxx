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
#include <TMath.h>


AcdCalibLoop_Svac::AcdCalibLoop_Svac(AcdCalibData::CALTYPE t, TChain* svacChain, 
				     Bool_t correctPathLength, 
				     Bool_t calGCRSelect,
				     AcdKey::Config config)
  :AcdCalibBase(t,config),
   m_correctPathLength(correctPathLength),
   m_calGCRSelect(calGCRSelect),
   m_gainHists(0),
   m_rangeHists(0){

  setChain(AcdCalib::SVAC,svacChain);
  
  switch ( t ) {
  case AcdCalibData::GAIN:
    m_gainHists = bookHists(AcdCalib::H_GAIN,64,-0.5,2047.5);
    break;
  case AcdCalibData::RIBBON:
    m_gainHists = bookHists(AcdCalib::H_RIBBONS,256,-0.5,4095.5,7);
    break;
  case AcdCalibData::CARBON:
    m_gainHists = bookHists(AcdCalib::H_GAIN,64,-0.5,1023.5);
    break;
  case AcdCalibData::MERITCALIB:
    m_rangeHists = bookHists(AcdCalib::H_RANGE,100,-1.,3.,2);
    m_gainHists = bookHists(AcdCalib::H_GAIN,50,-0.0,2.5);
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

    if ( calType() == AcdCalibData::MERITCALIB ) {
      svacChain->SetBranchAddress("AcdMips",&(m_AcdMips[0][0]));
      svacChain->SetBranchStatus("AcdMips", 1); 	 
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

  UInt_t keyA = AcdKey::makeKey(AcdDigi::A,id);
  UInt_t keyB = AcdKey::makeKey(AcdDigi::B,id);

  Float_t redPha_A = ((Float_t)(pmt0));
  Float_t redPha_B = ((Float_t)(pmt1));
  
  if ( calType() == AcdCalibData::GAIN || 
       calType() == AcdCalibData::RIBBON ) {
    float pedA = getPeds(keyA);
    float pedB = getPeds(keyB);
    if ( pedA < 0 || pedB < 0 ) {
      std::cerr << "No Pedestal " << keyA << ' ' << keyB << std::endl;
      return;
    }
    redPha_A -= pedA;
    redPha_B -= pedB;
  } 

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
  } else if ( calType() == AcdCalibData::MERITCALIB ) {
    if ( !ribbonCut ) return;    
    Float_t mipsA = m_AcdMips[id][0];
    Float_t mipsB = m_AcdMips[id][1];
    if ( m_correctPathLength ) {
      mipsA /= pathFactor;
      mipsB /= pathFactor;
    }
    if ( rng0 == 0 ) fillHist(*m_gainHists,id,AcdDigi::A,mipsA);
    if ( rng1 == 0 ) fillHist(*m_gainHists,id,AcdDigi::B,mipsB);
    if ( !getMipValues(id,mipsA,mipsB) ) return;
    if ( rng0 == 0 ) {
      fillHist(*m_rangeHists,id,AcdDigi::A,TMath::Log10(mipsA),0);
    } else {
      fillHist(*m_rangeHists,id,AcdDigi::A,TMath::Log10(mipsA),1);
    }
    if ( rng1 == 0 ) {
      fillHist(*m_rangeHists,id,AcdDigi::B,TMath::Log10(mipsB),0);
    } else {
      fillHist(*m_rangeHists,id,AcdDigi::B,TMath::Log10(mipsB),1);
    }
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
    
    if ( calType() == AcdCalibData::RIBBON &&
	 id < 500 ) continue;

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
  //Float_t ratio21 = TMath::Abs((CalELayer[2]/CalELayer[1])-1.);
  //Float_t ratio32 = TMath::Abs((CalELayer[3]/CalELayer[2])-1.);
  
  if ( ratio10 > 0.5 ) return kFALSE;
  //if ( ratio21 > 0.3 ) return kFALSE;
  //if ( ratio32 > 0.3 ) return kFALSE;

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
  if ( range == 1 ) {
    Float_t peds = getPedsHigh(key);
    val = pha - peds;
  } else {
    val = -1.;
  }
  return kTRUE;
}


/// Get the MIP values
bool AcdCalibLoop_Svac::getMipValues(UInt_t id, Float_t& mipsA, Float_t& mipsB) {
  UInt_t keyA = AcdKey::makeKey(AcdDigi::A,id);
  UInt_t keyB = AcdKey::makeKey(AcdDigi::B,id);
  if ( ! getMipValue(keyA,m_AcdRange[id][0],m_AcdPha[id][0],mipsA) ) return false;
  if ( ! getMipValue(keyB,m_AcdRange[id][1],m_AcdPha[id][1],mipsB) ) return false;
  return true;

}
  
/// get the MIP value for one pmt
bool AcdCalibLoop_Svac::getMipValue(UInt_t key, UInt_t range, UInt_t pha, Float_t& mips) {
  if ( range == 0 ) {    
    const CalibData::AcdCalibObj* mip = getCalibMap(AcdCalibData::GAIN)->get(key);
    const CalibData::AcdCalibObj* ped = getCalibMap(AcdCalibData::PEDESTAL)->get(key);
    if ( mip == 0 || ped == 0 ) return false;
    mips = (pha - ped->operator[](0) ) / mip->operator[](0);
  } else {
    const CalibData::AcdCalibObj* calib = getCalibMap(AcdCalibData::HIGH_RANGE)->get(key);
    if ( calib == 0 ) return false;
    float pedRed = pha - calib->operator[](0);
    if ( pedRed < 0 ) {
      mips = 3.;
      return true;
    }
    float fromSat = calib->operator[](2) - pedRed;
    if ( fromSat < 50 ) {
      mips = 1000.;
      return true;
    }
    float top =  calib->operator[](2) * pedRed;
    float bot =  calib->operator[](1) * fromSat;
    mips = top/bot;
    if ( mips < 3.5 ) {
      mips = 3.5;
    } if ( mips > 1000) {
      mips = 1000;
    }
  }
  return true;
}
