#define AcdVetoConfig_cxx

// base class
#include "AcdVetoConfig.h"

// local headers
#include "AcdCalibEnum.h"
#include "AcdKey.h"
#include "AcdXmlUtil.h"
#include "DomElement.h"
#include "AcdGarcGafeHits.h"

// ROOT
#include <TMath.h>

#include "facilities/Util.h"

// stl includes
#include <iostream>
#include <fstream>
#include <cassert>

#include "xmlBase/Dom.h"
#include "xmlBase/XmlParser.h"
#include "xercesc/dom/DOMElement.hpp"
#include "xercesc/dom/DOMDocument.hpp"
#include "xercesc/dom/DOMImplementation.hpp"

Float_t AcdVetoConfig::settingAsFloat(UChar_t veto_dac, UChar_t veto_vernier) {
  Float_t val = (Float_t) veto_dac;
  val += ((Float_t)(veto_vernier)) / 32.;
  return val;
}
  
void AcdVetoConfig::floatToDacVals(Float_t value, UChar_t& veto_dac, UChar_t& veto_vernier) {
  if ( value > 65 ) {
    veto_dac = 63;
    veto_vernier = 63;
    return;
  } else if ( value <= 0 ) {
    veto_dac = 0;
    veto_vernier = 0;    
    return;
  }
  veto_dac = (UChar_t)( value );
  Float_t rem = value - veto_dac;
  veto_vernier = (UChar_t)( rem / 32. );  
}

AcdVetoConfig::AcdVetoConfig( ){
  for ( UInt_t i(0); i < 12; i++ ) {
    for ( UInt_t j(0); j < 18; j++ ) {
      m_veto_dac[i][j] = 0;
      m_veto_vernier[i][j] = 0;
    }
  }
}


AcdVetoConfig::~AcdVetoConfig() {  
}

void AcdVetoConfig::getSetting(UInt_t key, UChar_t& veto_dac, UChar_t& veto_vernier) const {
  UInt_t garc(0), gafe(0);
  UInt_t tile = AcdKey::getId(key);
  UInt_t pmt = AcdKey::getPmt(key);
  AcdGarcGafeHits::convertToGarcGafe( tile,pmt,garc,gafe ); 
  getSetting(garc,gafe,veto_dac,veto_vernier);
}

void AcdVetoConfig::getSetting(UInt_t garc, UInt_t gafe, UChar_t& veto_dac, UChar_t& veto_vernier) const {
  veto_dac = m_veto_dac[garc][gafe];
  veto_vernier = m_veto_vernier[garc][gafe];
}

void AcdVetoConfig::setSetting(UInt_t key, UChar_t veto_dac, UChar_t veto_vernier) {
  UInt_t garc(0), gafe(0);
  UInt_t tile = AcdKey::getId(key);
  UInt_t pmt = AcdKey::getPmt(key);
  AcdGarcGafeHits::convertToGarcGafe(tile,pmt,garc,gafe); 
  setSetting(garc,gafe,veto_dac,veto_vernier);
}

void AcdVetoConfig::setSetting(UInt_t garc, UInt_t gafe, UChar_t veto_dac, UChar_t veto_vernier) {
  m_veto_dac[garc][gafe] = veto_dac;
  m_veto_vernier[garc][gafe] = veto_vernier;  
}


Bool_t AcdVetoConfig::readXmlFile(const char* fileName) {
  
  using XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument;
  xmlBase::XmlParser parser;
  static const std::string garcName("ARC");
  static const std::string gafeName("AFE");  
  static const std::string idName("ID");  
  DOMDocument* doc = parser.parse(fileName);
  if ( doc == 0 ) return kFALSE;
  std::vector<DOMElement*> garcs;
  
  DOMElement* top = doc->getDocumentElement();					       
  xmlBase::Dom::getDescendantsByTagName(top,garcName,garcs);

  UInt_t garc(0), gafe(0);
  for ( std::vector<DOMElement*>::iterator itr = garcs.begin(); itr != garcs.end(); itr++ ) {
    garc = xmlBase::Dom::getIntAttribute(*itr,idName);
    std::vector<DOMElement*> gafes;
    xmlBase::Dom::getDescendantsByTagName(*itr,gafeName,gafes);
    for ( std::vector<DOMElement*>::iterator itr2 = gafes.begin(); itr2 != gafes.end(); itr2++ ) {
      gafe = xmlBase::Dom::getIntAttribute(*itr2,idName);
      const DOMElement* vetoElem = xmlBase::Dom::findFirstChildByName(*itr2,"veto_dac");
      const DOMElement* vernElem = xmlBase::Dom::findFirstChildByName(*itr2,"veto_vernier");
      m_veto_dac[garc][gafe]     = facilities::Util::stringToUnsigned( xmlBase::Dom::getTextContent( vetoElem ) );
      m_veto_vernier[garc][gafe] = facilities::Util::stringToUnsigned( xmlBase::Dom::getTextContent( vernElem ) );      
    }
  }
  m_fileName = fileName;
  return kTRUE;
}


Bool_t AcdVetoConfig::writeXmlFile(const char* /* fileName */) const {
  return kFALSE;
}
