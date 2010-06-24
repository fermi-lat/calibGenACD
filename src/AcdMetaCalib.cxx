
#include "AcdMetaCalib.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TChain.h"

#include "AcdHistCalibMap.h"
#include "AcdCalibMap.h"
#include "AcdCalibUtil.h"
#include "AcdVetoConfig.h"
#include "AcdHldConfig.h"

#include "CalibData/Acd/AcdCalibObj.h"
#include "CalibData/Acd/AcdVeto.h"
#include "CalibData/Acd/AcdCno.h"
#include <TMath.h>

#include <cassert>
#include <cmath>
#include <fstream>

using std::cout;
using std::cerr;
using std::endl;
using std::string;


AcdMetaCalib::AcdMetaCalib(AcdCalibData::CALTYPE t, AcdKey::Config config)
  :AcdCalibBase(t,config),
   m_fitHists(0),
   m_highRangeHists(0){
  switch ( t ) {
  case AcdCalibData::VETO_FIT:
    m_fitHists = bookHists(AcdCalib::H_VETO_FIT,750,0.,750.);
    break;
  case AcdCalibData::CNO_FIT:
    m_fitHists = bookHists(AcdCalib::H_VETO_FIT,400,0.,400);    
    break;
  case AcdCalibData::HIGH_RANGE:
    m_highRangeHists = bookHists(AcdCalib::H_HIGH_RANGE,100,0.,100.,2);
    break;
  default:
    break;
  }
}


AcdMetaCalib::~AcdMetaCalib() {
}


Bool_t AcdMetaCalib::readSettingsSetPointPair(const std::string& settingsFile,
					      const std::string& setPointFile) {

  AcdVetoConfig* veto(0);
  AcdHldConfig* hld(0);  
  AcdCalibMap* calib(0);

  switch ( calType() ) {
  case AcdCalibData::VETO_FIT:
    calib = new AcdCalibMap( CalibData::AcdVetoFitDesc::instance() );
    calib->readXmlFile( setPointFile.c_str() );
    veto = new AcdVetoConfig;
    if ( ! veto->readXmlFile( settingsFile.c_str() ) ) return kFALSE;
    m_vetoPoints.push_back( std::make_pair( veto, calib ) );
    break;
  case AcdCalibData::CNO_FIT:
    calib = new AcdCalibMap( CalibData::AcdCnoFitDesc::instance() );
    calib->readXmlFile( setPointFile.c_str() );
    hld = new AcdHldConfig;
    if ( ! hld->readXmlFile( settingsFile.c_str() ) ) return kFALSE;
    m_cnoPoints.push_back( std::make_pair( hld, calib ) );
    break;
  default:
    return kFALSE;
  }

  
  return kTRUE;  
}

Bool_t AcdMetaCalib::fillVetoThresholds() {
  if ( m_vetoPoints.size() < 2 ) return kFALSE;
  for ( std::list< std::pair< AcdVetoConfig*, AcdCalibMap* > >::const_iterator itr = m_vetoPoints.begin();
	itr != m_vetoPoints.end(); itr++ ) {
    const AcdVetoConfig* veto = itr->first; 
    const AcdCalibMap* config = itr->second; 
    for ( std::map<UInt_t,CalibData::AcdCalibObj*>::const_iterator itrM = config->theMap().begin();
	  itrM != config->theMap().end(); itrM++ ) {
      UInt_t id = AcdKey::getId(itrM->first);
      if ( id >= 700 ) continue;
      UInt_t pmt = AcdKey::getPmt(itrM->first);      
      UChar_t veto_dac(0); UChar_t veto_vern(0);
      veto->getSetting(itrM->first,veto_dac,veto_vern);
      Float_t val = AcdVetoConfig::settingAsFloat(veto_dac,veto_vern);
      const CalibData::AcdCalibObj* setPoint = itrM->second;
      UInt_t bin = (UInt_t)( setPoint->operator[](0)) + 1;
      fillHistBin(*m_fitHists,id,pmt,bin,val,0.04);
    }
  }
  return kTRUE;
}


Bool_t AcdMetaCalib::fillCnoThresholds() {
  if ( m_cnoPoints.size() < 2 ) return kFALSE;
  for ( std::list< std::pair< AcdHldConfig*, AcdCalibMap* > >::const_iterator itr = m_cnoPoints.begin();
	itr != m_cnoPoints.end(); itr++ ) {
    const AcdHldConfig* hld = itr->first; 
    const AcdCalibMap* config = itr->second;    
    for ( std::map<UInt_t,CalibData::AcdCalibObj*>::const_iterator itrM = config->theMap().begin();
	  itrM != config->theMap().end(); itrM++ ) {
      UInt_t id = AcdKey::getId(itrM->first);
      UInt_t pmt = AcdKey::getPmt(itrM->first);      
      if ( id >= 700 ) continue;
      UChar_t hld_dac(0);
      hld->getSetting(itrM->first,hld_dac);		      
      Float_t val = AcdHldConfig::settingAsFloat(hld_dac);
      const CalibData::AcdCalibObj* setPoint = itrM->second;
      UInt_t bin = (UInt_t)( setPoint->operator[](0)) + 1;
      fillHistBin(*m_fitHists,id,pmt,bin,val,0.5);
    }
  }
  return kTRUE;
}

