
#ifndef AcdHtmlReport_h
#define AcdHtmlReport_h

#include "Rtypes.h"

// stl includes
#include <string>
#include <list>

#include <xercesc/util/XercesDefs.hpp>
#include "xercesc/dom/DOMElement.hpp"
using XERCES_CPP_NAMESPACE_QUALIFIER DOMElement;

namespace CalibData {
  class AcdCalibDescription;
  class AcdCalibObj;
}

class DomElement;
class TTree;
class AcdCalibMap;

/** 
 * @class AcdHtmlReport
 *
 * @brief 
 *
 * @author Eric Charles
 * $Header$
 */



class AcdHtmlReport {
public:

  /// Build from a calibration description
  AcdHtmlReport( const AcdCalibMap& calib );
  /// Null c'tor
  virtual ~AcdHtmlReport();


  /// Write calibration to an xml file
  Bool_t writeHtmlReport( const char* outputPrefix, const char* timeStamp ) const; 

protected:

  ///
  Bool_t makeSummaryPlots( const char* outputPrefix, TTree* inTree, 
			   std::list<std::string>& sumPlotsNames ) const;

  ///
  Bool_t makeDeltaPlots( const char* outputPrefix, TTree* inTree, TTree* refTree,
			 std::list<std::string>& delPlotsNames ) const;
  

  /// 
  Bool_t writeHtmlHeader( DomElement& outNode, const std::string& tString ) const;
  
  /// 
  Bool_t writeInputFileInfo( DomElement& outNode ) const;

  ///
  Bool_t writeCalibInfo( DomElement& outNode, const AcdCalibMap& calib ) const;

  ///
  Bool_t writeOutputFileInfo( DomElement& outNode ) const;

  ///
  Bool_t writeHistory( DomElement& outNode ) const; 

  ///
  Bool_t writeSummaryPlots( DomElement& outNode, const std::list<std::string>& sumPlotsNames ) const;
 
  ///
  Bool_t writeDeltaPlots( DomElement& outNode, const std::list<std::string>& delPlotsNames ) const;
  
  ///
  Bool_t writeComments( DomElement& outNode, const char* timeStamp ) const;
    
  /// 
  void makeTitleString( std::string& tString, const char* outputPrefix) const;

  
private:  

  /// The calibration
  const AcdCalibMap* m_calib;

  /// Description of the calibration handled by this map
  const CalibData::AcdCalibDescription* m_desc;

};

#endif

#ifdef AcdHtmlReport_cxx

#endif // #ifdef AcdHtmlReport_cxx
