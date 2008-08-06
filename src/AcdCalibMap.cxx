#define AcdcalibMap_cxx

// base class
#include "AcdCalibMap.h"

// local headers
#include "AcdCalibEnum.h"
#include "AcdCalibBase.h"
#include "AcdCalibVersion.h"
#include "AcdKey.h"
#include "AcdCalibUtil.h"
#include "AcdXmlUtil.h"
#include "DomElement.h"
#include "AcdHistCalibMap.h"
#include "AcdHtmlReport.h"
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
  :m_desc(&desc),
   m_reference(0),
   m_hists(0),
   m_tree(0),
   m_startTime(0),
   m_endTime(0),
   m_triggers(0){
}

AcdCalibMap::AcdCalibMap()
  :m_desc(0),
   m_reference(0),
   m_hists(0),
   m_tree(0),
   m_startTime(0),
   m_endTime(0),
   m_triggers(0){
}

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

Bool_t AcdCalibMap::writeOutputs(const std::string& outputPrefix, 
				 const std::string& algorithm,
				 const std::string& instrument,
				 const std::string& timestamp,
				 bool isCheckCalib ) {
  std::string suffix;
  std::string op = outputPrefix;
  if ( isCheckCalib ) {
    op += "_check";
  }
  AcdXmlUtil::getSuffix(suffix,m_desc->calibType() );
  std::string txtFile = op + suffix + ".txt";
  std::string xmlFile = op + suffix + ".xml";
  std::string histFile = op + suffix + ".root";
  std::string sumFile = op + suffix + "Pars.root";
  std::string htmlFile = op + suffix;
  std::string plotFile = op + suffix + "_";
  
  if ( m_hists != 0 ) {
    if ( ! m_hists->writeHistograms( histFile.c_str() ) ) {
      std::cerr << "Failed to write histograms to " << histFile << std::endl;
      return kFALSE;
    }
  }
  if ( ! AcdCalibUtil::makeFitPlots(*this,plotFile.c_str()) ) {
    std::cerr << "Failed to write fit to " << plotFile << std::endl;
    return kFALSE;
  }

  // For check type calibrations all we want are the plots, so return
  if ( isCheckCalib ) return kTRUE;

  if ( ! writeTxtFile(txtFile.c_str(),instrument.c_str(),timestamp.c_str(),algorithm.c_str()) ) {
    std::cerr << "Failed to write txt file to " << txtFile << std::endl;
    return kFALSE;
  }
  if ( ! writeXmlFile(xmlFile.c_str(),instrument.c_str(),timestamp.c_str(),algorithm.c_str()) ) {
    std::cerr << "Failed to write xml file to " << xmlFile << std::endl;
    return kFALSE;
  }
  if ( ! writeResultsToTree(sumFile.c_str()) ) {
    std::cerr << "Failed to write summary results file to " << sumFile << std::endl;
    return kFALSE;
  }
  

  AcdHtmlReport html(*this);
  if ( ! html.writeHtmlReport(htmlFile.c_str(),timestamp.c_str()) ) {
    std::cerr << "Failed to write html report to " << htmlFile << std::endl;
    return kFALSE;
  }

  return kTRUE;
								
}


Bool_t AcdCalibMap::writeTxtFile(const char* fileName,
				 const char* instrument,
				 const char* timestamp,
				 const char* algorithm ) {
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
     << "#startTime = " << m_startTime << endl
     << "#endTime   = " << m_endTime << endl
     << "#triggers  = " << m_triggers << endl
     << "#DTDVersion = " << AcdCalibVersion::dtdVersion() << endl
     << "#fmtVersion = " << AcdCalibVersion::fmtVersion() << endl;
  writeTxtInputInfo(os);
  // this line is needed as a tag
  os << "#START" << endl;
  // skip this line
  writeTxt(os);
  os.close();
  return kTRUE;
}


void AcdCalibMap::writeTxtInputInfo(std::ostream& os) const {
  for ( std::list< std::pair<std::string,std::string> >::const_iterator itr = m_inputs.begin();
	itr != m_inputs.end(); itr++ ) {
    os << "#input" << itr->second.c_str() << "File = " << itr->first.c_str() << std::endl;
  }
}


