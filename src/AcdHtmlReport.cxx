#define AcdcalibMap_cxx

// base class
#include "AcdHtmlReport.h"

// local headers
#include "AcdCalibEnum.h"
#include "AcdCalibBase.h"
#include "AcdCalibVersion.h"
#include "AcdCalibMap.h"
#include "AcdKey.h"
#include "AcdXmlUtil.h"
#include "DomElement.h"
#include "AcdReportPlots.h"

// ROOT
#include <TTree.h>
#include <TFile.h>

// stl includes
#include <iostream>
#include <fstream>

#include <cassert>

#include "facilities/Util.h"
#include "facilities/Timestamp.h"

#include "CalibData/Acd/AcdCalibObj.h"
#include "xmlBase/Dom.h"
#include "xmlBase/XmlParser.h"
#include "xercesc/dom/DOMElement.hpp"
#include "xercesc/dom/DOMDocument.hpp"
#include "xercesc/dom/DOMImplementation.hpp"

AcdHtmlReport::AcdHtmlReport(const AcdCalibMap& calib)
  :m_calib(&calib),
   m_desc(calib.theDesc()){
}

AcdHtmlReport::~AcdHtmlReport() {
}


/// Write calibration to an xml file
Bool_t AcdHtmlReport::writeHtmlReport( const char* outputPrefix, const char* timeStamp) const {


  DomElement inElem;
  m_calib->readTree();
  TTree* inTree = m_calib->theTree();
  TTree* refTree(0);
  if ( inTree == 0 ) {
    std::cerr << "Input calibration doesn't have summary tree." << std::endl;
    return kFALSE;
  }

  const AcdCalibMap* ref = m_calib->theReference();
  if ( ref != 0 ) {
    refTree = ref->theTree();
  }

  std::list<std::string> sumPlotNames;
  std::list<std::string> delPlotNames;  
  if ( ! makeSummaryPlots(outputPrefix,inTree,sumPlotNames) ) {
    std::cerr << "Failed to make summary plots" << std::endl;
    return kFALSE;
  }
  if ( ! makeDeltaPlots(outputPrefix,inTree,refTree,delPlotNames) ) {
    std::cerr << "Failed to make delta plots" << std::endl;
    return kFALSE;  
  }
  
  std::string titleString;
  makeTitleString(titleString,outputPrefix);
  
  DomElement elem = AcdXmlUtil::makeDocument("html");
  if ( ! writeHtmlHeader(elem,titleString) ) {
    std::cerr << "Failed to write header" << std::endl;
    return kFALSE;
  }
  
  DomElement body = AcdXmlUtil::makeChildNode(elem,"body");  
  xmlBase::Dom::makeChildNodeWithContent(&(body()),"h1",titleString.c_str());
  
  if ( ! writeInputFileInfo(body) ) {
    std::cerr << "Failed to write input file info" << std::endl;
    return kFALSE;
  }
  if ( ! writeHistory(body) ) {
    std::cerr << "Failed to write use history" << std::endl;
    return kFALSE; 
  }
  if ( ! writeOutputFileInfo(body) ) {
    std::cerr << "Failed to write output file info" << std::endl;
    return kFALSE;
  }
  if ( ! writeSummaryPlots(body,sumPlotNames) ) {
    std::cerr << "Failed to write summary plot links" << std::endl;
    return kFALSE;
  }
  if ( ! writeDeltaPlots(body,delPlotNames) ){
    std::cerr << "Failed to write data plot links" << std::endl; 
    return kFALSE;
  }
  if ( ! writeComments(body,timeStamp) ) {
    std::cerr << "Failed to write comments" << std::endl;
    return kFALSE;
  }
  std::string fileName = outputPrefix; fileName += ".html";
  return AcdXmlUtil::writeHtml(elem,fileName.c_str());				    
}


///
Bool_t AcdHtmlReport::makeSummaryPlots( const char* outputPrefix, TTree* inTree, 
					std::list<std::string>& sumPlotsNames) const {
  if ( outputPrefix == 0 || inTree == 0 ) return kFALSE;
  sumPlotsNames.clear();
  return AcdReport::makeSummaryPlots( m_desc->calibType(), outputPrefix, inTree, sumPlotsNames);
}

