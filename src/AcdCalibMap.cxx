#define AcdcalibMap_cxx

// base class
#include "AcdCalibMap.h"

// local headers
#include "AcdCalibEnum.h"
#include "AcdCalibBase.h"
#include "AcdCalibVersion.h"
#include "AcdMap.h"
#include "AcdXmlUtil.h"
#include "DomElement.h"
#include "AcdHistCalibMap.h"
#include "CalibData/Acd/AcdCalibObj.h"

// ROOT
#include <TTree.h>
#include <TFile.h>

// stl includes
#include <iostream>
#include <fstream>
#include <cassert>

#include "xmlBase/Dom.h"
#include "xmlBase/XmlParser.h"
#include "xercesc/dom/DOMElement.hpp"
#include "xercesc/dom/DOMDocument.hpp"
#include "xercesc/dom/DOMImplementation.hpp"

AcdCalibMap::AcdCalibMap(const CalibData::AcdCalibDescription& desc)
  :m_desc(&desc){;}

AcdCalibMap::AcdCalibMap()
  :m_desc(0){;}

AcdCalibMap::~AcdCalibMap() {
  for ( std::map<UInt_t,CalibData::AcdCalibObj*>::iterator itr = m_map.begin();
	itr != m_map.end(); itr++ ) {
    delete itr->second;
  }
  m_map.clear();
}

void AcdCalibMap::add(UInt_t key, CalibData::AcdCalibObj& result) {
  std::map<UInt_t,CalibData::AcdCalibObj*>::const_iterator itr = m_map.find(key);
  if ( itr != m_map.end() ) {
    // warn
    ;
  }
  m_map[key] = &result;
}

const CalibData::AcdCalibObj* AcdCalibMap::get(UInt_t key) const {
  std::map<UInt_t,CalibData::AcdCalibObj*>::const_iterator itr = m_map.find(key);
  return itr == m_map.end()  ? 0 : itr->second;
}

CalibData::AcdCalibObj* AcdCalibMap::get(UInt_t key) {
  std::map<UInt_t,CalibData::AcdCalibObj*>::iterator itr = m_map.find(key);
  return itr == m_map.end()  ? 0 : itr->second;
}

CalibData::AcdCalibObj* AcdCalibMap::makeNew() const {
  static const std::vector<float> nullVect;
  return new CalibData::AcdCalibObj(CalibData::AcdCalibObj::NOFIT,nullVect,*m_desc);
}

Bool_t AcdCalibMap::writeTxtFile(const char* fileName,
				 const char* instrument,
				 const char* timestamp,
				 const char* algorithm,
				 const AcdCalibBase& calib) {
  std::ofstream os(fileName);
  if ( !os.good() ) {
    std::cerr << "Problems opening text output file " << fileName << std::endl;
    return kFALSE;
  }
  using std::endl;
  os << "#SYSTEM = " << "acdCalib" << endl
     << "#instrument= " << instrument << endl
     << "#timestamp = " << timestamp << endl
     << "#calibType = " << m_desc->calibType() << endl
     << "#algorithm = " << algorithm << endl
     << "#DTDVersion = " << AcdCalibVersion::dtdVersion() << endl
     << "#fmtVersion = " << AcdCalibVersion::fmtVersion() << endl;
  calib.writeTxtHeader(os);
  // this line is needed as a tag
  os << "#START" << endl;
  // skip this line
  //os << m_desc->txtFormat() << endl;
    
  writeTxt(os);
  os.close();
  return kTRUE;
}

void AcdCalibMap::writeTxt(ostream& os) const {
  for(int iFace = 0; iFace != AcdMap::nFace; ++iFace) {
    for(int iRow = 0; iRow != (int)AcdMap::getNRow(iFace); ++iRow) {
      for(int iCol = 0; iCol != (int)AcdMap::getNCol(iFace,iRow); ++iCol) {	
	if ( ! AcdMap::channelExists(iFace,iRow,iCol) ) continue;
	for(int iPmt = 0; iPmt != AcdMap::nPmt; ++iPmt) {

	  UInt_t id = AcdMap::makeId(iFace,iRow,iCol);
	  UInt_t key = AcdMap::makeKey(iPmt,iFace,iRow,iCol);
	  std::map<UInt_t,CalibData::AcdCalibObj*>::const_iterator itr = m_map.find(key);   
	  if ( itr == m_map.end() ) continue;

	  os << id << ' ' << iPmt << ' ';
	  itr->second->printTxtLine(os,*m_desc);
	  os << std::endl;
	}
      }
    }
  }
}


