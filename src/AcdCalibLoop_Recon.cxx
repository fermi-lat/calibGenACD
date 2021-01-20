
#include "AcdCalibLoop_Recon.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TChain.h"

#include "AcdHistCalibMap.h"
#include "AcdCalibMap.h"
#include "AcdCalibUtil.h"
#include "AcdRibbonFit.h"

#include "digiRootData/DigiEvent.h"
#include "reconRootData/ReconEvent.h"
#include "reconRootData/AcdAssoc.h"
#include "reconRootData/AcdTkrHitPocaV2.h"
#include "reconRootData/AcdTkrHitPoca.h"
#include "reconRootData/AcdTkrIntersection.h"
#include "reconRootData/AcdTkrPointV2.h"
#include "reconRootData/AcdTkrPoint.h"
#include "CalibData/Acd/AcdCalibObj.h"
#include "CalibData/Acd/AcdCheck.h"

#include <TMath.h>

#include <cassert>
#include <cmath>
#include <fstream>

using std::cout;
using std::cerr;
using std::endl;
using std::string;


AcdCalibLoop_Recon::AcdCalibLoop_Recon(AcdCalibData::CALTYPE t, 
					 TChain *digiChain, 
					 TChain *reconChain, 
					 Bool_t requirePeriodic, AcdKey::Config config)
  :AcdCalibBase(t,config), 
   m_digiEvent(0),
   m_reconEvent(0),
   m_requirePeriodic(requirePeriodic),
   m_pedHists(0),
   m_peakHists(0),
   m_rangeHists(0),
   m_threshHists(0){

  if ( digiChain ){
    setChain(AcdCalib::DIGI,digiChain);
  }

  if ( reconChain ){
    setChain(AcdCalib::RECON,reconChain);
  }

  switch ( t ) {
  case AcdCalibData::RIBBON:
    m_peakHists = bookHists(AcdCalib::H_RIBBONS,64,-0.5,1023.5,7);
    break;
  case AcdCalibData::MERITCALIB:
    m_pedHists = bookHists(AcdCalib::H_RAW,4096,-0.5,4095.5);
    m_peakHists = bookHists(AcdCalib::H_GAIN,500,-0.005,4.995);
    m_rangeHists = bookHists(AcdCalib::H_RANGE,4096,-0.5,4095.5,2);
    m_threshHists = bookHists(AcdCalib::H_VETO,256,-0.5,4095.5,3);    
    break;
  case AcdCalibData::GAIN:
    m_peakHists = bookHists(AcdCalib::H_GAIN,64,-0.5,2047.5);
    break;
  default:
    break;
  }

  Bool_t ok = attachChains();
  if ( ! ok ) {
    cerr << "ERR:  Failed to attach to input chains."  << endl;
  }
}


AcdCalibLoop_Recon::~AcdCalibLoop_Recon() 
{
  if (m_digiEvent) delete m_digiEvent;	
  if (m_reconEvent) delete m_reconEvent;
}

Bool_t AcdCalibLoop_Recon::attachChains() {
  TChain* digiChain = getChain(AcdCalib::DIGI);
  if (digiChain != 0) {
    m_digiEvent = 0;
    digiChain->SetBranchAddress("DigiEvent", &m_digiEvent);
    digiChain->SetBranchStatus("*",0);  // disable all branches
    // activate desired brances
    digiChain->SetBranchStatus("m_timeStamp", 1); 
    digiChain->SetBranchStatus("m_gem", 1);
  }
  
  TChain* reconChain = getChain(AcdCalib::RECON);
  if (reconChain != 0) {
    m_reconEvent = 0;
    reconChain->SetBranchAddress("ReconEvent", &m_reconEvent);
    reconChain->SetBranchStatus("*",0);  // disable all branches
    // activate desired brances
    reconChain->SetBranchStatus("m_acdV2",1);
    reconChain->SetBranchStatus("m_eventId",1);
    reconChain->SetBranchStatus("m_runId",1);
  }
				
  return kTRUE;
}

