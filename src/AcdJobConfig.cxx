#include <fstream>
#include "AcdJobConfig.h"

#include <stdlib.h>
#include <string>
#include <fstream>

#include "TSystem.h"
#include "xmlBase/IFile.h"
#include "facilities/Util.h"
#include "facilities/XGetopt.h"
#include <time.h>

#include "TChain.h"

ClassImp(AcdJobConfig) ;

AcdJobConfig::AcdJobConfig(const char* appName, const char* desc)
  :m_theApp(appName),
   m_description(desc),
   m_config(AcdMap::LAT),
   m_optval_n(0),
   m_optval_s(0),
   m_optval_b(300),
   m_optval_P(kFALSE),
   m_optval_L(kFALSE),
   m_digiChain(0),
   m_reconChain(0),
   m_svacChain(0),
   m_meritChain(0)
{

}

AcdJobConfig::~AcdJobConfig() 
{
  delete m_digiChain;
  delete m_reconChain;
  delete m_svacChain; 
  delete m_meritChain;
}

void AcdJobConfig::usage() {

  using std::cout;
  using std::cerr;
  using std::endl;
  
  cout << endl
       << m_theApp << endl
       << m_description << endl
       << endl;
  
  cout << "Usage:" << endl
       << "\t" << m_theApp << " -c <configFile>" << endl 
       << endl
       << "\t   <configFile>      : name of xml file with job configuration" << endl
       << endl
       << "\t" << m_theApp << " [options] [input] -o <output>" << endl 
       << endl
       << "\tINPUT" << endl
       << "\t   -r <reconFiles>   : comma seperated list of recon ROOT files" << endl
       << "\t   -d <digiFiles>    : comma seperated list of digi ROOT files" << endl
       << "\t   -S <svacFiles>    : comma seperated list of svac ROOT files" << endl
       << "\t   -m <meritFiles>   : comma seperated list of merit ROOT files" << endl 
       << "\tNOTE:  Different calibrations jobs take diffenent types of input files" << endl
       << endl
       << "\t   -o <output>       : prefix (path or filename) to add to output files" << endl
       << endl
       << "\tOPTIONS for all jobs" << endl
       << "\t   -h                : print this message" << endl
       << "\t   -n <nEvents>      : run over <nEvents>" << endl
       << "\t   -s <startEvent>   : start with event <startEvent>" << endl
       << "\t   -I (LAT | CU06)   : specify instument being calibrated [LAT]" << endl
       << endl
       << "\tOPTIONS for specific jobs (will be ignored by other jobs)"  << endl
       << "\t   -P                : use only periodic triggers" << endl
       << "\t   -L                : correct for pathlength in tile" << endl
       << "\t   -b <bins>         : number of time bins in strip chart [300]" << endl   
       << "\t   -p <pedFile>      : use pedestals from this file" << endl
       << "\t   -g <gainFile>     : use gains from this file" << endl
       << endl;
}
  
