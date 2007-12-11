
#ifndef AcdCalibMap_h
#define AcdCalibMap_h

#include "Rtypes.h"

// stl includes
#include <map>
#include <string>
#include <iostream>

#include <xercesc/util/XercesDefs.hpp>
#include "xercesc/dom/DOMElement.hpp"
using XERCES_CPP_NAMESPACE_QUALIFIER DOMElement;

namespace CalibData {
  class AcdCalibDescription;
  class AcdCalibObj;
}

class AcdHistCalibMap;
class AcdCalibBase;
class DomElement;

class AcdCalibMap {
public:

  AcdCalibMap(const CalibData::AcdCalibDescription& desc);

  virtual ~AcdCalibMap();
  
  void add(UInt_t key, CalibData::AcdCalibObj& result);
  const CalibData::AcdCalibObj* get(UInt_t key) const;
  CalibData::AcdCalibObj* get(UInt_t key);
  CalibData::AcdCalibObj* makeNew() const;

  Bool_t writeTxtFile(const char* fileName,
		      const char* instrument,
		      const char* timestamp,
		      const char* algorithm,
		      const AcdCalibBase& calib);
  void writeTxt(std::ostream& os) const;

  Bool_t writeXmlFile(const char* fileName,
		      const char* instrument,
		      const char* timestamp,
		      const char* algorithm,
		      const AcdCalibBase& calib) const;
  void writeXmlHeader(DomElement& node,
		      const char* instrument,
		      const char* timestamp,
		      const char* algorithm,
		      const AcdCalibBase& calib) const;
  void writeXmlFooter(DomElement& node) const;
  void writeXmlBody(DomElement& node) const;
  
  Bool_t readXmlFile(const char* fileName);
  Bool_t readXmlTile(DOMElement& node);
  Bool_t readXmlCalib(DOMElement& node, UInt_t key);

  Bool_t readTxtFile(const char* fileName);
  Bool_t readTxt(std::istream& is);

  const char* fileName() const {
    return m_fileName.c_str();
  }

  inline const std::map<UInt_t,CalibData::AcdCalibObj*>& theMap() const { return m_map; }

protected:

  AcdCalibMap();

private:  

  const CalibData::AcdCalibDescription* m_desc;
  
  std::map<UInt_t,CalibData::AcdCalibObj*> m_map;
  std::string m_fileName;  

};

#endif

#ifdef AcdCalibMap_cxx

#endif // #ifdef AcdCalibMap_cxx
