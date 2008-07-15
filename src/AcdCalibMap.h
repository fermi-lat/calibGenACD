
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

/** 
 * @class AcdCalibMap
 *
 * @brief A set of calibration constants for all channels, 
 *
 * Also provides functions to read and write constants to XML and TXT formats
 *
 * All mapping is done using a decimal key:
 *    1000 * pmt + 100*face + 10*row + col
 *  
 *
 * @author Eric Charles
 * $Header$
 */



class AcdCalibMap {
public:

  /// Build from a calibration description
  AcdCalibMap(const CalibData::AcdCalibDescription& desc);
  /// Null c'tor
  virtual ~AcdCalibMap();

  /// Add a calibration object by key
  void add(UInt_t key, CalibData::AcdCalibObj& result);

  /// Get a calibration object by key
  const CalibData::AcdCalibObj* get(UInt_t key) const;  
  /// Get a calibration object by key
  CalibData::AcdCalibObj* get(UInt_t key);

  /// Allocate a new calibration object
  /// This uses the calibration description to make the correct type of object
  CalibData::AcdCalibObj* makeNew() const;

  /// Write calibration to a text file
  Bool_t writeTxtFile(const char* fileName,
		      const char* instrument,
		      const char* timestamp,
		      const char* algorithm,
		      const AcdCalibBase& calib);
  /// Write calibration data to an output stream
  void writeTxt(std::ostream& os) const;

  /// Write calibration to an xml file
  Bool_t writeXmlFile(const char* fileName,
		      const char* instrument,
		      const char* timestamp,
		      const char* algorithm,
		      const AcdCalibBase& calib) const;
  
  /// Write header data to an xml file
  void writeXmlHeader(DomElement& node,
		      const char* instrument,
		      const char* timestamp,
		      const char* algorithm,
		      const AcdCalibBase& calib) const;
  /// Write footer data to an xml file
  void writeXmlFooter(DomElement& node) const;
  /// Write actual calibration data to an xml file
  void writeXmlBody(DomElement& node) const;

  /// Write fit results to a TTree in a file
  Bool_t writeResultsToTree(const char* newFileName);
  
  /// Read calibration from an xml file, return kTRUE for success
  Bool_t readXmlFile(const char* fileName);
  /// Read calibration for 1 element (2 PMTs) from an xml file, return kTRUE for success
  Bool_t readXmlTile(DOMElement& node);
  /// Read calibration for 1 PMT from an xml file, return kTRUE for success
  Bool_t readXmlCalib(DOMElement& node, UInt_t key);

  /// Read calibration from a text file, return kTRUE for success
  Bool_t readTxtFile(const char* fileName);
  /// Read calibration from input stream, return kTRUE for success
  Bool_t readTxt(std::istream& is);

  /// Return the name of the file associated with this calibration
  const char* fileName() const {
    return m_fileName.c_str();
  }

  
  inline const std::map<UInt_t,CalibData::AcdCalibObj*>& theMap() const { return m_map; }

protected:

  AcdCalibMap();

private:  

  /// Description of the calibration handled by this map
  const CalibData::AcdCalibDescription* m_desc;
  
  /// The calibration of the individual channels
  std::map<UInt_t,CalibData::AcdCalibObj*> m_map;

  /// the name of the file associated with this calibration
  std::string m_fileName;  

};

#endif

#ifdef AcdCalibMap_cxx

#endif // #ifdef AcdCalibMap_cxx
