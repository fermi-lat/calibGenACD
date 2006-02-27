#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>

#include "TSystem.h"
#include "xmlBase/IFile.h"
#include "facilities/Util.h"

#include "TList.h"
#include "../src/AcdCalibUtil.h"
#include "../src/AcdGainFitLibrary.h"
#include "../src/AcdMuonTkrCalib.h"

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
       << "\t" << "runMuonCalib_Tkr.exe" << " -c <configFile>" << endl 
       << endl
       << "\t   <configFile>      : name of xml file with job configuration" << endl
       << endl
       << "\t" << "runMuonCalib_Tkr.exe" << " [options] -d <digiFiles> -r <reconFiles> -o <output>" << endl 
       << endl
       << "\t   <digiFiles>       : comma seperated list of digi ROOT files" << endl
       << "\t   <reconFiles>      : comma seperated list of recon ROOT files" << endl
       << "\t   <output>          : prefix (path or filename) to add to output files" << endl
       << endl
       << "\tOPTIONS" << endl
       << "\t   -h                : print this message" << endl
       << "\t   -p <pedFile>      : specify the file with the pedestals" << endl
       << "\t   -I <Instrument>   : specify instument being calibrated" << endl
       << "\t   -n <nEvents>      : run over <nEvents>" << endl
       << "\t   -s <startEvent>   : start with event <startEvent>" << endl
       << endl;
}

int main(int argn, char** argc) {

  string path = ::getenv("CALIBGENACDROOT");

  string jobOptionXmlFile(path + "/src/muonCalibOption_gain.xml");

  string inputDigiFileStr;
  string inputReconFileStr;
  string pedFileName;
  string outputPrefix;
  string instrument;

  int optval_n(0);
  int optval_s(0);

  char* endPtr;
  
  int opt;
  while ( (opt = getopt(argn, argc, "ho:d:r:c:p:I:n:s:")) != EOF ) {
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
    case 'r':
      inputReconFileStr += string(optarg);
      inputReconFileStr += ',';
      break;
    case 'c':
      jobOptionXmlFile = string(optarg);
      break;
    case 'p':
      pedFileName = string(optarg);
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

  if (myFile.contains("parameters","reconFileList")) {
    inputReconFileStr += myFile.getString("parameters", "reconFileList");
  }

  if (myFile.contains("parameters","pedestalFile")  && pedFileName == "" ) {
    pedFileName = myFile.getString("parameters", "pedestalFile");
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

  facilities::Util::stringTokenize(inputReconFileStr, ";, ", token);
  nFiles = token.size();
  TChain* reconChain(0);

  cout << "Input recon files:" << endl;
  for (i=0; i!=nFiles; ++i) {
    if (token[i]=="") continue;
    if ( reconChain == 0 ) reconChain = new TChain("Recon");
    reconChain->Add(token[i].c_str());
    cout << "   " << i+1 << ") " << token[i] << endl;
  }

  if (myFile.contains("parameters","instrument") && instrument == "") {
    instrument = myFile.getString("parameters", "instrument");    
  }

  string gainTextFile, gainXmlFile, outputHistFile, psFile;  
  if (myFile.contains("parameters","outputPrefix") && outputPrefix == "" ) {
    outputPrefix = myFile.getString("parameters", "outputPrefix");
  }
    
  gainTextFile = outputPrefix + "_gain.txt";
  gainXmlFile = outputPrefix + "_gain.xml";
  outputHistFile = outputPrefix + "_gain.root";
  psFile = outputPrefix + "_gain_";

  std::time_t theTime = std::time(0);
  const char* timeString = std::ctime(&theTime);
  string timeStamp(timeString);
  timeStamp.erase(timeStamp.size()-1);

  cout << "output file prefix: " << outputPrefix << endl;
  cout << "instrument: " << instrument << endl;
  cout << "timestamp: " << timeStamp << endl;
  cout << "pedestal file: " << pedFileName << endl;

  AcdMuonTkrCalib r(digiChain,reconChain);
  r.setCalType(AcdCalibBase::GAIN);        

  bool removePeds(true);
  if ( pedFileName != "" && reconChain != 0 ) {
    r.readPedestals(pedFileName.c_str());
    removePeds = false;
  }

  r.go(optval_n,optval_s);    

  AcdGainFitLibrary gainFitter(AcdGainFitLibrary::P5,removePeds);
  AcdGainFitMap* gains = r.fitGains(gainFitter);

  r.writeHistograms(AcdCalibBase::GAIN, outputHistFile.c_str());
  gains->writeTxtFile(gainTextFile.c_str(),instrument.c_str(),timeStamp.c_str(),r);
  gains->writeXmlFile(gainXmlFile.c_str(),instrument.c_str(),timeStamp.c_str(),r);

  AcdHistCalibMap* hists = r.getHistMap(AcdCalibBase::GAIN);

  TList cl_log;
  std::string psFile_log = psFile + "log_";
  AcdCalibUtil::drawMips(cl_log,*hists,*gains,kTRUE,psFile_log.c_str());
  AcdCalibUtil::saveCanvases(cl_log);

  TList cl_lin;
  std::string psFile_lin = psFile + "lin_";
  AcdCalibUtil::drawMips(cl_lin,*hists,*gains,kFALSE,psFile_lin.c_str());
  AcdCalibUtil::saveCanvases(cl_lin);  

  delete digiChain;
  delete reconChain;

  return 0;
}