Bool_t AcdCalibMap::writeXmlFile(const char* fileName,
				 const char* instrument,
				 const char* timestamp,
				 const char* algorithm,
				 const AcdCalibBase& calib) const {
  
  

  DomElement elem = AcdXmlUtil::makeDocument("acdCalib");
  writeXmlHeader(elem,instrument,timestamp,algorithm,calib);
  writeXmlBody(elem);
  writeXmlFooter(elem);
  
  return AcdXmlUtil::writeIt(elem,fileName);
}

void AcdCalibMap::writeXmlHeader(DomElement& node,
				 const char* instrument,
				 const char* timestamp,
				 const char* algorithm,
				 const AcdCalibBase& calib) const {  

  DomElement genNode = AcdXmlUtil::makeChildNode(node,"generic");
  
  AcdXmlUtil::addAttribute(genNode,"instrument",instrument);
  AcdXmlUtil::addAttribute(genNode,"timestamp",timestamp);
  std::string calibTypeName;
  AcdXmlUtil::getCalibTypeName(calibTypeName,m_desc->calibType());
  AcdXmlUtil::addAttribute(genNode,"calibType",calibTypeName.c_str());
  AcdXmlUtil::addAttribute(genNode,"algorithm",algorithm);
  AcdXmlUtil::addAttribute(genNode,"DTDVersion",AcdCalibVersion::dtdVersion());
  AcdXmlUtil::addAttribute(genNode,"fmtVersion",AcdCalibVersion::fmtVersion());  
  calib.writeXmlHeader(genNode);

  DomElement dimNode = AcdXmlUtil::makeChildNode(node,"dimension");
  AcdXmlUtil::addAttribute(dimNode,"nTile",(int)108);
}


void AcdCalibMap::writeXmlFooter(DomElement& /* node */) const {
  return;
}

void AcdCalibMap::writeXmlBody(DomElement& node) const {

  std::string calibElemName;
  AcdXmlUtil::getCalibElemName(calibElemName,m_desc->calibType());

  for(int iFace = 0; iFace != AcdMap::nFace; iFace++) {
    for(int iRow = 0; iRow != (int)AcdMap::getNRow(iFace); ++iRow) {
      for(int iCol = 0; iCol != (int)AcdMap::getNCol(iFace,iRow); ++iCol) {	
	if ( ! AcdMap::channelExists(iFace,iRow,iCol) ) continue;

	UInt_t id = AcdMap::makeId(iFace,iRow,iCol);
	DomElement tileNode = AcdXmlUtil::makeChildNode(node,"tile");
	char tileStr[4];
	sprintf(tileStr,"%03d",id);
	AcdXmlUtil::addAttribute(tileNode,"tileId",tileStr);

	for(int iPmt = 0; iPmt != AcdMap::nPmt; iPmt++) {
	  DomElement pmtNode = AcdXmlUtil::makeChildNode(tileNode,"pmt");
	  AcdXmlUtil::addAttribute(pmtNode,"iPmt",(int)iPmt);
	  
	  UInt_t key = AcdMap::makeKey(iPmt,iFace,iRow,iCol);
	  std::map<UInt_t,CalibData::AcdCalibObj*>::const_iterator itr = m_map.find(key);  
	  if ( itr == m_map.end() ) continue;	  

	  DomElement calibNode = AcdXmlUtil::makeChildNode(pmtNode,calibElemName.c_str());
	  for ( Int_t iVar(0); iVar < m_desc->nVar(); iVar++ ) {
	    const std::string& vName = m_desc->getVarName(iVar);
	    Float_t varVal = itr->second->operator[](iVar);
	    AcdXmlUtil::addAttribute(calibNode,vName.c_str(),varVal);
	  }
	  AcdXmlUtil::addAttribute(calibNode,"status",(int)itr->second->getStatus());
	}
      }
    }
  }
}


Bool_t AcdCalibMap::writeResultsToTree(const char* newFileName) {
  TFile * histFile(0);
  TTree* newTree(0);
  UInt_t nVal = m_desc->nVar();
  Float_t* vals = new Float_t[nVal];
  UInt_t id;
  UInt_t pmt;
  Int_t status;
  if ( newFileName != 0 ) {
    histFile = TFile::Open(newFileName, "RECREATE");
    if ( histFile == 0 ) return kFALSE;
    newTree = new TTree(m_desc->calibTypeName().c_str(),"Calibration");    
  }
  if( histFile == 0 || newTree == 0 ) return kFALSE;
  newTree->Branch("id",&id,"id/i");
  newTree->Branch("pmt",&pmt,"pmt/i");
  newTree->Branch("status",&status,"status/I");
  for ( UInt_t i(0); i < nVal; i++ ) {
    std::string vName = m_desc->getVarName(i);
    std::string lName = vName + "/F";
    newTree->Branch(vName.c_str(),&(vals[i]),lName.c_str());
  }
  for ( std::map<UInt_t,CalibData::AcdCalibObj*>::const_iterator itr = m_map.begin(); 
	itr != m_map.end(); itr++ ) {
    id = AcdMap::getId(itr->first);
    pmt =  AcdMap::getPmt(itr->first);
    status = itr->second->getStatus();
    for ( UInt_t iV(0); iV < nVal; iV++ ) {
      vals[iV] = itr->second->operator[](iV);
    }
    newTree->Fill();
  }    
  newTree->Write();
  histFile->Close();
  delete [] vals;
  return kTRUE;
}


