#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>

#include "TSystem.h"
#include "xmlBase/IFile.h"
#include "facilities/Util.h"

#include "../src/AcdMuonRoiCalib.h"
#include "../src/AcdPedestalFitLibrary.h"

#include <time.h>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::ifstream;


void usage(const char* argv) {
  cout << argv << endl;
  cout << "This utility runs the pedestal calibration code" << endl 
       << endl;
  
  cout << "Usage:" << endl
       << "\t" << "runPedestal.exe" << " -c <configFile>" << endl 
       << endl
       << "\t   <configFile>      : name of xml file with job configuration" << endl
       << endl
       << "\t" << "runPedestal.exe" << " [options] -d <digiFiles> -o <output>" << endl 
       << endl
       << "\t   <digiFiles>       : comma seperated list of digi ROOT files" << endl
       << "\t   <output>          : prefix (path or filename) to add to output files" << endl
       << endl
       << "\tOPTIONS" << endl
       << "\t   -h                : print this message" << endl
       << "\t   -p                : use only periodic triggers" << endl
       << "\t   -I <Instrument>   : specify instument being calibrated" << endl
       << "\t   -n <nEvents>      : run over <nEvents>" << endl
       << "\t   -s <startEvent>   : start with event <startEvent>" << endl
       << endl;
}

int main(int argn, char** argc) {

  string path = ::getenv("CALIBGENACDROOT");

  string jobOptionXmlFile(path + "/src/muonCalibOption.xml");

  string inputDigiFileStr;
  string outputPrefix;
  string instrument;
  int optval_n(0);
  int optval_s(0);
  bool optval_p(false);

  char* endPtr;
  
  int opt;
  while ( (opt = getopt(argn, argc, "ho:d:c:I:n:s:p")) != EOF ) {
    switch (opt) {
    case 'h':
      usage(argc[0]);
      return 0;
    case 'o':
      outputPrefix = string(optarg);
      break;
    case 'd':
      inputDigiFileStr += string(optarg);
      inputDigiFileStr += ',';
      break;
    case 'c':
      jobOptionXmlFile = string(optarg);
      break;
    case 'I':
      instrument = string(optarg);
      break;
    case 'n':
      optval_n = strtoul( optarg, &endPtr, 0 );
      break;
    case 's':
      optval_s = strtoul( optarg, &endPtr, 0 );
      break;
    case 'p':
      optval_p = true;
      break;
    case '?':
      usage(argc[0]);
      return 0;
    default:
      std::cerr << argc[0] << " not parsable..." << endl;
    }
  }    

  xmlBase::IFile myFile(jobOptionXmlFile.c_str());

  if (myFile.contains("parameters","digiFileList")) {
    inputDigiFileStr += myFile.getString("parameters", "digiFileList");
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

  string pedTextFile, pedXmlFile, outputHistFile;
  if (myFile.contains("parameters","outputPrefix") && outputPrefix == "") {
    outputPrefix = myFile.getString("parameters", "outputPrefix");
  }
  
  pedTextFile = outputPrefix + "_ped.txt";
  pedXmlFile = outputPrefix + "_ped.xml";
  outputHistFile = outputPrefix + "_ped.root";

  if (myFile.contains("parameters","instrument") && instrument == "") {
    instrument = myFile.getString("parameters", "instrument");
  }

  if (myFile.contains("parameters","requirePeriodic")) {
    optval_p = true;
  }

  std::time_t theTime = std::time(0);
  const char* timeString = std::ctime(&theTime);
  string timeStamp(timeString);
  timeStamp.erase(timeStamp.size()-1);

  cout << "output file prefix: " << outputPrefix << endl;
  cout << "instrument: " << instrument << endl;
  cout << "timestamp: " << timeStamp << endl;
  if ( optval_p ) {
    cout << "Using only periodic triggers" << endl;
  }

  AcdMuonRoiCalib r(digiChain,optval_p);

  r.setCalType(AcdCalibBase::PEDESTAL);
  r.go(optval_n,optval_s);    

  AcdPedestalFitLibrary pedFitter(AcdPedestalFitLibrary::MeanValue);
  AcdPedestalFitMap* peds = r.fitPedestals(pedFitter);

  r.writeHistograms(AcdCalibBase::PEDESTAL, outputHistFile.c_str());
  peds->writeTxtFile(pedTextFile.c_str(),instrument.c_str(),timeStamp.c_str(),pedFitter.algorithm(),r);
  peds->writeXmlFile(pedXmlFile.c_str(),instrument.c_str(),timeStamp.c_str(),pedFitter.algorithm(),r);

  delete digiChain;

  return 0;
}