void AcdCalibMap::writeTxt(ostream& os) const {
  for(int iFace = 0; iFace != AcdKey::nFace; ++iFace) {
    for(int iRow = 0; iRow != (int)AcdKey::getNRow(iFace); ++iRow) {
      for(int iCol = 0; iCol != (int)AcdKey::getNCol(iFace,iRow); ++iCol) {	
	if ( ! AcdKey::channelExists(iFace,iRow,iCol) ) continue;
	for(int iPmt = 0; iPmt != AcdKey::nPmt; ++iPmt) {

	  UInt_t id = AcdKey::makeId(iFace,iRow,iCol);
	  UInt_t key = AcdKey::makeKey(iPmt,iFace,iRow,iCol);
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
				 const char* algorithm ) const {
  
  

  DomElement elem = AcdXmlUtil::makeDocument("acdCalib");
  writeXmlHeader(elem,instrument,timestamp,algorithm);
  writeXmlBody(elem);
  Bool_t ok = AcdXmlUtil::writeIt(elem,fileName);
  if ( ok ) {
    m_fileName = fileName;
  }
  return ok;
}

void AcdCalibMap::writeXmlHeader(DomElement& node,
				 const char* instrument,
				 const char* timestamp,
				 const char* algorithm ) const {  

  DomElement genNode = AcdXmlUtil::makeChildNode(node,"generic");  
  AcdXmlUtil::addAttribute(genNode,"instrument",instrument);
  AcdXmlUtil::addAttribute(genNode,"timestamp",timestamp);
  std::string calibTypeName;
  AcdXmlUtil::getCalibTypeName(calibTypeName,m_desc->calibType());
  AcdXmlUtil::addAttribute(genNode,"calibType",calibTypeName.c_str());
  AcdXmlUtil::addAttribute(genNode,"algorithm",algorithm);
  AcdXmlUtil::addAttribute(genNode,"DTDVersion",AcdCalibVersion::dtdVersion());
  AcdXmlUtil::addAttribute(genNode,"fmtVersion",AcdCalibVersion::fmtVersion());    
  writeXmlInputInfo(genNode);
  DomElement dimNode = AcdXmlUtil::makeChildNode(node,"dimension");
  AcdXmlUtil::addAttribute(dimNode,"nTile",(int)108);
}

void AcdCalibMap::writeXmlInputInfo(DomElement& node) const {
  DomElement sourceNode = AcdXmlUtil::makeChildNode(node,"inputSample");  
  AcdXmlUtil::addAttributeMET(sourceNode,"startTime",m_startTime);
  AcdXmlUtil::addAttributeMET(sourceNode,"stopTime",m_endTime);
  AcdXmlUtil::addAttribute(sourceNode,"triggers",(Int_t)m_triggers);
  AcdXmlUtil::addAttribute(sourceNode,"source","Orbit");
  AcdXmlUtil::addAttribute(sourceNode,"mode","Normal"); 
  for ( std::list< std::pair<std::string,std::string> >::const_iterator itr = m_inputs.begin();
	itr != m_inputs.end(); itr++ ) {
    if ( itr->first == "" ) continue;
    DomElement fileNode = AcdXmlUtil::makeChildNode(sourceNode,"inputFile");  
    AcdXmlUtil::addAttribute(fileNode,"path",itr->first.c_str());
    AcdXmlUtil::addAttribute(fileNode,"type",itr->second.c_str());
  }
}

void AcdCalibMap::writeXmlBody(DomElement& node) const {

  std::string calibElemName;
  AcdXmlUtil::getCalibElemName(calibElemName,m_desc->calibType());

  for(int iFace = 0; iFace != AcdKey::nFace; iFace++) {
    for(int iRow = 0; iRow != (int)AcdKey::getNRow(iFace); ++iRow) {
      for(int iCol = 0; iCol != (int)AcdKey::getNCol(iFace,iRow); ++iCol) {	
	if ( ! AcdKey::channelExists(iFace,iRow,iCol) ) continue;

	UInt_t id = AcdKey::makeId(iFace,iRow,iCol);
	DomElement tileNode = AcdXmlUtil::makeChildNode(node,"tile");
	char tileStr[4];
	sprintf(tileStr,"%03d",id);
	AcdXmlUtil::addAttribute(tileNode,"tileId",tileStr);

	for(int iPmt = 0; iPmt != AcdKey::nPmt; iPmt++) {
	  DomElement pmtNode = AcdXmlUtil::makeChildNode(tileNode,"pmt");
	  AcdXmlUtil::addAttribute(pmtNode,"iPmt",(int)iPmt);
	  
	  UInt_t key = AcdKey::makeKey(iPmt,iFace,iRow,iCol);
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
  
  TFile* histFile(0);
  UInt_t nVal = m_desc->nVar();

  Float_t vals[10][216];

  UInt_t id[216];
  UInt_t pmt[216];
  Int_t status[216];
  if ( newFileName != 0 ) {
    histFile = TFile::Open(newFileName, "RECREATE");
    if ( histFile == 0 ) return kFALSE;
    m_tree = new TTree(m_desc->calibTypeName().c_str(),"Calibration");    
  }
  if( histFile == 0 || m_tree == 0 ) return kFALSE;
  m_tree->Branch("startTime",&m_startTime,"startTime/D");
  m_tree->Branch("endTime",&m_endTime,"endTime/D");
  m_tree->Branch("triggers",&m_triggers,"triggers/i");
  m_tree->Branch("id",&id[0],"id[216]/i");
  m_tree->Branch("pmt",&pmt[0],"pmt[216]/i");
  m_tree->Branch("status",&status[0],"status[216]/I");
  for ( UInt_t i(0); i < nVal; i++ ) {
    std::string vName = m_desc->getVarName(i);
    std::string lName = vName + "[216]/F";
    m_tree->Branch(vName.c_str(),&(vals[i][0]),lName.c_str());
  }
  UInt_t idx(0);
  for ( std::map<UInt_t,CalibData::AcdCalibObj*>::const_iterator itr = m_map.begin(); 
	itr != m_map.end(); itr++, idx++ ) {
    id[idx] = AcdKey::getId(itr->first);
    pmt[idx] =  AcdKey::getPmt(itr->first);
    status[idx] = itr->second->getStatus();
    for ( UInt_t iV(0); iV < nVal; iV++ ) {
      vals[iV][idx] = itr->second->operator[](iV);
    }
  }
  m_tree->Fill();
  m_tree->Write();
  histFile->Close();
  return kTRUE;
}


Bool_t AcdCalibMap::readXmlFile(const char* fileName) {
  using XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument;

  xmlBase::XmlParser parser;
  static const std::string inputName("inputSample");
  static const std::string tileName("tile");
  DOMDocument* doc = parser.parse(fileName);
  if ( doc == 0 ) {
    std::cerr << "XmlParser failed to parse " << fileName << std::endl;
    return kFALSE;
  }

  m_fileName = fileName;

  std::vector<DOMElement*> input;
  std::vector<DOMElement*> tiles;
  
  DOMElement* top = doc->getDocumentElement();					       
  xmlBase::Dom::getDescendantsByTagName(top,inputName,input);
  if ( input.size() != 1 ) {
    std::cerr << "Wrong number of inputSample nodes " << input.size() << std::endl;
    return kFALSE;
  }
  if ( ! readXmlHeader(*(input[0]) ) ) return kFALSE;
  xmlBase::Dom::getDescendantsByTagName(top,tileName,tiles);
  for ( std::vector<DOMElement*>::iterator itr = tiles.begin(); itr!= tiles.end(); itr++ ) {
    if ( ! readXmlTile(*(*itr) ) ) return kFALSE;
  }
  
  return kTRUE;
}

Bool_t AcdCalibMap::readXmlHeader(DOMElement& node) {
  m_startTime = xmlBase::Dom::getDoubleAttribute(&node,"startTime");
  m_endTime = xmlBase::Dom::getDoubleAttribute(&node,"stopTime");
  m_triggers = xmlBase::Dom::getIntAttribute(&node,"triggers");
  std::vector<DOMElement*> inputs;  
  static const std::string inputName("inputFile");
  xmlBase::Dom::getDescendantsByTagName(&node,inputName,inputs);
  for ( std::vector<DOMElement*>::const_iterator itr = inputs.begin(); itr != inputs.end(); itr++ ) {
    std::string path = xmlBase::Dom::getAttribute(*itr,"path");
    std::string type = xmlBase::Dom::getAttribute(*itr,"type");
    addInput(path,type);
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
  UInt_t keyA = AcdKey::makeKey(0,id);
  UInt_t keyB = AcdKey::makeKey(1,id);

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
    std::cerr << "From AcdCalibMap::readXmlCalib for " << m_fileName << " at pmt element with key: " << key << std::endl;
    std::cerr << ex.getMsg() << std::endl;
    for ( int iO(0); iO < m_desc->nVar(); iO++ ) {
      std::cerr << "\t" << m_desc->getVarName(iO) << std::endl;
    }
    return kFALSE;
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
    UInt_t key = AcdKey::makeKey(iPmt,id);

    CalibData::AcdCalibObj* result = makeNew();
    assert( result != 0 );

    result->readTxt(is,*m_desc);
    if ( !is.good() ) return kFALSE;
    
    m_map[key] = result;
  }
  return kTRUE;
}


Bool_t AcdCalibMap::readTree() const {
  TString rootFileName(fileName());
  rootFileName.ReplaceAll(".xml","Pars.root");
  TFile* f = TFile::Open(rootFileName);
  if ( f == 0 ) return kFALSE;
  TObject* obj = f->Get( m_desc->calibTypeName().c_str() );
  if ( obj == 0 ) return kFALSE;
  m_tree = dynamic_cast<TTree*>(obj);
  if ( m_tree == 0 ) return kFALSE;
  return kTRUE;
}
