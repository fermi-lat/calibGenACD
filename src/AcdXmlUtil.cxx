#ifndef AcdXmlUtil_cxx
#define AcdXmlUtil_cxx
#include "AcdXmlUtil.h"

#include "xmlBase/Dom.h"
#include "DomElement.h"

#include "AcdCalibVersion.h"

#include <fstream>

#include "facilities/Util.h"
#include <xercesc/util/XercesDefs.hpp>

#include "xercesc/dom/DOMElement.hpp"
#include "xercesc/dom/DOMDocument.hpp"

#include "xercesc/util/XercesDefs.hpp"
#include <xercesc/util/XMLString.hpp>


void
AcdXmlUtil::getDtdFileName(std::string& dtdFileName) {
  std::string dtdPath("$(CALIBUTILROOT)/xml/acdCalib_");
  dtdPath += AcdCalibVersion::dtdVersion();
  dtdPath += ".dtd";  
  facilities::Util::expandEnvVar(&dtdPath);
  dtdFileName = dtdPath;
}

bool 
AcdXmlUtil::redirectLinkPath(std::string& path) {
  static std::string searchForXml(".xml");
  static std::string searchForDir("/ACD/FLIGHT/");
  std::string::size_type found = path.find(searchForXml);
  if ( found == path.npos ) {
    return false;
  }
  path.replace(found,found+searchForXml.size(),".html");  
  found = path.find(searchForDir);
  if ( found != path.npos ) {
    path.erase(0,found);  
  }
  return true;
}

void 
AcdXmlUtil::getSuffix( std::string& suffix, int calType) {
  switch ( calType ) {
  case AcdCalibData::PEDESTAL: 
    suffix = "_ped"; 
    break;
  case AcdCalibData::GAIN: 
    suffix = "_gain"; 
    break;
  case AcdCalibData::VETO: 
    suffix = "_veto"; 
    break;
  case AcdCalibData::RANGE: 
    suffix = "_range"; 
    break;
  case AcdCalibData::CNO: 
    suffix = "_cno"; 
    break;
  case AcdCalibData::HIGH_RANGE: 
    suffix = "_highRange"; 
    break;
  case AcdCalibData::COHERENT_NOISE: 
    suffix = "_coherentNoise"; 
    break;
  case AcdCalibData::RIBBON: 
    suffix = "_ribbon"; 
    break;    
  case AcdCalibData::PED_HIGH: 
    suffix = "_pedHigh"; 
    break;
  case AcdCalibData::CARBON: 
    suffix = "_carbon"; 
    break;
  case AcdCalibData::VETO_FIT: 
    suffix = "_vetoFit"; 
    break;
  case AcdCalibData::CNO_FIT: 
    suffix = "_cnoFit"; 
    break;
  case AcdCalibData::MERITCALIB: 
    suffix = "_check"; 
    break;
  default:
    suffix = "_calib";    
  }
}

void 
AcdXmlUtil::getEventFileType( std::string& eventFileType, int cType) {
  switch (cType) {
  case AcdCalib::DIGI:  eventFileType = "Digi";  break;
  case AcdCalib::SVAC:  eventFileType = "Svac";  break;
  default:
    return;
  }
}

void 
AcdXmlUtil::getCalibElemName( std::string& calibElemName, int calType) {
  switch ( calType ) {
  case AcdCalibData::PEDESTAL: 
    calibElemName = "acdPed"; 
    break;
  case AcdCalibData::GAIN: 
    calibElemName = "acdGain"; 
    break;
  case AcdCalibData::VETO: 
    calibElemName = "acdVeto"; 
    break;
  case AcdCalibData::RANGE: 
    calibElemName = "acdRange"; 
    break;
  case AcdCalibData::CNO: 
    calibElemName = "acdCno"; 
    break;
  case AcdCalibData::HIGH_RANGE: 
    calibElemName = "acdHighRange"; 
    break;
  case AcdCalibData::COHERENT_NOISE: 
    calibElemName = "acdCoherentNoise"; 
    break;
  case AcdCalibData::RIBBON: 
    calibElemName = "acdRibbon"; 
    break;    
  case AcdCalibData::PED_HIGH: 
    calibElemName = "acdHighPed"; 
    break;
  case AcdCalibData::CARBON: 
    calibElemName = "acdCarbon"; 
    break;
  case AcdCalibData::VETO_FIT: 
    calibElemName = "acdVetoFit"; 
    break;
  case AcdCalibData::CNO_FIT: 
    calibElemName = "acdCnoFit"; 
    break;
  case AcdCalibData::MERITCALIB: 
    calibElemName = "acdCheck"; 
    break;
  default:
    calibElemName = "calib";    
  }
}

