
#ifndef AcdCalibMap_h
#define AcdCalibMap_h

#include "Rtypes.h"

// stl includes
#include <map>
#include <string>
#include <iostream>
#include <list>

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
class TTree;

/** 
 * @class AcdCalibMap
 *
 * @brief A set of calibration constants for all channels, 
 *
 * Also provides functions to read and write constants to XML and TXT formats, write html reports
 * store plots of the fit results 
 *
 * All mapping is done using a decimal key:
 *    1000 * pmt + 100*face + 10*row + col
 *  
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdCalibMap.h,v 1.13 2008/08/01 00:08:25 echarles Exp $
 */



class AcdCalibMap {

public:

  /// Build from a calibration description
  AcdCalibMap(const CalibData::AcdCalibDescription& desc);
  /// Null c'tor
  virtual ~AcdCalibMap();

  ///  add a calibration object by key
  void add(UInt_t key, CalibData::AcdCalibObj& result);

  /// return a calibration object by key
  const CalibData::AcdCalibObj* get(UInt_t key) const;  
  /// return a calibration object by key
  CalibData::AcdCalibObj* get(UInt_t key);

  /// return the start time for data used for this calib
  Double_t startTime() const { return m_startTime; }

  /// return the end time for data used for this calib
  Double_t endTime() const { return m_endTime; }
  
  /// return the number of triggers used for this calib
  UInt_t nTriggers() const { return m_triggers; }
 
  ///  Latch the stats for this calib
  void latchStats( Double_t sTime, Double_t eTime, UInt_t nTrig ) {
    m_startTime = sTime;
    m_endTime = eTime;
    m_triggers = nTrig;
  }

  ///  add a file to the list of inputs associated with this calibration
  void addInput( const std::string& path, const std::string& type ) {
    m_inputs.push_back( std::make_pair(path,type) ) ;
  }

  ///  write all the various output types
  Bool_t writeOutputs( const std::string& outputPrefix, 
		       const std::string& algorithm,
		       const std::string& instrument,
		       const std::string& timestamp,
		       bool isCheckCalib = false);

  /// Allocate a new calibration object
  /// This uses the calibration description to make the correct type of object
  CalibData::AcdCalibObj* makeNew() const;

  
  ///  read calibration from an xml file, return kTRUE for success
  Bool_t readXmlFile(const char* fileName);

  ///  read calibration from a text file, return kTRUE for success
  Bool_t readTxtFile(const char* fileName);

  ///  read the TTree with the calibration results (root filename is based on the xml filename)
  Bool_t readTree() const;

  /// return the name of the file associated with this calibration
  const char* fileName() const {
    return m_fileName.c_str();
  }

  ///  set the group of input histograms to be used for this calibration
  inline void setHists( AcdHistCalibMap& theHists ) {
    m_hists = &theHists;
  }

  ///  set the TTree which has the results of this calibration
  inline void setTree(TTree& tree) {
    m_tree = &tree;
  }

  ///  associate a reference calibration with this one
  inline void setReference(const AcdCalibMap& ref) {
    m_reference = &ref;
  }
  
  /// return the group of input histograms used for this calibration
  inline const AcdHistCalibMap* theHists() const { return m_hists; }

  /// return the reference calibration
  inline const AcdCalibMap* theReference() const { return m_reference; };

  /// return the TTree which has the results of this calibration
  inline TTree* theTree() const { return m_tree; }

  /// return the list of inputs associated with this calibration
  inline const std::list<std::pair<std::string,std::string> >& inputs() const { return m_inputs; }
  
  /// return the map from key to calibration result for each channel
  inline const std::map<UInt_t,CalibData::AcdCalibObj*>& theMap() const { return m_map; }

  /// return the description of the calibration results
  inline const CalibData::AcdCalibDescription* theDesc() const { return m_desc; }

  ///  write fit results to a TTree in a file
  Bool_t writeResultsToTree(const char* newFileName);

protected:

  /// Default c'tor
  AcdCalibMap();

  /// Read the header info from an xml file, return kTRUE for success
  Bool_t readXmlHeader(DOMElement& node);

  /// Read calibration for 1 element (2 PMTs) from an xml file, return kTRUE for success
  Bool_t readXmlTile(DOMElement& node);

  /// Read calibration for 1 PMT from an xml file, return kTRUE for success
  Bool_t readXmlCalib(DOMElement& node, UInt_t key);

  /// Read calibration from input stream, return kTRUE for success
  Bool_t readTxt(std::istream& is);

  /// Write calibration to a text file
  Bool_t writeTxtFile(const char* fileName,
		      const char* instrument,
		      const char* timestamp,
		      const char* algorithm );

  /// Write input data to an xml file
  void writeTxtInputInfo(std::ostream& os) const;  
  
  /// Write calibration data to an output stream
  void writeTxt(std::ostream& os) const;

  /// Write calibration to an xml file
  Bool_t writeXmlFile(const char* fileName,
		      const char* instrument,
		      const char* timestamp,
		      const char* algorithm ) const;
  
  /// Write header data to an xml file
  void writeXmlHeader(DomElement& node,
		      const char* instrument,
		      const char* timestamp,
		      const char* algorithm ) const;

  /// Write input data to an xml file
  void writeXmlInputInfo(DomElement& node) const;  

  /// Write actual calibration data to an xml file
  void writeXmlBody(DomElement& node) const;

private:    

  /// Description of the calibration handled by this map
  const CalibData::AcdCalibDescription* m_desc;
  
  /// A reference calibration
  const AcdCalibMap* m_reference;

  /// The histograms we git
  AcdHistCalibMap* m_hists;

  /// A tree with the summary data
  mutable TTree* m_tree;

  /// Start time for data used for this calib
  Double_t m_startTime;

  /// End time for data used for this calib
  Double_t m_endTime;

  /// The input files for this calib
  std::list< std::pair<std::string,std::string> > m_inputs;
  
  /// Number of triggers used for this calib
  UInt_t m_triggers;  

  /// The calibration of the individual channels
  std::map<UInt_t,CalibData::AcdCalibObj*> m_map;

  /// the name of the file associated with this calibration
  mutable std::string m_fileName;  

};

#endif