Bool_t AcdCalibLoop_Recon::readEvent(int ievent, Bool_t& filtered, 
				      int& runId, int& evtId, Double_t& timeStamp) {
  
  if(m_digiEvent) m_digiEvent->Clear();

  filtered = kFALSE;
  TChain* digiChain = getChain(AcdCalib::DIGI);
  if(digiChain) { 
    digiChain->GetEvent(ievent);
    timeStamp = m_digiEvent->getTimeStamp();
  }

  TChain* reconChain = getChain(AcdCalib::RECON);
  if(reconChain) { 
    reconChain->GetEvent(ievent);
    evtId = m_reconEvent->getEventId();
    runId = m_reconEvent->getRunId();
  }

    filtered = kFALSE; 
  return kTRUE;
}


//void AcdCalibLoop_Recon::useEvent(Bool_t& used, ofstream& outfile, int ievent) {
void AcdCalibLoop_Recon::useEvent(Bool_t& used) {

  used = kFALSE;

    fillRecoGainInfo(used);
    return;

}

/// Fill gain variables from reco information
//void AcdCalibLoop_Recon::fillRecoGainInfo(Bool_t& used, ofstream& outfile, int ievent) {
void AcdCalibLoop_Recon::fillRecoGainInfo(Bool_t& used) {
 
  int AcdRange[700][2] = {{0}};
  int AcdPha[700][2] = {{0}};
  int id;
 
  const TClonesArray& acdHitCol = m_reconEvent->getAcdReconV2()->getHitCol(); 

  UInt_t nAcdHit = acdHitCol.GetEntries();
  for ( UInt_t n(0); n < nAcdHit; n++) {
    const AcdHit* acdHit = dynamic_cast<const AcdHit*>(acdHitCol.At(n));
    id = acdHit->getId().getId();

    AcdPha[id][0] = acdHit->getPha(AcdHit::A);
    AcdPha[id][1] = acdHit->getPha(AcdHit::B);

   if ( (acdHit->getFlags(AcdHit::A) & 0x4) != 0 ) { AcdRange[id][0] = 1; }
   else { AcdRange[id][0] = 0; }

   if ( (acdHit->getFlags(AcdHit::B) & 0x4) != 0 ) { AcdRange[id][1] = 1; }
   else { AcdRange[id][1] = 0; }

  }

  bool usedTkr = false;
  bool usedCal = false;

  const TClonesArray& acdTkrAssocCol = m_reconEvent->getAcdReconV2()->getTkrAssocCol();
  UInt_t nTkrAssoc = acdTkrAssocCol.GetEntries();


  int bestCR = -9999;
  for ( UInt_t i(0); i < nTkrAssoc; i++ ) {
    const AcdAssoc* anAssoc = dynamic_cast<const AcdAssoc*>(acdTkrAssocCol.At(i));
    if (anAssoc == 0) continue;
    if ( anAssoc->getTrackIndex() <= bestCR ) continue;  // First track only
    if (! anAssoc->getUpward()) continue;            
    if (anAssoc->getTrackIndex() > bestCR ) bestCR = anAssoc->getTrackIndex();
  }

//  std::cout << bestCR << std::endl;

  for ( UInt_t i(0); i < nTkrAssoc; i++ ) {
    const AcdAssoc* anAssoc = dynamic_cast<const AcdAssoc*>(acdTkrAssocCol.At(i));
    if (anAssoc == 0) continue;
    if ( anAssoc->getTrackIndex() != bestCR ) continue;  // First track only
//    if ( anAssoc->getTrackIndex() <= 100 ) continue;  // First track only

//    if ( anAssoc->getTrackIndex() !=0 ) continue;  // First track only
    if (! anAssoc->getUpward()) continue;            

//    UInt_t nTkrHit = 0;
//    UInt_t nTkrHit = anAssoc->nAcdHitPoca();

    UInt_t shit = 1;
    UInt_t nTkrHit = std::min(anAssoc->nAcdHitPoca(),shit);
    
    for ( UInt_t iHit(0); iHit < nTkrHit; iHit++ ) {
      const AcdTkrHitPocaV2* aHitPoca = anAssoc->getHitPoca(iHit);

      if ( ! aHitPoca->hasHit() ) continue;

      const AcdId& aHitId = aHitPoca->getId();

      int rng0 = AcdRange[aHitId.getId()][AcdDigi::A];
      int rng1 = AcdRange[aHitId.getId()][AcdDigi::B];

      int pmt0 = AcdPha[aHitId.getId()][AcdDigi::A];
      int pmt1 = AcdPha[aHitId.getId()][AcdDigi::B];

//      float pathFactor = 1;

//      if (aHitId.getId() < 50 ){
//        pathFactor = fabs(1/anAssoc->getDir().z());
//      }
//      else if ( (aHitId.getId() >= 100 && aHitId.getId() <= 150) || (aHitId.getId() >= 300 && aHitId.getId() <= 350) ) {
//        pathFactor = fabs(1/anAssoc->getDir().x());
//      }
//      else if ( (aHitId.getId() >= 200 && aHitId.getId() <= 250) || (aHitId.getId() >= 400 && aHitId.getId() <= 450) ) {
//        pathFactor = fabs(1/anAssoc->getDir().y());
//      }
//      else{
//        pathFactor = 1;
//      }

      float pathFactor = fabs(1/(aHitPoca->getCosTheta()));


      if (aHitId.isRibbon()) pathFactor = 1;

//      if (pathFactor < 0.99) continue;
//      if (pathFactor > 2.0) continue;

      bool docaCut = false;

      if (aHitId.isRibbon()){
        if (aHitPoca->getDoca() >= 0) docaCut = true;
        else docaCut = false;
      }
      else if (aHitId.isTile()){
        if (aHitPoca->getDoca() >= 0) docaCut = true;
        else docaCut = false;
      }
      else {
        docaCut = false;
      }

      if (!docaCut) continue;

//      bool VetoSigmaProjCut = false;
//
//      if (aHitId.isTile()) {
//        if ( aHitPoca->vetoSigmaProj() < 0 ) VetoSigmaProjCut = true;
//        else VetoSigmaProjCut = false;
//      }
//      else if (aHitId.isRibbon()) {
//        if ( aHitPoca->vetoSigmaProj() < 0 ) VetoSigmaProjCut = true;
//        else VetoSigmaProjCut = false;
//      }
//      else {
//        VetoSigmaProjCut = false;
//      }
//
//     if (!VetoSigmaProjCut) continue;

      usedTkr = true;      

      UInt_t keyA = AcdKey::makeKey(AcdDigi::A,aHitId.getId());
      UInt_t keyB = AcdKey::makeKey(AcdDigi::B,aHitId.getId());

      Float_t redPha_A = ((Float_t)(pmt0));
      Float_t redPha_B = ((Float_t)(pmt1));


      if ( calType() == AcdCalibData::GAIN || calType() == AcdCalibData::RIBBON ) {

        float pedA = getPeds(keyA);
        float pedB = getPeds(keyB);

        if (pedA < 0 || pedB < 0){
          std::cerr << "No Pedestal " << keyA << " " << keyB << std::endl;
          return;
        }

        redPha_A -= pedA;
        redPha_B -= pedB;
        // Correct for the pathLength
        redPha_A /= pathFactor;
        redPha_B /= pathFactor;

      }

      //now ribbon stuff gets fun, only take stuff from the middle of the ribbon
      Int_t ribbonBin(-1);
      Bool_t ribbonCut(kTRUE);
      if ( aHitId.isRibbon()) {

        Float_t localX = AcdRibbonFitLibrary::getLocalX(aHitId.getId(),
                                                    aHitPoca->getPoca().x(),
                                                    aHitPoca->getPoca().y(),
                                                    aHitPoca->getPoca().z());

        ribbonBin = AcdRibbonFitLibrary::getBin(id,localX);

        if ( ribbonBin != 3) ribbonCut = kFALSE;
      }
     
    if ( calType() == AcdCalibData::GAIN ) {
      if (!ribbonCut ) continue;
      if ( rng0 == 0 ) { 
//        std::cout << aHitId.getId() << " 0 " << pathFactor << " " << redPha_A << std::endl;
        fillHist(*m_peakHists, aHitId.getId(), AcdDigi::A, redPha_A);
        }
      if ( rng1 == 0 ) {
//        std::cout << aHitId.getId() << " 1 " << pathFactor << " " << redPha_B << std::endl;
        fillHist(*m_peakHists, aHitId.getId(), AcdDigi::B, redPha_B);
        }
    }
    else if ( calType() == AcdCalibData::RIBBON ) {
      if ( ribbonBin < 0 ) {
        std::cerr << "Couldn't bin ribbon " << id << ' '
                << aHitPoca->getGlobalPosition().x() << ' '
                << aHitPoca->getGlobalPosition().y() << ' '
                << aHitPoca->getGlobalPosition().z() << std::endl;
        continue;
      }
      if ( rng0 == 0 ) fillHist(*m_peakHists,aHitId.getId(),AcdDigi::A,redPha_A,ribbonBin);
      if ( rng1 == 0 ) fillHist(*m_peakHists,aHitId.getId(),AcdDigi::B,redPha_B,ribbonBin);
    }
    else {
      std::cerr << "Unknown calibration type " << calType() << std::endl;
    }
    }    
  }


  used = usedTkr || usedCal;

//  if ( usedTkr ) return;
//  return;

  const TClonesArray& acdCalAssocCol = m_reconEvent->getAcdReconV2()->getCalAssocCol();
  UInt_t nCalAssoc = acdCalAssocCol.GetEntries();
  for ( UInt_t i(0); i < nCalAssoc; i++ ) {
    const AcdAssoc* anAssoc = dynamic_cast<const AcdAssoc*>(acdCalAssocCol.At(i));
    if (anAssoc == 0) continue;
    if ( anAssoc->getTrackIndex() !=0 ) continue;  // First track only
    if (! anAssoc->getUpward()) continue;

//    UInt_t nCalHit = 0;
    UInt_t shit = 1;
    UInt_t nCalHit = std::min(anAssoc->nAcdHitPoca(),shit);

    for ( UInt_t iHit(0); iHit < nCalHit; iHit++ ) {
      const AcdTkrHitPocaV2* aHitPoca = anAssoc->getHitPoca(iHit);

      if ( ! aHitPoca->hasHit() ) continue;

      const AcdId& aHitId = aHitPoca->getId();

//      if (aHitId.isRibbon()) continue;

      bool tileCut = false;

      if (aHitId.getId() == 130 || aHitId.getId() == 230 || aHitId.getId() == 330 || aHitId.getId() == 430) tileCut = true;

      if (!tileCut) continue;

      int rng0 = AcdRange[aHitId.getId()][AcdDigi::A];
      int rng1 = AcdRange[aHitId.getId()][AcdDigi::B];

      int pmt0 = AcdPha[aHitId.getId()][AcdDigi::A];
      int pmt1 = AcdPha[aHitId.getId()][AcdDigi::B];

//      float pathFactor = 1;

      float pathFactor = fabs(1/(aHitPoca->getCosTheta()));

//      if (aHitId.getId() < 50 ){
//        pathFactor = fabs(1/anAssoc->getDir().z());
//      }
//      else if ( (aHitId.getId() >= 100 && aHitId.getId() <= 150) || (aHitId.getId() >= 300 && aHitId.getId() <= 350) ) {
//        pathFactor = fabs(1/anAssoc->getDir().x());
//      }
//      else if ( (aHitId.getId() >= 200 && aHitId.getId() <= 250) || (aHitId.getId() >= 400 && aHitId.getId() <= 450) ) {
//        pathFactor = fabs(1/anAssoc->getDir().y());
//      }
//      else{
//        pathFactor = 1;
//      }


//      if (pathFactor < 0.99) continue;
      if (pathFactor > 2.0) continue;

      bool docaCut = false;

      if (aHitId.isRibbon()){
        if (aHitPoca->getDoca() >= 0) docaCut = true;
        else docaCut = false;
      }
      else if (aHitId.isTile()){
       if (aHitPoca->getDoca() >= 0) docaCut = true;
        else docaCut = false;
      }
      else {
        docaCut = false;
      }

      if (!docaCut) continue;

//      bool VetoSigmaProjCut = false;
//
//      if (aHitId.isTile()) {
//        if ( aHitPoca->vetoSigmaProj() < 0 ) VetoSigmaProjCut = true;
//        else VetoSigmaProjCut = false;
//      }
//      else if (aHitId.isRibbon()) {
//        if ( aHitPoca->vetoSigmaProj() < 0 ) VetoSigmaProjCut = true;
//        else VetoSigmaProjCut = false;
//      }
//      else {
//        VetoSigmaProjCut = false;
//      }
//
//      if (!VetoSigmaProjCut) continue;

      usedCal = true;

      UInt_t keyA = AcdKey::makeKey(AcdDigi::A,aHitId.getId());
      UInt_t keyB = AcdKey::makeKey(AcdDigi::B,aHitId.getId());

      Float_t redPha_A = ((Float_t)(pmt0));
      Float_t redPha_B = ((Float_t)(pmt1));


      if ( calType() == AcdCalibData::GAIN || calType() == AcdCalibData::RIBBON ) {

        float pedA = getPeds(keyA);
        float pedB = getPeds(keyB);

        if (pedA < 0 || pedB < 0){
          std::cerr << "No Pedestal " << keyA << " " << keyB << std::endl;
          return;
        }

        redPha_A -= pedA;
        redPha_B -= pedB;
        // Correct for the pathLength
        redPha_A /= pathFactor;
        redPha_B /= pathFactor;

      }

      //now ribbon stuff gets fun, only take stuff from the middle of the ribbon
      Int_t ribbonBin(-1);
      Bool_t ribbonCut(kTRUE);
      if ( aHitId.isRibbon()) {

        Float_t localX = AcdRibbonFitLibrary::getLocalX(aHitId.getId(),
                                                    aHitPoca->getPoca().x(),
                                                    aHitPoca->getPoca().y(),
                                                    aHitPoca->getPoca().z());

        ribbonBin = AcdRibbonFitLibrary::getBin(id,localX);

        if ( ribbonBin != 3) ribbonCut = kFALSE;
      }

    if ( calType() == AcdCalibData::GAIN ) {
      if (!ribbonCut ) continue;
      if ( rng0 == 0 ) {
        fillHist(*m_peakHists, aHitId.getId(), AcdDigi::A, redPha_A);
        }
      if ( rng1 == 0 ) {
        fillHist(*m_peakHists, aHitId.getId(), AcdDigi::B, redPha_B);
        }
    }
    else if ( calType() == AcdCalibData::RIBBON ) {
      if ( ribbonBin < 0 ) {
        std::cerr << "Couldn't bin ribbon " << id << ' '
                << aHitPoca->getGlobalPosition().x() << ' '
                << aHitPoca->getGlobalPosition().y() << ' '
                << aHitPoca->getGlobalPosition().z() << std::endl;
        continue;
      }
      if ( rng0 == 0 ) fillHist(*m_peakHists,aHitId.getId(),AcdDigi::A,redPha_A,ribbonBin);
      if ( rng1 == 0 ) fillHist(*m_peakHists,aHitId.getId(),AcdDigi::B,redPha_B,ribbonBin);
    }
    else {
      std::cerr << "Unknown calibration type " << calType() << std::endl;
    }
    }
  }

  used = usedTkr || usedCal;

  return;
}

/// get the MIP value for one pmt
bool AcdCalibLoop_Recon::getMipValue(UInt_t key, UInt_t range, UInt_t pha, Float_t& mips) {
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


