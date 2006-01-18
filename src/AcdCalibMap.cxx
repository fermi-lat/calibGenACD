#define AcdGainFit_cxx

#include "AcdHistCalibMap.h"
#include "AcdGainFit.h"
#include "AcdMap.h"
#include "AcdCalibVersion.h"
#include "AcdCalibBase.h"

#include <iostream>
#include <fstream>

ClassImp(AcdCalibMap) ;

AcdCalibMap::AcdCalibMap(){;}

AcdCalibMap::~AcdCalibMap() {
  for ( std::map<UInt_t,AcdCalibResult*>::iterator itr = m_map.begin();
	itr != m_map.end(); itr++ ) {
    delete itr->second;
  }
  m_map.clear();
}

void AcdCalibMap::add(UInt_t key, AcdCalibResult& result) {
  std::map<UInt_t,AcdCalibResult*>::const_iterator itr = m_map.find(key);
  if ( itr != m_map.end() ) {
    // warn
    ;
  }
  m_map[key] = &result;
}

const AcdCalibResult* AcdCalibMap::get(UInt_t key) const {
  std::map<UInt_t,AcdCalibResult*>::const_iterator itr = m_map.find(key);
  return itr == m_map.end()  ? 0 : itr->second;
}

AcdCalibResult* AcdCalibMap::get(UInt_t key) {
  std::map<UInt_t,AcdCalibResult*>::iterator itr = m_map.find(key);
  return itr == m_map.end()  ? 0 : itr->second;
}

Bool_t AcdCalibMap::writeTxtFile(const char* fileName,
				 const char* instrument,
				 const char* timestamp,
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
     << "#calibType = " << calibType() << endl
     << "#fmtVersion = " << AcdCalibVersion::version() << endl
     << "#startTime = " << calib.runId_first() << ':'  << calib.evtId_first() << endl
     << "#stopTime = " << calib.runId_last() << ':'  << calib.evtId_last() << endl
     << "#triggers = " << calib.nUsed() << '/' << calib.nTrigger() << endl;
  //calib.printFilesTxt(os);
  os << "#source = " << 0 << endl
     << "#mode = " << 0 << endl;

  // this line is needed as a tag
  os << "#START" << endl;
  // skip this line
  os << txtFormat() << endl;
    
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
	  std::map<UInt_t,AcdCalibResult*>::const_iterator itr = m_map.find(key);   
	  if ( itr == m_map.end() ) continue;

	  os << id << ' ' << iPmt << ' ';
	  itr->second->printTxtLine(os);
	  os << std::endl;
	}
      }
    }
  }
}


Bool_t AcdCalibMap::writeXmlFile(const char* fileName,
				 const char* instrument,
				 const char* timestamp,
				 const AcdCalibBase& calib) const {
  
  std::ofstream os(fileName);
  if ( !os.good() ) {
    std::cerr << "Problems opening XML output file " << fileName << std::endl;
    return kFALSE;
  }
  writeXmlHeader(os,instrument,timestamp,calib);
  writeXmlBody(os);
  writeXmlFooter(os);
  os.close();
  return kTRUE;
}

void AcdCalibMap::writeXmlHeader(ostream& os,
				 const char* instrument,
				 const char* timestamp,
				 const AcdCalibBase& calib) const {  
  using std::endl;
  os << "<!DOCTYPE acdCalib SYSTEM \"" << "acdCalib" << '_' << AcdCalibVersion::version()  << ".dtd\" [] >" << endl;
  os << "<acdCalib>" << endl;
  os << "<generic instrument=\"" << instrument 
     << "\" timestamp=\"" << timestamp 
     << "\" calibType=\"" << calibType() 
     << "\" fmtVersion=\"" << AcdCalibVersion::version()
     << "\">" << endl;\
  os << "  <inputSample startTime=\"" << calib.runId_first() << ':'  << calib.evtId_first() 
     << "\" stopTime=\"" << calib.runId_last() << ':'  << calib.evtId_last() 
     << "\" triggers=\"" << calib.nUsed() << '/' << calib.nTrigger()
     << "\" source=\"" << 0
     << "\" mode=\"" << 0
     << "\"/>" << endl;
  os << "</generic>" << endl;
  os << "<dimension nTile=\"216\"/>" << endl;    
}


void AcdCalibMap::writeXmlFooter(ostream& os) const {
  using std::endl;
  os << "</acdCalib>" << endl;
}

void AcdCalibMap::writeXmlBody(ostream& os) const {

  using std::endl;

  for(int iFace = 0; iFace != AcdMap::nFace; iFace++) {
    for(int iRow = 0; iRow != (int)AcdMap::getNRow(iFace); ++iRow) {
      for(int iCol = 0; iCol != (int)AcdMap::getNCol(iFace,iRow); ++iCol) {	
	if ( ! AcdMap::channelExists(iFace,iRow,iCol) ) continue;
	UInt_t id = AcdMap::makeId(iFace,iRow,iCol);
	AcdMap::nSpaces(os,2); os << "<tile tileId=\"" << id << "\">" << endl; 
	for(int iPmt = 0; iPmt != AcdMap::nPmt; iPmt++) {
	  AcdMap::nSpaces(os,4); os << "<pmt iPmt=\"" << iPmt << "\">" << endl; 
	  
	  UInt_t key = AcdMap::makeKey(iPmt,iFace,iRow,iCol);
	  std::map<UInt_t,AcdCalibResult*>::const_iterator itr = m_map.find(key);   
	  if ( itr == m_map.end() ) continue;
	  
	  AcdMap::nSpaces(os,6); 
	  itr->second->printXmlLine(os);

	  AcdMap::nSpaces(os,4); os << "</pmt>" << endl;
	}
	AcdMap::nSpaces(os,2); os << "</tile>" << endl;
      }
    }
  }
}


Bool_t AcdCalibMap::readTxtFile(const char* fileName) {
  std::ifstream infile(fileName);
  if ( ! infile.good() ) return kFALSE;

  bool foundTag(kFALSE);
  while ( ! infile.eof() ) {
    std::string aLine;
    infile >> aLine;
    if ( !infile.good() ) return kFALSE;
    
    //look for the tag to start
    if ( aLine.find("#START") ) {
      foundTag = kTRUE;      
      break;
    }
  }

  if ( !foundTag ) return kFALSE;
  std::string formatLine;
  infile >> formatLine;

  Bool_t result = readTxt(infile);
  infile.close();
  return result;
}

Bool_t AcdCalibMap::readTxt(istream& is) {
  int id, iPmt;
  while ( ! is.eof() ) { 

    is >> id >> iPmt;
    if ( !is.good() ) return kFALSE;
    UInt_t key = AcdMap::makeKey(iPmt,id);

    AcdCalibResult* result = createHolder();
    assert( result != 0 );

    result->readTxt(is);
    if ( !is.good() ) return kFALSE;
    
    m_map[key] = result;
  }
  return kTRUE;
}
