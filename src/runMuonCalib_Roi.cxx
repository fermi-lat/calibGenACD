#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>
#include "AcdMuonRoiCalib.h"
#include "TSystem.h"
#include "xmlBase/IFile.h"
#include "facilities/Util.h"

#include "AcdGainFitLibrary.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::ifstream;

int main(int argn, char** argc) {

  string path = ::getenv("CALIBGENACDROOT");

  string jobOptionXmlFile(path + "/src/muonCalibOption.xml");

  if(argn > 1) {
    jobOptionXmlFile = argc[1];
    cout << "Reading in user-specified options file: " << jobOptionXmlFile 
	 << std::endl << endl;
  }

  xmlBase::IFile myFile(jobOptionXmlFile.c_str());

  string inputDigiFileStr;
  if (myFile.contains("parameters","digiFileList")) {
    inputDigiFileStr = myFile.getString("parameters", "digiFileList");
  }

  std::vector <std::string> token;
  facilities::Util::stringTokenize(inputDigiFileStr, ";, ", token);
  unsigned int nFiles = token.size();
  TChain* digiChain(0);

  unsigned i;
  cout << "Input digi files:" << endl;
  for (i=0; i!=nFiles; ++i) {
    if (token[i]=="") break;
    if ( digiChain == 0 ) digiChain = new TChain("Digi");
    digiChain->Add(token[i].c_str());
    cout << "   " << i+1 << ") " << token[i] << endl;
  }

  string inputMeritFileStr;
  if (myFile.contains("parameters","meritFileList")) {
    inputMeritFileStr = myFile.getString("parameters", "meritFileList");
  }

  token.resize(0);
  facilities::Util::stringTokenize(inputMeritFileStr, ";, ", token);
  nFiles = token.size();
  
  TChain* meritChain(0);
  cout << "Input merit files:" << endl;
  for (i=0; i!=nFiles; ++i) {
    if (token[i]=="") break;
    if ( meritChain == 0 ) meritChain = new TChain("MeritTuple");
    meritChain->Add(token[i].c_str());
    cout << "   " << i+1 << ") " << token[i] << endl;
  }

  string pedTextFile, pedXmlFile;
  if (myFile.contains("parameters","pedTextFile")) {
    pedTextFile = myFile.getString("parameters", "pedTextFile");
    cout << "pedestal text file: " << pedTextFile << endl;
  }

  string gainTextFile, gainXmlFile;
  if (myFile.contains("parameters","gainTextFile")) {
    gainTextFile = myFile.getString("parameters", "gainTextFile");
    cout << "gain text file: " << gainTextFile << endl;
  }
  if (myFile.contains("parameters","gainXmlFile")) {
    gainXmlFile = myFile.getString("parameters", "gainXmlFile");
    cout << "gain xml file: " << gainXmlFile << endl;
  }

  string outputHistFile;
  if (myFile.contains("parameters","outputHistFile")) {
    outputHistFile = myFile.getString("parameters", "outputHistFile");
    cout << "output histogram root file: " << outputHistFile << endl;
  }

  string instrument;
  if (myFile.contains("parameters","instrument")) {
    instrument = myFile.getString("parameters", "instrument");
    cout << "instrument: " << instrument << endl;
  }

  string timeStamp;
  if (myFile.contains("parameters","timeStamp")) {
    timeStamp = myFile.getString("parameters", "timeStamp");
    cout << "timeStamp: " << timeStamp << endl;
  }

  string fmtVersion;
  if (myFile.contains("parameters","fmtVersion")) {
    timeStamp = myFile.getString("parameters", "fmtVersion");
    cout << "format version: " << fmtVersion << endl;
  }

  cout << digiChain << ' ' << meritChain << endl;
  AcdMuonRoiCalib r(digiChain, meritChain, outputHistFile.c_str());
  //r.setInstrument(instrument.c_str());
  //r.setTimeStamp(timeStamp.c_str());
  //r.setVersion(fmtVersion.c_str());

  //r.setCalType(muonCalib::PEDESTAL);        
  //r.go();
  //r.fitPedHist();
  //r.writePedTxt(pedTextFile.c_str());
  //r.writePedXml(pedXmlFile.c_str());

  r.readPedestals(pedTextFile.c_str());
  r.rewind();
  r.setCalType(AcdCalibBase::GAIN);        
  r.go(5000000);    

  AcdGainFitLibrary gainFitter(AcdGainFitLibrary::P7);
  r.fitGains(gainFitter);

  r.writeHistograms(AcdCalibBase::GAIN, outputHistFile.c_str());
  r.getGains()->writeTxtFile(gainTextFile.c_str());
  r.getGains()->writeXmlFile(gainXmlFile.c_str(),"AcdCalib",instrument.c_str(),timeStamp.c_str(),fmtVersion.c_str());

  delete digiChain;
  delete meritChain;

  return 0;
}







