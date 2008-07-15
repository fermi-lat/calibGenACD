#ifndef AcdXmlUtil_h
#define AcdXmlUtil_h 

#include <string>
#include "CalibData/Acd/AcdCalibEnum.h"

class DomElement;


/** 
 *
 * @brief A few function to help manage xml
 *
 */

namespace AcdXmlUtil {

  void getDtdFileName(std::string& dtdFileName);

  void getCalibElemName( std::string& calibElemName, AcdCalibData::CALTYPE calType);

  void getCalibTypeName( std::string& calibTypeName, AcdCalibData::CALTYPE calType);

  DomElement makeDocument(const char* topNode);
  DomElement makeChildNode(DomElement& domNode, const char* name);
  
  void addAttribute(DomElement& domNode, const char* name, const char* val);
  void addAttribute(DomElement& domNode, const char* name, const int& val);
  void addAttribute(DomElement& domNode, const char* name, const double& val);
  
  void addAttributeMET(DomElement& domNode, const char* name, const double& val);
  
  bool writeIt(DomElement& doc, const char* fileName);

};

#endif
