#define AcdGainFit_cxx

#include "AcdHistCalibMap.h"
#include "AcdGainFit.h"
#include "AcdMap.h"

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

Bool_t AcdCalibMap::writeTxtFile(const char* fileName) const {
  std::ofstream gain(fileName);
  if ( !gain.good() ) {
    std::cerr << "Problems opening text output file " << fileName << std::endl;
    return kFALSE;
  }
  writeTxt(gain);
  gain.close();
  return kTRUE;
}

void AcdCalibMap::writeTxt(ostream& gain) const {
  for(int iFace = 0; iFace != AcdMap::nFace; ++iFace) {
    for(int iRow = 0; iRow != AcdMap::nRow; ++iRow) {
      for(int iCol = 0; iCol != AcdMap::nCol; ++iCol) {	
	if ( ! AcdMap::channelExists(iFace,iRow,iCol) ) continue;
	for(int iPmt = 0; iPmt != AcdMap::nPmt; ++iPmt) {
	  for(int iRange = 0; iRange != AcdMap::nRange; ++iRange) {

	    UInt_t key = AcdMap::makeKey(iRange,iPmt,iFace,iRow,iCol);
	    std::map<UInt_t,AcdCalibResult*>::const_iterator itr = m_map.find(key);   
	    if ( itr == m_map.end() ) continue;

	    gain << iFace << ' ' << iRow << ' ' << iCol << ' ' 
		 << iPmt << ' ' << iRange << ' ';
	    itr->second->printTxtLine(gain);
	    gain << std::endl;

	    /* Float_t pedValue = itr == m_map.end() ? 0. : itr->second->ped();
	    //Float_t gainPeak = itr == m_map.end() ? -9999. : itr->second->peak();	    
	    //Float_t gainWidth = itr == m_map.end() ? -9999. : itr->second->width();	    
	    //AcdCalibResult::STATUS status = itr == m_map.end() ? AcdCalibResult::NOFIT : itr->second->status();
	    //Int_t type = itr == m_map.end() ? 0 : itr->second->type();	    

	    //gain << iFace << ' ' << iRow << ' ' << iCol << ' ' 
	    //	 << iPmt << ' ' << iRange << ' ' 
	    // << pedValue << ' '
	    // << gainPeak << ' '
	    // << gainWidth << ' '
	    // << status << ' '
	    // << type << ' ' 
	    // << std::endl;	    
	    */
	  }
	}
      }
    }
  }
}


Bool_t AcdCalibMap::writeXmlFile(const char* fileName, 
				   const char* system,
				   const char* instrument,
				   const char* timestamp,
				   const char* version) const {
  
  std::ofstream gain(fileName);
  if ( !gain.good() ) {
    std::cerr << "Problems opening XML output file " << fileName << std::endl;
    return kFALSE;
  }
  writeXmlHeader(gain,system,instrument,timestamp,version);
  writeXmlBody(gain);
  writeXmlFooter(gain);
  gain.close();
  return kTRUE;
}

void AcdCalibMap::writeXmlHeader(ostream& gain, 
				   const char* system,
				   const char* instrument,
				   const char* timestamp,
				   const char* version) const {  
  using std::endl;
  gain << "<!DOCTYPE acdCalib SYSTEM \"" << system << "\" [] >" << endl;
  gain << "<acdCalib>" << endl;
  gain << "<generic instrument=\"" << instrument 
       << "\" timestamp=\"" << timestamp 
       << "\" calibType=\"ACD_ElecGain\" "
       << "fmtVersion=\"" << version 
       << "\">" << endl;\
  gain << "  <inputSample startTime=\"" << 0
       << "\" stopTime=\"" << 0
       << "\" triggers=\"" << 0
       << "\" source=\"" << 0
       << "\" mode=\"" << 0
       << "\"/>" << endl;
  gain << "</generic>" << endl;
  gain << "<dimension nFace=\"7\" nRow=\"5\" nCol=\"5\"/>" << endl;    
}


void AcdCalibMap::writeXmlFooter(ostream& gain) const {
  using std::endl;
  gain << "</acdCalib>" << endl;
}

void AcdCalibMap::writeXmlBody(ostream& gain) const {

  using std::endl;

  for(int iFace = 0; iFace != AcdMap::nFace; iFace++) {
    AcdMap::nSpaces(gain,2); gain << "<face iFace=\"" << iFace << "\">" << endl; 
    for(int iRow = 0; iRow != AcdMap::nRow; iRow++) {
      AcdMap::nSpaces(gain,4); gain << "<row iRow=\"" << iRow << "\">" << endl; 
      for(int iCol = 0; iCol != AcdMap::nCol; iCol++) {
	AcdMap::nSpaces(gain,6); gain << "<col iCol=\"" << iCol << "\">" << endl; 
	for(int iPmt = 0; iPmt != AcdMap::nPmt; iPmt++) {
	  AcdMap::nSpaces(gain,8); gain << "<pmt iPmt=\"" << iPmt << "\">" << endl; 
	  for(int iRange = 0; iRange != AcdMap::nRange; iRange++) {
	    
	    UInt_t key = AcdMap::makeKey(iRange,iPmt,iFace,iRow,iCol);
	    std::map<UInt_t,AcdCalibResult*>::const_iterator itr = m_map.find(key);   
	    if ( itr == m_map.end() ) continue;

	    //Float_t gainPeak = itr == m_map.end() ? -9999. : itr->second->peak();
	    //Float_t gainWidth = itr == m_map.end() ? -9999. : itr->second->width();	    
	    
	    AcdMap::nSpaces(gain,10); 

	    itr->second->printXmlLine(gain);
	    //gain << "<acdGain range=\"" << iRange
	    //			   << "\" avg=\"" << gainPeak
	    //			   << "\" sig=\" " << gainWidth
	    //
	    //<< "\"/>" << endl;
	    gain << endl;
	  }
	  AcdMap::nSpaces(gain,8); gain << "</pmt>" << endl;
	}
	AcdMap::nSpaces(gain,6); gain << "</col>" << endl;
      }
      AcdMap::nSpaces(gain,4); gain << "</row>" << endl;
    }
    AcdMap::nSpaces(gain,2); gain << "</face>" << endl;
  }
}


Bool_t AcdCalibMap::readTxtFile(const char* fileName) {
  std::ifstream gain(fileName);
  if ( ! gain.good() ) return kFALSE;
  Bool_t result = readTxt(gain);
  gain.close();
  return result;
}

Bool_t AcdCalibMap::readTxt(istream& gain) {
  int iFace, iRow, iCol, iPmt, iRange;
  while ( ! gain.eof() ) { 

    gain >> iFace >> iRow >> iCol >> iPmt >> iRange;

    if ( !gain.good() ) return kFALSE;
    UInt_t key = AcdMap::makeKey(iRange,iPmt,iFace,iRow,iCol);

    AcdCalibResult* result = createHolder();
    assert( result != 0 );

    result->readTxt(gain);
    if ( !gain.good() ) return kFALSE;
    
    m_map[key] = result;
  }
  return kTRUE;
}
