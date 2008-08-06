#ifndef AcdXmlUtil_h
#define AcdXmlUtil_h 

#include <string>
#include "CalibData/Acd/AcdCalibEnum.h"
#include "AcdCalibEnum.h"

class DomElement;


/** 
 *
 * @brief A few function to help manage xml
 *
 */

namespace AcdXmlUtil {

  /// Get the name of the DTD file: "$(CALIBUTILROOT)/xml/acdCalib_vNrM.dtd"
  void getDtdFileName(std::string& dtdFileName);

  /// Get the path for the monitoring report files
  bool redirectLinkPath(std::string& path);

  /// Get the suffix for the products, based on the calib type, ie "_ped", "_gain" ...
  void getSuffix( std::string& suffix, int calType );

  /// Get the name event file type: "Digi" or "Svac"
  void getEventFileType( std::string& eventFileType, int cType);

  /// Get the name of the XML element, based on the calib type, ie "acdPed", "acdGain"
  void getCalibElemName( std::string& calibElemName, int calType);

  /// Get the name of the Calibration type, ie "ACD_Pedestal", "ACD_Gain"
  void getCalibTypeName( std::string& calibTypeName, int calType);

  /// Get the name of the Calibration tree, ie "ACD_Pedestal", "ACD_Gain"
  void getCalibTreeName( std::string& calibTreeName, int calType);

  /// Make a DOMDocument and return the top level element node
  DomElement makeDocument(const char* topNode);

  /// Make a DOMElement with name and add it as a child to a domNode
  DomElement makeChildNode(DomElement& domNode, const char* name);
  
  /// Add an attribute to DOMElement
  void addAttribute(DomElement& domNode, const char* name, const char* val);
  /// Add an attribute to DOMElement
  void addAttribute(DomElement& domNode, const char* name, const int& val);
  /// Add an attribute to DOMElement
  void addAttribute(DomElement& domNode, const char* name, const double& val);

  /// Add a Mission Elapsed Time as an attribute to DOMElement in the format 238287711.9731
  void addAttributeMET(DomElement& domNode, const char* name, const double& val);
  
  /// Write the DOMDocument, embed the DTD
  bool writeIt(DomElement& doc, const char* fileName);

  /// Write the DOMDocument as HTML
  bool writeHtml(DomElement& doc, const char* fileName);

};

#endif
