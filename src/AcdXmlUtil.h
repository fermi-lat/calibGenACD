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

  void getDtdFileName(std::string& dtdFileName);

  bool redirectLinkPath(std::string& path);

  void getSuffix( std::string& suffix, int calType );

  void getEventFileType( std::string& eventFileType, int cType);

  void getCalibElemName( std::string& calibElemName, int calType);

  void getCalibTypeName( std::string& calibTypeName, int calType);

  DomElement makeDocument(const char* topNode);
  DomElement makeChildNode(DomElement& domNode, const char* name);
  
  void addAttribute(DomElement& domNode, const char* name, const char* val);
  void addAttribute(DomElement& domNode, const char* name, const int& val);
  void addAttribute(DomElement& domNode, const char* name, const double& val);
  
  void addAttributeMET(DomElement& domNode, const char* name, const double& val);
  
  bool writeIt(DomElement& doc, const char* fileName);

  bool writeHtml(DomElement& doc, const char* fileName);

};

#endif
