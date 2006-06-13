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
#include "../src/AcdStripChart.h"
#include "../src/AcdStripFit.h"

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
       << "\t" << "runStripChart.exe" << " -c <configFile>" << endl 
       << endl
       << "\t   <configFile>      : name of xml file with job configuration" << endl
       << endl
       << "\t" << "runStripChart.exe" << " [options] -d <digiFiles> -o <output>" << endl 
       << endl
       << "\t   <digiFiles>       : comma seperated list of digi ROOT files" << endl
       << "\t   <output>          : prefix (path or filename) to add to output files" << endl
       << endl
       << "\tOPTIONS" << endl
       << "\t   -h                : print this message" << endl
       << "\t   -I <Instrument>   : specify instument being calibrated" << endl
       << "\t   -n <nEvents>      : run over <nEvents>" << endl
       << "\t   -s <startEvent>   : start with event <startEvent>" << endl
       << "\t   -b <bins>         : number of time bins [300]" << endl
       << endl;
}

int main(int argn, char** argc) {

  string path = ::getenv("CALIBGENACDROOT");

  string jobOptionXmlFile(path + "/src/muonCalibOption_strip.xml");

  string inputDigiFileStr;
  string outputPrefix;
  string instrument;
  string pedFileName;

  int optval_n(0);
  int optval_s(0);
  int optval_b(300);

  char* endPtr;  
  int opt;
  while ( (opt = getopt(argn, argc, "ho:d:c:p:I:n:s:b:")) != EOF ) {
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
    case 'p':
      pedFileName = string(optarg);
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
    case 'b':
      optval_b = strtoul( optarg, &endPtr, 0 );
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
  if (myFile.contains("parameters","pedestalFile")  && pedFileName == "" ) {
    pedFileName = myFile.getString("parameters", "pedestalFile");
  }

  string textFile = outputPrefix + "_strip.txt";

  string outputPhaHistFile = outputPrefix + "_PhaStrip.root";
  string outputHitHistFile = outputPrefix + "_HitStrip.root";
  string outputVetoHistFile = outputPrefix + "_VetoStrip.root";  
  
  string outputPhaTxtFile = outputPrefix + "_PhaStrip.txt";
  string outputHitTxtFile = outputPrefix + "_HitStrip.txt";
  string outputVetoTxtFile = outputPrefix + "_VetoStrip.txt";
 
  string outputPhaDeltaFile = outputPrefix + "_PhaDelta.root";
  string outputHitDeltaFile = outputPrefix + "_HitDelta.root";
  string outputVetoDeltaFile = outputPrefix + "_VetoDelta.root";

  std::time_t theTime = std::time(0);
  const char* timeString = std::ctime(&theTime);
  string timeStamp(timeString);
  timeStamp.erase(timeStamp.size()-1);

  cout << "output file prefix: " << outputPrefix << endl;
  cout << "instrument: " << instrument << endl;
  cout << "timestamp: " << timeStamp << endl;
  
  AcdStripChart r(digiChain, optval_b);
  r.setCalType(AcdCalibBase::TIME_PROF_PHA);        
  if ( pedFileName != "" ) {
    r.readPedestals(pedFileName.c_str());
  }
  r.go(optval_n,optval_s);    

  cout << endl;

  r.writeHistograms(AcdCalibBase::TIME_PROF_PHA, outputPhaHistFile.c_str());
  r.writeHistograms(AcdCalibBase::TIME_PROF_HIT, outputHitHistFile.c_str());
  r.writeHistograms(AcdCalibBase::TIME_PROF_VETO, outputVetoHistFile.c_str());
  
  AcdStripFit fitPha("StripPha",AcdCalibUtil::MEAN_ABSOLUTE,100,-50.,50.);  
  AcdStripFit fitHit("StripHit",AcdCalibUtil::MEAN_RELATIVE,100,0.,20.);
  AcdStripFit fitVeto("StripVeto",AcdCalibUtil::MEAN_RELATIVE,100,0.,20.);

  AcdStripFitMap phaStripMap;
  fitPha.fitAll(phaStripMap,*(r.getHistMap(AcdCalibBase::TIME_PROF_PHA)));
  phaStripMap.writeTxtFile(outputPhaTxtFile.c_str(),instrument.c_str(),timeStamp.c_str(),fitPha.algorithm(),r);
  fitPha.fittedHists().writeHistograms(outputPhaDeltaFile.c_str());

  AcdStripFitMap hitStripMap;
  fitHit.fitAll(hitStripMap,*(r.getHistMap(AcdCalibBase::TIME_PROF_HIT)));
  hitStripMap.writeTxtFile(outputHitTxtFile.c_str(),instrument.c_str(),timeStamp.c_str(),fitHit.algorithm(),r);
  fitHit.fittedHists().writeHistograms(outputHitDeltaFile.c_str());

  AcdStripFitMap vetoStripMap;
  fitVeto.fitAll(vetoStripMap,*(r.getHistMap(AcdCalibBase::TIME_PROF_VETO)));
  vetoStripMap.writeTxtFile(outputVetoTxtFile.c_str(),instrument.c_str(),timeStamp.c_str(),fitVeto.algorithm(),r);
  fitVeto.fittedHists().writeHistograms(outputVetoDeltaFile.c_str());

  cout << endl;

  Bool_t ok = phaStripMap.test(-30.,30.,"FAIL!  ","pha time analysis");
  ok = hitStripMap.test(-1.,3.,"FAIL!  ","hit rate time analysis");
  ok = vetoStripMap.test(-1.,3.,"FAIL!  ","veto rate time analysis");

  delete digiChain;

  return 0;
}