///
Bool_t AcdHtmlReport::makeDeltaPlots( const char* outputPrefix, TTree* inTree, TTree* refTree,
				      std::list<std::string>& delPlotsNames) const {
  if ( outputPrefix == 0 || inTree == 0 ) return kFALSE;
  delPlotsNames.clear();
  if ( refTree == 0 ) return kTRUE;
  inTree->AddFriend(refTree,"old");
  return AcdReport::makeDeltaPlots( m_desc->calibType(), outputPrefix, inTree, delPlotsNames);
}
 
Bool_t AcdHtmlReport::writeHtmlHeader( DomElement& outNode, const std::string& titleString) const {  

  DOMElement* headNode = xmlBase::Dom::makeChildNode(&(outNode()),"head");
  xmlBase::Dom::makeChildNodeWithContent(headNode,"title",titleString.c_str());
  return kTRUE;
}

Bool_t AcdHtmlReport::writeInputFileInfo( DomElement& outNode ) const {

  xmlBase::Dom::makeChildNodeWithContent(&(outNode()),"h3","Input:");
  return writeCalibInfo(outNode,*m_calib);
}

///
Bool_t AcdHtmlReport::writeCalibInfo( DomElement& outNode, const AcdCalibMap& calib ) const {
  
  std::vector<DOMElement*> inputFiles;  

  xmlBase::Dom::makeChildNodeWithContent(&(outNode()),"h5","Files:");
  DOMElement* ul = xmlBase::Dom::makeChildNode(&(outNode()),"ul");

  const  std::list<std::pair<std::string,std::string> >& inputs = calib.inputs();
  for ( std::list<std::pair<std::string,std::string> >::const_iterator itr = inputs.begin(); 
	itr!= inputs.end(); itr++ ) {
    std::string thePath = itr->first;
    std::string fileString = "Type: ";
    fileString += itr->second;
    fileString += ".  Path: ";
    fileString += thePath;    ;
    if ( AcdXmlUtil::redirectLinkPath(thePath) ) {
      DOMElement* li = xmlBase::Dom::makeChildNode(ul,"li");
      DOMElement* aFile = xmlBase::Dom::makeChildNodeWithContent(li,"a",fileString.c_str());
      xmlBase::Dom::addAttribute(aFile,"href",thePath.c_str());
    } else {
      xmlBase::Dom::makeChildNodeWithContent(ul,"li",fileString.c_str());
    }
  }
  
  char timeString[100];
  sprintf(timeString,"Time Span (MET seconds): %13.4f - %13.4f",calib.startTime(),calib.endTime());
  xmlBase::Dom::makeChildNodeWithContent(&(outNode()),"h5",timeString);

  char trigString[50];
  sprintf(trigString,"Triggers used: %d",calib.nTriggers());
  xmlBase::Dom::makeChildNodeWithContent(&(outNode()),"h5",trigString);
  xmlBase::Dom::makeChildNode(&(outNode()),"hr");
  return kTRUE;
}



///
Bool_t AcdHtmlReport::writeOutputFileInfo( DomElement& outNode ) const {

  xmlBase::Dom::makeChildNodeWithContent(&(outNode()),"h3","Output:");
  DOMElement* ul = xmlBase::Dom::makeChildNode(&(outNode()),"ul");
  DOMElement* liFile = xmlBase::Dom::makeChildNode(ul,"li");
  DOMElement* aFile = xmlBase::Dom::makeChildNodeWithContent(liFile,"a","Calibration XML");
  xmlBase::Dom::addAttribute(aFile,"href",m_calib->fileName());

  DOMElement* liPlot = xmlBase::Dom::makeChildNode(ul,"li");
  DOMElement* aPlot = xmlBase::Dom::makeChildNodeWithContent(liPlot,"a","analysisDir");
  xmlBase::Dom::addAttribute(aPlot,"href",".");
  xmlBase::Dom::makeChildNode(&(outNode()),"hr");
  return kTRUE;
}