void 
AcdXmlUtil::getCalibTypeName( std::string& calibTreeName, int calType) {
  switch ( calType ) {
  case AcdCalibData::PED_HIGH: 
    calibTreeName = "ACD_HighPed"; 
    break;
  case AcdCalibData::GAIN: 
    calibTreeName = "ACD_Gain"; 
    break;
  case AcdCalibData::VETO: 
    calibTreeName = "ACD_Veto"; 
    break;
  case AcdCalibData::RANGE: 
    calibTreeName = "ACD_Range"; 
    break;
  case AcdCalibData::CNO: 
    calibTreeName = "ACD_Cno"; 
    break;
  case AcdCalibData::HIGH_RANGE: 
    calibTreeName = "ACD_HighRange"; 
    break;
  case AcdCalibData::COHERENT_NOISE: 
    calibTreeName = "ACD_CoherentNoise"; 
    break;
  case AcdCalibData::RIBBON: 
    calibTreeName = "ACD_Ribbon"; 
    break;    
  case AcdCalibData::PEDESTAL: 
    calibTreeName = "ACD_Pedestal"; 
    break;    
  case AcdCalibData::CARBON: 
    calibTreeName = "ACD_Carbon"; 
    break;
  case AcdCalibData::VETO_FIT: 
    calibTreeName = "ACD_VetoFit"; 
    break;    
  case AcdCalibData::CNO_FIT: 
    calibTreeName = "ACD_CnoFit"; 
    break;    
  case AcdCalibData::MERITCALIB: 
    calibTreeName = "ACD_Check"; 
    break;    
  default:
    calibTreeName = "ACD_Calib";    
  }
}

void 
AcdXmlUtil::getCalibTreeName( std::string& calibTreeName, int calType) {
  switch ( calType ) {
  case AcdCalibData::PED_HIGH: 
    calibTreeName = "ACD_HighPed"; 
    break;
  case AcdCalibData::GAIN: 
    calibTreeName = "ACD_Gain"; 
    break;
  case AcdCalibData::VETO: 
    calibTreeName = "ACD_Veto"; 
    break;
  case AcdCalibData::RANGE: 
    calibTreeName = "ACD_Range"; 
    break;
  case AcdCalibData::CNO: 
    calibTreeName = "ACD_Cno"; 
    break;
  case AcdCalibData::HIGH_RANGE: 
    calibTreeName = "ACD_HighRange"; 
    break;
  case AcdCalibData::COHERENT_NOISE: 
    calibTreeName = "ACD_CoherentNoise"; 
    break;
  case AcdCalibData::RIBBON: 
    calibTreeName = "ACD_Ribbon"; 
    break;    
  case AcdCalibData::PEDESTAL: 
    calibTreeName = "ACD_Ped"; 
    break;    
  case AcdCalibData::CARBON: 
    calibTreeName = "ACD_Carbon"; 
    break;
  case AcdCalibData::VETO_FIT: 
    calibTreeName = "ACD_VetoFit"; 
    break;    
  case AcdCalibData::CNO_FIT: 
    calibTreeName = "ACD_CnoFit"; 
    break;    
  case AcdCalibData::MERITCALIB: 
    calibTreeName = "ACD_Check"; 
    break;    
  default:
    calibTreeName = "ACD_Calib";    
  }
}

DomElement
AcdXmlUtil::makeDocument(const char* name){
  using XERCES_CPP_NAMESPACE_QUALIFIER DOMElement;
  DOMElement* docElem = xmlBase::Dom::makeDocument(name);
  return DomElement(*docElem);
}

DomElement 
AcdXmlUtil::makeChildNode(DomElement& domNode, const char* name) {

  using XERCES_CPP_NAMESPACE_QUALIFIER DOMElement;
  DOMElement* newChild = xmlBase::Dom::makeChildNode(&(domNode()),name);
  return DomElement(*newChild);
}

void  
AcdXmlUtil::addAttribute(DomElement& domNode, const char* name, const char* val) {
  xmlBase::Dom::addAttribute(&(domNode()),std::string(name),val);
}

void  
AcdXmlUtil::addAttribute(DomElement& domNode, const char* name, const int& val) {
  xmlBase::Dom::addAttribute(&(domNode()),std::string(name),val);
}

void  
AcdXmlUtil::addAttribute(DomElement& domNode, const char* name, const double& val) {
  xmlBase::Dom::addAttribute(&(domNode()),std::string(name),val);
}

void  
AcdXmlUtil::addAttributeMET(DomElement& domNode, const char* name, const double& val) {
  char temp[30];
  sprintf(temp,"%014.4f",val);
  xmlBase::Dom::addAttribute(&(domNode()),std::string(name),temp);
}

bool  
AcdXmlUtil::writeIt(DomElement& doc,const char* fileName) {
  std::ofstream fout(fileName,std::ios::out);
  std::string dtdFileName;
  getDtdFileName(dtdFileName);
  std::cout << dtdFileName << std::endl;
  std::ifstream dtdfile(dtdFileName.c_str(),std::ios::in);  
  if (!dtdfile.is_open()) { 
    std::cerr << "dtd file not found!" << std::endl;
    return false;
  }
  std::string buffer;
  fout << "<!DOCTYPE acdCalib [" << std::endl;
  while (!dtdfile.eof()) {
    getline(dtdfile,buffer);
    fout << buffer << std::endl;
  }
  dtdfile.close();
  fout << "] >" << std::endl;  
  xmlBase::Dom::prettyPrintElement(&(doc()),fout,"");
  fout.close();
  return true;
}

bool  
AcdXmlUtil::writeHtml(DomElement& doc,const char* fileName) {
  xmlBase::Dom::writeIt(&(doc()),fileName); 
  return true;
}


#endif