Int_t AcdJobConfig::parse(int argn, char** argc) {

  using std::cout;
  using std::cerr;
  using std::endl;
  using std::string;

  // grab path from env
  m_path = ::getenv("CALIBGENACDROOT");
  m_jobOptionXmlFile = m_path + string("/src/calibOption.xml");
  
  // parse options
  char* endPtr;  
  int opt;
  while ( (opt = getopt(argn, argc, "ho:d:r:S:m:p:g:PLc:I:n:s:b:")) != EOF ) {
    switch (opt) {
    case 'h':   // help      
      usage();
      return 1;
    case 'o':   //  output
      m_outputPrefix = string(optarg);
      break;
    case 'd':   // digi files
      m_inputDigiFileStr += string(optarg);
      m_inputDigiFileStr += ',';
      break;
    case 'r':   // recon files
      m_inputReconFileStr += string(optarg);
      m_inputReconFileStr += ',';
      break;
    case 'S':   // Svac files
      m_inputSvacFileStr += string(optarg);
      m_inputSvacFileStr += ',';
      break;
    case 'm':   // Merit files
      m_inputMeritFileStr += string(optarg);
      m_inputMeritFileStr += ',';
      break;
    case 'p':   // pedestals
      m_pedFileName += string(optarg);
      break;
    case 'g':   // gains
      m_gainFileName += string(optarg);
      break;
    case 'P':   // periodic only
      m_optval_P = kTRUE;
      break;
    case 'L':   // pathlength correction
      m_optval_L = kTRUE;
      break;
    case 'c':   // config file
      m_jobOptionXmlFile = string(optarg);
      break;
    case 'I':   // Instrument
      m_instrument = string(optarg);
      break;
    case 'b':   // number of bins
      m_optval_b = strtoul( optarg, &endPtr, 0 );
      break;
    case 'n':   // number of events
      m_optval_n = strtoul( optarg, &endPtr, 0 );
      break;
    case 's':   // start event
      m_optval_s = strtoul( optarg, &endPtr, 0 );
      break;
    case '?':
      usage();
      return 2;
    default:
      cerr << opt << " not parsable..." << endl;
      cerr << "Try " << m_theApp << " -h" << endl;
      return 2;
    }
  }

  if ( argn - optind > 0 ) {
    cerr << m_theApp << " only takes options, not bare arguments" << endl
	 << "Try " << m_theApp << " -h" << endl;
    return 3;
  }

  // parse xml config file
  using xmlBase::IFile;
  IFile myFile(m_jobOptionXmlFile.c_str()); 

  // periodic triggers only
  if (myFile.contains("parameters","requirePeriodic") ) {
    m_optval_P = kTRUE;
  }

  // pathlength correction
  if (myFile.contains("parameters","pathLengthCorrection") ) {
    m_optval_L = kTRUE;
  }
  
  // instrument
  if (myFile.contains("parameters","instrument") && m_instrument == "") {
    m_instrument = myFile.getString("parameters", "instrument");    
  }

  // pedestal file
  if (myFile.contains("parameters","pedestalFile")  && m_pedFileName == "" ) {
    m_pedFileName = myFile.getString("parameters", "pedestalFile");
  } 

  // pain file
  if (myFile.contains("parameters","gainFile")  && m_gainFileName == "" ) {
    m_gainFileName = myFile.getString("parameters", "gainFile");
  }

  // output file prefix
  if (myFile.contains("parameters","outputPrefix") && m_outputPrefix == "" ) {
    m_outputPrefix = myFile.getString("parameters", "outputPrefix");
  }
    
  // timestamp
  time_t theTime = time(0);
  const char* timeString = ctime(&theTime);

  m_timeStamp = string(timeString);
  m_timeStamp.erase(m_timeStamp.size()-1);

  if ( m_instrument == std::string("LAT") || 
       m_instrument == std::string("") ) {
    m_config = AcdMap::LAT;
  } else if ( m_instrument == std::string("CU06") ) {
    m_config = AcdMap::BEAM;
  } else {
    std::cerr << "Invalid instrument: " << m_instrument << std::endl
	      << "Valid choice are LAT and CU06" << std::endl;
    return 3;
  }

  // digi files
  if (myFile.contains("parameters","digiFileList")) {
    m_inputDigiFileStr += myFile.getString("parameters", "digiFileList");
  }
  
  if ( m_inputDigiFileStr != "" ) {
    cout << "Input digi files:" << endl;
    m_digiChain = makeChain("Digi",m_inputDigiFileStr);
  }    


  // recon files
  if (myFile.contains("parameters","reconFileList")) {
    m_inputReconFileStr += myFile.getString("parameters", "reconFileList");
  }
  
  if ( m_inputReconFileStr != "" ) {
    cout << "Input recon files:" << endl;
    m_reconChain = makeChain("Recon",m_inputReconFileStr);
  }    


  // svac files
  if (myFile.contains("parameters","svacFileList")) {
    m_inputSvacFileStr += myFile.getString("parameters", "svacFileList");
  }
  
  if ( m_inputSvacFileStr != "" ) {
    cout << "Input svac files:" << endl;
    m_svacChain = makeChain("Output",m_inputSvacFileStr);
  }    

  // merit files
  if (myFile.contains("parameters","meritFileList")) {
    m_inputMeritFileStr += myFile.getString("parameters", "meritFileList");
  }
  
  if ( m_inputMeritFileStr != "" ) {
    cout << "Input merit files:" << endl;
    m_meritChain = makeChain("MeritTuple",m_inputMeritFileStr);
  }    

  std::cout << "output file prefix: " << m_outputPrefix << std::endl;
  std::cout << "instrument: " << m_instrument << std::endl;
  std::cout << "timestamp: " << m_timeStamp << std::endl;
  if ( m_optval_P ) {
    std::cout << "Using only periodic triggers" << std::endl;
  }
  if ( m_optval_L ) {
    std::cout << "Using pathlength correction" << std::endl;
  }
  if ( m_pedFileName != "" ) {
    std::cout << "pedestal file: " << m_pedFileName << std::endl;
  }
  if ( m_gainFileName != "" ) {   
    std::cout << "gain file: " << m_gainFileName << std::endl;
  }

  return 0;
}


TChain* AcdJobConfig::makeChain(const char* name, const std::string& fileString) const {

  TChain* chain(0);
  std::vector <std::string> token;
  facilities::Util::stringTokenize(fileString, ";, ", token);
  unsigned int nFiles = token.size();
  
  for ( unsigned int iFile(0); iFile < nFiles; iFile++ ) {
    if (token[iFile]=="") continue;
    if ( chain == 0 ) chain = new TChain(name);
    chain->Add(token[iFile].c_str());
    std::cout << "   " << iFile+1 << ") " << token[iFile] << std::endl;
  }
  return chain;
  
}


Bool_t AcdJobConfig::checkDigi() const {
  if ( m_digiChain == 0 ) {
    std::cerr << "This job requires digi ROOT files as input." << std::endl
	      << "\tuse -d <file> option to specify them." << std::endl
	      << std::endl;
    return kFALSE;
  }
  return kTRUE;
}

Bool_t AcdJobConfig::checkRecon() const {
  if ( m_reconChain == 0 ) {
    std::cerr << "This job requires recon ROOT files as input." << std::endl
	      << "\tuse -r <file> option to specify them." << std::endl
	      << std::endl;
    return kFALSE;
  }
  return kTRUE;
}

Bool_t AcdJobConfig::checkSvac() const {
  if ( m_svacChain == 0 ) {
    std::cerr << "This job requires svac ROOT files as input." << std::endl
	      << "\tuse -S <file> option to specify them." << std::endl
	      << std::endl;
    return kFALSE;
  }
  return kTRUE;
}

Bool_t AcdJobConfig::checkMerit() const {
  if ( m_meritChain == 0 ) {
    std::cerr << "This job requires merit ROOT files as input." << std::endl
	      << "\tuse -m <file> option to specify them." << std::endl
	      << std::endl;
    return kFALSE;
  }
  return kTRUE;
}