Bool_t AcdCalibMap::readXmlFile(const char* fileName) {
  using XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument;

  xmlBase::XmlParser parser;
  static const std::string tileName("tile");
  DOMDocument* doc = parser.parse(fileName);
  if ( doc == 0 ) return kFALSE;
  std::vector<DOMElement*> tiles;
  DOMElement* top = doc->getDocumentElement();					       
  xmlBase::Dom::getDescendantsByTagName(top,tileName,tiles);
  for ( std::vector<DOMElement*>::iterator itr = tiles.begin(); itr!= tiles.end(); itr++ ) {
    if ( ! readXmlTile(*(*itr) ) ) return kFALSE;
  }
  return kTRUE;
}

Bool_t AcdCalibMap::readXmlTile(DOMElement& node) {

  Int_t id = xmlBase::Dom::getIntAttribute(&node,"tileId");
  if ( id >= 1000 ) return kTRUE;
  std::vector<DOMElement*> pmts;
  static const std::string pmtName("pmt");
  xmlBase::Dom::getDescendantsByTagName(&node,pmtName,pmts);
  DOMElement* pmtA = pmts[0];
  DOMElement* pmtB = pmts[1];
  UInt_t keyA = AcdMap::makeKey(0,id);
  UInt_t keyB = AcdMap::makeKey(1,id);

  if ( ! readXmlCalib(*pmtA,keyA) ) {
    std::cerr << "Failed to read " << keyA << std::endl;
    return kFALSE;
  }
  if ( ! readXmlCalib(*pmtB,keyB) ) {
    std::cerr << "Failed to read " << keyB << std::endl;
    return kFALSE;    
  }
  return kTRUE;
    
}

Bool_t AcdCalibMap::readXmlCalib(DOMElement& node, UInt_t key) {

  // Element we're interested in is child of <pmt>
  DOMElement* calibElt = xmlBase::Dom::getFirstChildElement(&node);
  
  // Could check here to make sure it really is an <acdPed>
  int status;
  std::vector<float> vals;
  try {    
    status = xmlBase::Dom::getIntAttribute(calibElt, "status");
    for ( int i(0); i < m_desc->nVar(); i++ ) {
      float varVal = xmlBase::Dom::getDoubleAttribute(calibElt, m_desc->getVarName(i).c_str());
      vals.push_back(varVal);
    }
  }
  catch (xmlBase::DomException ex) {
    std::cerr << "From CalibSvc::XmlAcdCnv::processPmt" << std::endl;
    std::cerr << ex.getMsg() << std::endl;
    throw ex;
  }

  CalibData::AcdCalibObj* result = makeNew();
  assert( result != 0 );
  result->setVals(vals,(CalibData::AcdCalibObj::STATUS)status);
  m_map[key] = result;
  return kTRUE;
}



Bool_t AcdCalibMap::readTxtFile(const char* fileName) {
  std::ifstream infile(fileName);
  if ( ! infile.good() ) return kFALSE;

  bool foundTag(kFALSE);
  char formatLine[512];
  while ( ! infile.eof() ) {
    infile.getline(formatLine,512);
    if ( !infile.good() ) return kFALSE;
    std::string aLine(&formatLine[0]);
    //look for the tag to start
    if ( aLine.find("#START") != std::string::npos) {
      foundTag = kTRUE;      
      break;
    }
  }

  if ( !foundTag ) return kFALSE;
  infile.getline(formatLine,512);

  Bool_t result = readTxt(infile);
  infile.close();

  m_fileName = fileName;
  return result;
}

Bool_t AcdCalibMap::readTxt(istream& is) {
  int id, iPmt;
  while ( ! is.eof() ) { 

    is >> id >> iPmt;
    if ( !is.good() ) return kFALSE;
    UInt_t key = AcdMap::makeKey(iPmt,id);

    CalibData::AcdCalibObj* result = makeNew();
    assert( result != 0 );

    result->readTxt(is,*m_desc);
    if ( !is.good() ) return kFALSE;
    
    m_map[key] = result;
  }
  return kTRUE;
}
