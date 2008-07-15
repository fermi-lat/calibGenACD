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

void 
AcdXmlUtil::getCalibElemName( std::string& calibElemName, AcdCalibData::CALTYPE calType) {
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
  default:
    calibElemName = "calib";    
  }
}

void 
AcdXmlUtil::getCalibTypeName( std::string& calibElemName, AcdCalibData::CALTYPE calType) {
  switch ( calType ) {
  case AcdCalibData::PED_HIGH: 
     calibElemName = "ACD_Pedestal"; 
    break;
  case AcdCalibData::GAIN: 
    calibElemName = "ACD_Gain"; 
    break;
  case AcdCalibData::VETO: 
    calibElemName = "ACD_Veto"; 
    break;
  case AcdCalibData::RANGE: 
    calibElemName = "ACD_Range"; 
    break;
  case AcdCalibData::CNO: 
    calibElemName = "ACD_Cno"; 
    break;
  case AcdCalibData::HIGH_RANGE: 
    calibElemName = "ACD_HighRange"; 
    break;
  case AcdCalibData::COHERENT_NOISE: 
    calibElemName = "ACD_CoherentNoise"; 
    break;
  case AcdCalibData::RIBBON: 
    calibElemName = "ACD_Ribbon"; 
    break;    
  case AcdCalibData::PEDESTAL: 
    calibElemName = "ACD_HighPed"; 
    break;    
  case AcdCalibData::CARBON: 
    calibElemName = "ACD_Carbon"; 
    break;
  case AcdCalibData::VETO_FIT: 
    calibElemName = "ACD_VetoFit"; 
    break;    
  case AcdCalibData::CNO_FIT: 
    calibElemName = "ACD_CnoFit"; 
    break;    
  default:
    calibElemName = "ACD_Calib";    
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


#endif
