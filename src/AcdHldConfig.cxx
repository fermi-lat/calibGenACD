#define AcdHldConfig_cxx

// base class
#include "AcdHldConfig.h"

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

Float_t AcdHldConfig::settingAsFloat(UChar_t hld_dac) {
  Float_t val = (Float_t) hld_dac;
  return val;
}
  
void AcdHldConfig::floatToDacVals(Float_t value, UChar_t& hld_dac) {
  if ( value > 63 ) {
    hld_dac = 63;
    return;
  } else if ( value <= 0 ) {
    hld_dac = 0;
    return;
  }
  hld_dac = (UChar_t)( value );
}

AcdHldConfig::AcdHldConfig( ){
  for ( UInt_t i(0); i < 12; i++ ) {
    for ( UInt_t j(0); j < 18; j++ ) {
      m_hld_dac[i][j] = 0;
    }
  }
}


AcdHldConfig::~AcdHldConfig() {  
}

void AcdHldConfig::getSetting(UInt_t key, UChar_t& hld_dac) const {
  UInt_t garc(0), gafe(0);
  UInt_t tile = AcdKey::getId(key);
  UInt_t pmt = AcdKey::getPmt(key);
  AcdGarcGafeHits::convertToGarcGafe(tile,pmt,garc,gafe); 
  getSetting(garc,gafe,hld_dac);
}

void AcdHldConfig::getSetting(UInt_t garc, UInt_t gafe, UChar_t& hld_dac) const {
  hld_dac = m_hld_dac[garc][gafe];
}

void AcdHldConfig::setSetting(UInt_t key, UChar_t hld_dac) {
  UInt_t garc(0), gafe(0);
  UInt_t tile = AcdKey::getId(key);
  UInt_t pmt = AcdKey::getPmt(key);
  AcdGarcGafeHits::convertToGarcGafe(tile,pmt,garc,gafe); 
  setSetting(garc,gafe,hld_dac);
}

void AcdHldConfig::setSetting(UInt_t garc, UInt_t gafe, UChar_t hld_dac) {
  m_hld_dac[garc][gafe] = hld_dac;
}


Bool_t AcdHldConfig::readXmlFile(const char* fileName) {
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
      const DOMElement* hldElem = xmlBase::Dom::findFirstChildByName(*itr2,"hld_dac");
      m_hld_dac[garc][gafe]     = facilities::Util::stringToUnsigned( xmlBase::Dom::getTextContent( hldElem ) );
    }
  }
  m_fileName = fileName;
  return kTRUE; 
}


Bool_t AcdHldConfig::writeXmlFile(const char* /* fileName */) const {
  return kFALSE;
}
