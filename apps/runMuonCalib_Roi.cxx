#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>
#include "../src/AcdMuonRoiCalib.h"
#include "TSystem.h"
#include "xmlBase/IFile.h"
#include "facilities/Util.h"

#include "../src/AcdGainFitLibrary.h"

#include <time.h>

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

  string pedTextFile;
  if (myFile.contains("parameters","pedTextFile")) {
    pedTextFile = myFile.getString("parameters", "pedTextFile");
    cout << "pedestal text file: " << pedTextFile << endl;
  }

  string outputPrefix;
  string gainTextFile, gainXmlFile, outputHistFile;
  if (myFile.contains("parameters","outputPrefix")) {
    outputPrefix = myFile.getString("parameters", "outputPrefix");
    cout << "output file prefix: " << outputPrefix << endl;
    gainTextFile = outputPrefix + "_gain.txt";
    gainXmlFile = outputPrefix + "_gain.xml";
    outputHistFile = outputPrefix + "_gain.root";
  }

  string instrument;
  if (myFile.contains("parameters","instrument")) {
    instrument = myFile.getString("parameters", "instrument");
    cout << "instrument: " << instrument << endl;
  }

  std::time_t theTime = std::time(0);
  const char* timeString = std::ctime(&theTime);
  string timeStamp(timeString);
  timeStamp.erase(timeStamp.size()-1);

  
  AcdMuonRoiCalib r(digiChain, meritChain, outputHistFile.c_str());

  r.readPedestals(pedTextFile.c_str());
  r.setCalType(AcdCalibBase::GAIN);        
  r.go();    

  AcdGainFitLibrary gainFitter(AcdGainFitLibrary::P7);
  r.fitGains(gainFitter);

  r.writeHistograms(AcdCalibBase::GAIN, outputHistFile.c_str());
  r.getGains()->writeTxtFile(gainTextFile.c_str(),instrument.c_str(),timeStamp.c_str(),r);
  r.getGains()->writeXmlFile(gainXmlFile.c_str(),instrument.c_str(),timeStamp.c_str(),r);

  delete digiChain;
  delete meritChain;

  return 0;
}







