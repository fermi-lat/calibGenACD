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
     << "#calibType = " << calibType() << endl
     << "#algorithm = " << algorithm << endl
     << "#DTDVersion = " << AcdCalibVersion::dtdVersion() << endl
     << "#fmtVersion = " << AcdCalibVersion::fmtVersion() << endl;
  calib.writeTxtHeader(os);
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
				 const char* algorithm,
				 const AcdCalibBase& calib) const {
  
  std::ofstream os(fileName);
  if ( !os.good() ) {
    std::cerr << "Problems opening XML output file " << fileName << std::endl;
    return kFALSE;
  }
  writeXmlHeader(os,instrument,timestamp,algorithm,calib);
  writeXmlBody(os);
  writeXmlFooter(os);
  os.close();
  return kTRUE;
}

void AcdCalibMap::writeXmlHeader(ostream& os,
				 const char* instrument,
				 const char* timestamp,
				 const char* algorithm,
				 const AcdCalibBase& calib) const {  
  using std::endl;
  os << "<!DOCTYPE acdCalib SYSTEM \"" << "acdCalib" << '_' << AcdCalibVersion::dtdVersion()  << ".dtd\" [] >" << endl;
  os << "<acdCalib>" << endl;
  os << "<generic instrument=\"" << instrument 
     << "\" timestamp=\"" << timestamp 
     << "\" calibType=\"" << calibType() 
     << "\" algorithm=\"" << algorithm
     << "\" DTDVersion=\"" << AcdCalibVersion::dtdVersion()
     << "\" fmtVersion=\"" << AcdCalibVersion::fmtVersion()
     << "\">" << endl;
  calib.writeXmlHeader(os);
  os << "</generic>" << endl;
  os << "<dimension nTile=\"108\"/>" << endl;    
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

    AcdCalibResult* result = createHolder();
    assert( result != 0 );

    result->readTxt(is);
    if ( !is.good() ) return kFALSE;
    
    m_map[key] = result;
  }
  return kTRUE;
}
