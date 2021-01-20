
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
 * @brief Class to generate HTML reports for calibrations
 *
 * The productions reports are located at 
 *   http://www.slac.stanford.edu/exp/glast/acd/calib_reports/index.html
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdHtmlReport.h,v 1.3 2008/08/06 17:46:09 echarles Exp $
 */



class AcdHtmlReport {
public:

  /// Build from a calibration description
  AcdHtmlReport( const AcdCalibMap& calib );
  /// Null c'tor
  virtual ~AcdHtmlReport();


  /**
   * @brief write an html report about the calibration   
   * @param outputPrefix is the prefix for the output html file and associated gifs
   * @param timeStamp is the date/time string used to mark the report
   * @param isCheckCalib is true if the calibration is a check, not a real calibration
   * @return kTRUE for success, kFALSE otherwise
   **/
  Bool_t writeHtmlReport( const char* outputPrefix, const char* timeStamp) const; 

protected:

  /// Make all the summary plots for the config
  Bool_t makeSummaryPlots( const char* outputPrefix, TTree* inTree, 
			   std::list<std::string>& sumPlotsNames) const;

  /// Make all the plots showing deltas w.r.t. reference values
  Bool_t makeDeltaPlots( const char* outputPrefix, TTree* inTree, TTree* refTree,
			 std::list<std::string>& delPlotsNames) const;
  

  /// Write the html header
  Bool_t writeHtmlHeader( DomElement& outNode, const std::string& tString ) const;
  
  /// Write the information about the calibration input files
  Bool_t writeInputFileInfo( DomElement& outNode ) const;

  /// Write the information about the data used in the calibration
  Bool_t writeCalibInfo( DomElement& outNode, const AcdCalibMap& calib ) const;

  /// Write the links to the output data and plots
  Bool_t writeOutputFileInfo( DomElement& outNode ) const;

  /// Write a hook to add the used history of the calibration
  Bool_t writeHistory( DomElement& outNode ) const; 

  /// Write the links to the summary plots
  Bool_t writeSummaryPlots( DomElement& outNode, const std::list<std::string>& sumPlotsNames ) const;
 
  /// Write the links to the delta plots
  Bool_t writeDeltaPlots( DomElement& outNode, const std::list<std::string>& delPlotsNames ) const;
  
  /// Write a hood to add comments about this calibration
  Bool_t writeComments( DomElement& outNode, const char* timeStamp ) const;
    
  /// Form the string used as the title of the html page
  void makeTitleString( std::string& tString, const char* outputPrefix) const;

  
private:  

  /// The calibration
  const AcdCalibMap* m_calib;

  /// Description of the calibration handled by this map
  const CalibData::AcdCalibDescription* m_desc;

};

#endif