Bool_t  AcdMetaCalib::fillHighRangeData() {

  const AcdCalibMap* carbonMap = getCalibMap(AcdCalibData::CARBON);
  const AcdCalibMap* pedMap = getCalibMap(AcdCalibData::PEDESTAL);
  const AcdCalibMap* pedHighMap = getCalibMap(AcdCalibData::PED_HIGH);
  const AcdCalibMap* mipMap = getCalibMap(AcdCalibData::GAIN);
  const AcdCalibMap* rangeMap = getCalibMap(AcdCalibData::RANGE);
  if ( carbonMap == 0 ||
       pedMap == 0 ||
       pedHighMap == 0 ||
       mipMap == 0 ||
       rangeMap == 0 ) return kFALSE;
  for ( std::map<UInt_t,CalibData::AcdCalibObj*>::const_iterator itrM = carbonMap->theMap().begin();
	itrM != carbonMap->theMap().end(); itrM++ ) {
    UInt_t id = AcdKey::getId(itrM->first);
    UInt_t pmt = AcdKey::getPmt(itrM->first);      
    if ( id >= 700 ) continue;
    const CalibData::AcdCalibObj* carbonCalib = itrM->second;
    const CalibData::AcdCalibObj* pedCalib = pedMap->get(itrM->first);
    const CalibData::AcdCalibObj* pedHighCalib = pedHighMap->get(itrM->first);
    const CalibData::AcdCalibObj* rangeCalib = rangeMap->get(itrM->first);
    const CalibData::AcdCalibObj* mipCalib = mipMap->get(itrM->first);
    Float_t lowPed =  pedCalib->operator[](0);
    Float_t highPed =  pedHighCalib->operator[](0);    
    Float_t lowDynRange = rangeCalib->operator[](0) - lowPed;
    Float_t highGap = rangeCalib->operator[](1) - highPed;
    Float_t gainRatio = lowDynRange / highGap;
    Float_t mipsInHiPha = mipCalib->operator[](1) / gainRatio;
    Float_t carbonPeak = carbonCalib->operator[](0);
    fillHistBin(*m_highRangeHists,id,pmt,1,highPed,0.01);   
    fillHistBin(*m_highRangeHists,id,pmt,2,mipsInHiPha+highPed,5.,1);
    fillHistBin(*m_highRangeHists,id,pmt,34,carbonPeak+highPed,1.);
  }
  return kTRUE;
}

Bool_t AcdMetaCalib::copyPeakVals() {

  AcdCalibMap* outCalib = getCalibMap( calType() );
  const AcdCalibMap* inCalib(0);
  switch ( calType()) {
  case AcdCalibData::VETO_FIT:
    inCalib = getCalibMap( AcdCalibData::GAIN );
    break;
  case AcdCalibData::CNO_FIT:
    inCalib = getCalibMap( AcdCalibData::CARBON );
    break;
  default:
    return kFALSE;
  }
  if ( inCalib == 0 || outCalib == 0 ) return kFALSE;
  
  const std::map<UInt_t,CalibData::AcdCalibObj*>& theMap = inCalib->theMap();
  for ( std::map<UInt_t,CalibData::AcdCalibObj*>::const_iterator itr = theMap.begin();
	itr != theMap.end(); itr++ ) {
    const CalibData::AcdCalibObj* inRes = itr->second;
    CalibData::AcdCalibObj* outRes = outCalib->get(itr->first);
    outRes->operator[](2) = inRes->operator[](0);
  }

  static const std::string vetoSetType("VetoSettings");
  static const std::string hldSetType("HldSettings");
  static const std::string vetoType("VetoFit");
  static const std::string hldType("CnoFit");

  for ( std::list< std::pair< AcdVetoConfig*, AcdCalibMap* > >::const_iterator itr = m_vetoPoints.begin();
	itr != m_vetoPoints.end(); itr++ ) {
    const AcdVetoConfig* veto = itr->first; 
    const AcdCalibMap* calib = itr->second; 
    outCalib->addInput(std::string(veto->fileName()),vetoSetType);
    outCalib->addInput(std::string(calib->fileName()),vetoType);
  }
  for ( std::list< std::pair< AcdHldConfig*, AcdCalibMap* > >::const_iterator itr2 = m_cnoPoints.begin();
	itr2 != m_cnoPoints.end(); itr2++ ) {
    const AcdHldConfig* hld = itr2->first; 
    const AcdCalibMap* calib = itr2->second;    
    outCalib->addInput(std::string(hld->fileName()),hldSetType);
    outCalib->addInput(std::string(calib->fileName()),hldType);    
  }

  return kTRUE;
}
