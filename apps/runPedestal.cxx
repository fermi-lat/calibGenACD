#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>
#include "../src/AcdMuonRoiCalib.h"
#include "TSystem.h"
#include "xmlBase/IFile.h"
#include "facilities/Util.h"

#include "../src/AcdPedestalFitLibrary.h"

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

  string outputPrefix;
  string pedTextFile, pedXmlFile, outputHistFile;
  if (myFile.contains("parameters","outputPrefix")) {
    outputPrefix = myFile.getString("parameters", "outputPrefix");
    cout << "output file prefix: " << outputPrefix << endl;
    pedTextFile = outputPrefix + "_ped.txt";
    pedXmlFile = outputPrefix + "_ped.xml";
    outputHistFile = outputPrefix + "_ped.root";
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

  string fmtVersion;
  if (myFile.contains("parameters","fmtVersion")) {
    fmtVersion = myFile.getString("parameters", "fmtVersion");
    cout << "format version: " << fmtVersion << endl;
  }

  AcdMuonRoiCalib r(digiChain, 0, outputHistFile.c_str());

  r.setCalType(AcdCalibBase::PEDESTAL);
  r.go();    

  AcdPedestalFitLibrary pedFitter(AcdPedestalFitLibrary::MeanValue);
  r.fitPedestals(pedFitter);

  r.writeHistograms(AcdCalibBase::PEDESTAL, outputHistFile.c_str());
  r.getPedestals()->writeTxtFile(pedTextFile.c_str(),instrument.c_str(),timeStamp.c_str(),r);
  r.getPedestals()->writeXmlFile(pedXmlFile.c_str(),instrument.c_str(),timeStamp.c_str(),r);

  delete digiChain;

  return 0;
}







