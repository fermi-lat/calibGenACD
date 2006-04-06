#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>

#include "TSystem.h"
#include "xmlBase/IFile.h"
#include "facilities/Util.h"

#include "TList.h"
#include "../src/AcdCalibUtil.h"
#include "../src/AcdVetoFitLibrary.h"
#include "../src/AcdMuonRoiCalib.h"

#include <time.h>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::ifstream;

void usage(const char* argv) {
  cout << argv << endl;
  cout << "This utility runs the muon MIP calibration code" << endl 
       << endl;
  
  cout << "Usage:" << endl
       << "\t" << "runVetoCalib.exe" << " -c <configFile>" << endl 
       << endl
       << "\t   <configFile>      : name of xml file with job configuration" << endl
       << endl
       << "\t" << "runMuonCalib_Roi.exe" << " [options] -d <digiFiles> -o <output>" << endl 
       << endl
       << "\t   <digiFiles>       : comma seperated list of digi ROOT files" << endl
       << "\t   <meritFiles>      : comma seperated list of merit ROOT files" << endl
       << "\t   <output>          : prefix (path or filename) to add to output files" << endl
       << endl
       << "\tOPTIONS" << endl
       << "\t   -h                : print this message" << endl
       << "\t   -I <Instrument>   : specify instument being calibrated" << endl
       << "\t   -n <nEvents>      : run over <nEvents>" << endl
       << "\t   -s <startEvent>   : start with event <startEvent>" << endl
       << endl;
}

int main(int argn, char** argc) {

  string path = ::getenv("CALIBGENACDROOT");

  string jobOptionXmlFile(path + "/src/muonCalibOption_veto.xml");

  string inputDigiFileStr;
  string outputPrefix;
  string instrument;

  int optval_n(0);
  int optval_s(0);

  char* endPtr;
  
  int opt;
  while ( (opt = getopt(argn, argc, "ho:d:c:p:I:n:s:")) != EOF ) {
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
    if (token[i]=="") continue;
    if ( digiChain == 0 ) digiChain = new TChain("Digi");
    digiChain->Add(token[i].c_str());
    cout << "   " << i+1 << ") " << token[i] << endl;
  }

  if (myFile.contains("parameters","instrument") && instrument == "") {
    instrument = myFile.getString("parameters", "instrument");    
  }

  if (myFile.contains("parameters","outputPrefix") && outputPrefix == "" ) {
    outputPrefix = myFile.getString("parameters", "outputPrefix");
  }
    
  string textFile = outputPrefix + "_veto.txt";
  string xmlFile = outputPrefix + "_veto.xml";
  string outputHistFile = outputPrefix + "_veto.root";

  std::time_t theTime = std::time(0);
  const char* timeString = std::ctime(&theTime);
  string timeStamp(timeString);
  timeStamp.erase(timeStamp.size()-1);

  cout << "output file prefix: " << outputPrefix << endl;
  cout << "instrument: " << instrument << endl;
  cout << "timestamp: " << timeStamp << endl;
  

  AcdMuonRoiCalib r(digiChain);
  r.setCalType(AcdCalibBase::VETO);        
  r.go(optval_n,optval_s);    

  r.makeVetoRatio();

  AcdVetoFitLibrary vetoFitter(AcdVetoFitLibrary::Counting);
  AcdVetoFitMap* vetos = r.fitVetos(vetoFitter);

  r.writeHistograms(AcdCalibBase::VETO_FRAC, outputHistFile.c_str());
  vetos->writeTxtFile(textFile.c_str(),instrument.c_str(),timeStamp.c_str(),vetoFitter.algorithm(),r);
  vetos->writeXmlFile(xmlFile.c_str(),instrument.c_str(),timeStamp.c_str(),vetoFitter.algorithm(),r);

  delete digiChain;

  return 0;
}