///
Bool_t AcdHtmlReport::writeHistory( DomElement& outNode ) const {
  xmlBase::Dom::makeChildNodeWithContent(&(outNode()),"h3","Use History");
  DOMElement* ul = xmlBase::Dom::makeChildNode(&(outNode()),"ul");
  xmlBase::Dom::makeComment(ul,"USE__HOOK");
  xmlBase::Dom::makeChildNode(&(outNode()),"hr");
  return kTRUE;
}



///
Bool_t AcdHtmlReport::writeSummaryPlots( DomElement& outNode,
					 const std::list<std::string>& sumPlotsNames) const {
  xmlBase::Dom::makeChildNodeWithContent(&(outNode()),"h3","Summary Plots");
  for ( std::list<std::string>::const_iterator itr = sumPlotsNames.begin();
	itr != sumPlotsNames.end(); itr++ ) {
    //if ( itr->find(".gif") == itr->npos ) {
    if ( itr->find(".png") == itr->npos ) {
      //if ( itr->find(".pdf") == itr->npos ) {
      xmlBase::Dom::makeChildNode(&(outNode()),itr->c_str());
      continue;
    }
    DOMElement* img = xmlBase::Dom::makeChildNode(&(outNode()),"img");
    xmlBase::Dom::addAttribute(img,"height","250");
    xmlBase::Dom::addAttribute(img,"src",itr->c_str());
  }
  xmlBase::Dom::makeChildNode(&(outNode()),"hr");
  return kTRUE;
}

///
Bool_t AcdHtmlReport::writeDeltaPlots( DomElement& outNode, 
				       const std::list<std::string>& delPlotsNames ) const {
  xmlBase::Dom::makeChildNodeWithContent(&(outNode()),"h3","Plots with respect to reference calibration");
  for ( std::list<std::string>::const_iterator itr = delPlotsNames.begin();
	itr != delPlotsNames.end(); itr++ ) {
    //if ( itr->find(".gif") == itr->npos ) {
    if ( itr->find(".png") == itr->npos ) {
      //if ( itr->find(".pdf") == itr->npos ) {
      DOMElement* img = xmlBase::Dom::makeChildNode(&(outNode()),itr->c_str());
      continue;
    }
    DOMElement* img = xmlBase::Dom::makeChildNode(&(outNode()),"img");
    xmlBase::Dom::addAttribute(img,"height","250");
    xmlBase::Dom::addAttribute(img,"src",itr->c_str());
  }
  xmlBase::Dom::makeChildNode(&(outNode()),"hr");
  const AcdCalibMap* refCalib = m_calib->theReference();
  if ( refCalib == 0 ) return kTRUE;  
  xmlBase::Dom::makeChildNodeWithContent(&(outNode()),"h4","Reference Calibration");  

  std::string thePath = refCalib->fileName();
  AcdXmlUtil::redirectLinkPath(thePath);
  DOMElement* aFile = xmlBase::Dom::makeChildNodeWithContent(&(outNode()),"a",refCalib->fileName());
  xmlBase::Dom::addAttribute(aFile,"href",thePath.c_str());
  return writeCalibInfo(outNode,*refCalib);
}

///
Bool_t AcdHtmlReport::writeComments( DomElement& outNode, const char* timeStamp  ) const {
  xmlBase::Dom::makeChildNodeWithContent(&(outNode()),"h3","Comments");
  DOMElement* ul = xmlBase::Dom::makeChildNode(&(outNode()),"ul");
  std::string stampString("Report made at  ");
  stampString += timeStamp;
  stampString += " by user $(USER)";
  facilities::Util::expandEnvVar(&stampString);    
  xmlBase::Dom::makeChildNodeWithContent(ul,"li",stampString.c_str());
  xmlBase::Dom::makeComment(ul,"COMMENT__HOOK");
  return kTRUE;
}

void AcdHtmlReport::makeTitleString( std::string& tString, const char* outputPrefix) const{
  tString = m_desc->calibTypeName();
  tString += " Calibration ";
  tString += outputPrefix;
}
